/*
 * Test general functionality of software emulated smart card
 *
 * Copyright 2022 Red Hat, Inc.
 *
 * Authors:
 *  Jakub Jelen <jjelen@redhat.com>
 *
 * This code is licensed under the GNU LGPL, version 2.1 or later.
 * See the COPYING file in the top-level directory.
 */
#include <glib.h>
#include "vreader.c"
#include "libcacard.h"
#include "common.h"

#define ARGS "db=\"sql:%s\" use_hw=no soft=(,Test,CAC,,cert1,cert2,cert3)"

static GMainLoop *loop;
static GThread *thread;
static guint nreaders;
static GMutex mutex;
static GCond cond;

static gpointer
events_thread(G_GNUC_UNUSED gpointer arg)
{
    unsigned int reader_id;
    VEvent *event;

    while (1) {
        event = vevent_wait_next_vevent();
        if (event->type == VEVENT_LAST) {
            vevent_delete(event);
            break;
        }
        reader_id = vreader_get_id(event->reader);
        if (reader_id == VSCARD_UNDEFINED_READER_ID) {
            g_mutex_lock(&mutex);
            vreader_set_id(event->reader, nreaders++);
            g_cond_signal(&cond);
            g_mutex_unlock(&mutex);
            reader_id = vreader_get_id(event->reader);
        }
        switch (event->type) {
        case VEVENT_READER_INSERT:
        case VEVENT_READER_REMOVE:
        case VEVENT_CARD_INSERT:
        case VEVENT_CARD_REMOVE:
            break;
        case VEVENT_LAST:
        default:
            g_warn_if_reached();
            break;
        }
        vevent_delete(event);
    }

    return NULL;
}

static void libcacard_init(void)
{
    VCardEmulOptions *command_line_options = NULL;
    gchar *dbdir = g_test_build_filename(G_TEST_DIST, "db", NULL);
    gchar *args = g_strdup_printf(ARGS, dbdir);
    VReader *r;
    VCardEmulError ret;

    thread = g_thread_new("test/events", events_thread, NULL);

    command_line_options = vcard_emul_options(args);
    ret = vcard_emul_init(command_line_options);
    g_assert_cmpint(ret, ==, VCARD_EMUL_OK);

    r = vreader_get_reader_by_name("Test");
    g_assert_nonnull(r);
    vreader_free(r); /* get by name ref */

    g_mutex_lock(&mutex);
    while (nreaders == 0)
        g_cond_wait(&cond, &mutex);
    g_mutex_unlock(&mutex);

    g_free(args);
    g_free(dbdir);
}

static void libcacard_finalize(void)
{
    VReader *reader = vreader_get_reader_by_id(0);

    /* This actually still generates events */
    if (reader) /*if /remove didn't run */
        vreader_remove_reader(reader);

    /* This probably supposed to be a event that terminates the loop */
    vevent_queue_vevent(vevent_new(VEVENT_LAST, reader, NULL));

    /* join */
    g_thread_join(thread);

    /* Clean up */
    vreader_free(reader);

    vcard_emul_finalize();
}

#define MAX_ATR_LEN 100
#define NSS_ATR "\x3B\x89\x00\x56\x43\x41\x52\x44\x5F\x4E\x53\x53"
#define NSS_ATR_LEN (sizeof(NSS_ATR) - 1)
static void test_atr_default(void)
{
    VReader *reader = vreader_get_reader_by_id(0);
    VCard *card;
    unsigned char *atr = g_malloc0(MAX_ATR_LEN);
    int atr_len = MAX_ATR_LEN;

    /* Replace the CAC ATR handled with default one */
    card = vreader_get_card(reader);
    vcard_set_atr_func(card, NULL);
    vcard_free(card);

    vreader_power_off(reader);
    vreader_power_on(reader, atr, &atr_len);
    g_assert_cmpmem(atr, atr_len, NSS_ATR, NSS_ATR_LEN);

    vreader_free(reader); /* get by id ref */
    g_free(atr);
}


int main(int argc, char *argv[])
{
    int ret;

    g_test_init(&argc, &argv, NULL);

    loop = g_main_loop_new(NULL, TRUE);

    libcacard_init();

    g_test_add_func("/atr/default", test_atr_default);

    ret = g_test_run();

    g_main_loop_unref(loop);

    libcacard_finalize();
    return ret;
}

/* vim: set ts=4 sw=4 tw=0 noet expandtab: */
