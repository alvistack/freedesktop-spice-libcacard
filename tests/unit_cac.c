/*
 * Unit-test low-level CAC code
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
#include "cac-aca.c"

struct acr_applet a = {
    .id = 0x42, .num_objects = 1, .objects = {
            {.id = "\xab\xcd", .num_ins = 1, .ins = {
                {.code = 0x01, .acrid = 0x02, .config = ACR_INS_CONFIG_P2, .p1 = 0x03, .p2 = 0x04, .data1 = 0x05}
            }
        }
    }
};

static void test_acr_applet_object_encode(void)
{
    unsigned int buflen, objlen;
    unsigned char *buf = NULL, *p = NULL;

    /* Too short */
    buflen = 0;
    p = acr_applet_object_encode(a.objects, buf, buflen, &objlen);
    g_assert_null(p);

    buflen = 16;
    buf = g_malloc(buflen);

    /* Still too short */
    buflen = 2;
    p = acr_applet_object_encode(a.objects, buf, buflen, &objlen);
    g_assert_null(p);

    /* Still too short */
    buflen = 4;
    p = acr_applet_object_encode(a.objects, buf, buflen, &objlen);
    g_assert_null(p);

    /* This should do */
    buflen = 16;
    p = acr_applet_object_encode(a.objects, buf, buflen, &objlen);
    g_assert_cmpmem(buf, objlen, "\xab\xcd\x01\x02\x04\x02", 6);
    g_free(buf);
}

static void test_acr_applet_encode(void)
{
    unsigned int outlen;
    unsigned char *p = NULL;

    /* Wrong arguments */
    p = acr_applet_encode(&a, NULL);
    g_assert_null(p);

    p = acr_applet_encode(&a, &outlen);
    g_assert_cmpmem(p, outlen, "\x42\x01\x82\x06\xab\xcd\x01\x02\x04\x02", 10);
    g_free(p);
}

int main(int argc, char *argv[])
{
    int ret;

    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/unittests/cac/acr_applet_object_encode", test_acr_applet_object_encode);
    g_test_add_func("/unittests/cac/acr_applet_encode", test_acr_applet_encode);
    ret = g_test_run();

    return ret;
}

/* vim: set ts=4 sw=4 tw=0 noet expandtab: */
