/*
 * Shared test functions for libCACard
 *
 * Copyright 2018 Red Hat, Inc.
 *
 * Author: Jakub Jelen <jjelen@redhat.com>
 *
 * This code is licensed under the GNU LGPL, version 2.1 or later.
 * See the COPYING file in the top-level directory.
 */
#ifndef _TESTS_COMMON
#define _TESTS_COMMON

#include "libcacard.h"

#define APDUBufSize 270

enum {
    TEST_PKI = 1,
    TEST_PKI_2,
    TEST_CCC,
    TEST_ACA,
    TEST_GENERIC,
    TEST_EMPTY_BUFFER,
    TEST_EMPTY,
    TEST_PASSTHROUGH,
};

void select_coid_good(VReader *reader, unsigned char *coid);
void select_coid_bad(VReader *reader, unsigned char *coid);

int select_aid_response(VReader *reader, unsigned char *aid,
                        unsigned int aid_len, int response);
void select_aid(VReader *reader, unsigned char *aid, unsigned int aid_len);
void select_applet(VReader *reader, int type);

void get_properties_coid(VReader *reader, const unsigned char coid[2], int object_type);
void get_properties(VReader *reader, int object_type);

void read_buffer(VReader *reader, uint8_t type, int object_type);

void do_sign(VReader *reader, int parts);

void do_decipher(VReader *reader, int type);

void test_empty_applets(void);

void test_get_response(void);

void check_login_count(void);

void test_msft_applet(void);
void test_gp_applet(void);

int isHWTests(void);
void setHWTests(int);

int getBits(void);

#endif /* _TESTS_COMMON */
