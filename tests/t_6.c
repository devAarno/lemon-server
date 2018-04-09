/*
 * Copyright (C) 2017, 2018 Parkhomenko Stanislav
 *
 * This file is part of Lemon Server.
 *
 * Lemon Server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../3rdParty/unity/git/src/unity.h"
#include "../src/lemonHttp/parser.h"
#include "../src/lemonHttp/string.h"
#include "../src/net/socket.h"
#include "../src/lemonHttp/lemonError.h"

#include "fakeDescriptor.h"

#define TESTNAME ReadBody

static const char* rawRequest1 = "POST /1%201.%6apg?heLLo=world&vAlue1=&value2=++%20+&TesT=te+st&Yes= HTTP/1.1\r\nClient:           \r\nServer:\r\nThis:   is+a value  \r\n\r\n";

void setUp(void) {
    FILE *f;
    size_t charBitShifts;
    int rnd = RAND_MAX;
    unsigned short int i;

    charBitShifts = 0;
    while (0 != rnd) {
        rnd >>= 8;
        ++charBitShifts;
    }
    charBitShifts = (charBitShifts * 8) / 6; /* Also it is an amount of chars which we can get from one rand() */

    f = fopen("request.txt", "wb");
    
    if (NULL == f) {
        return ;
    }

    fwrite(rawRequest1, sizeof (char), strlen(rawRequest1), f);

    {
        /* Should be char buf[charBitShifts]; but it is not C89 */
        char buf[sizeof(rnd) * 2];
        unsigned char j;
        for (i = 0; i < 32000; ++i) {
            rnd = rand();
            j = 0;
            while (j < charBitShifts) {
                buf[j] = rnd % 64 + 32;
                rnd /= 64;
                ++j;
            }
            fwrite(&buf, sizeof (char), sizeof (char) * charBitShifts, f);
        }
    }


    fputc('\r', f);
    fputc('\n', f);
    fputc('\0', f);
    fclose(f);


}

void tearDown(void) {
    remove("request.txt");
}

static void test_body(void) {
    httpRequest request;
    string* out;
    const string* body;
    FILE *fparser, *fraw;

    fparser = fopen("request.txt", "rb");
    
    if (NULL == fparser) {
        TEST_ASSERT_NOT_NULL(fparser);
        return ;
    }
    
    /* What should I do if test fails? How to close files? */
    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, fileno(fparser)));
    readData(&request);
    TEST_ASSERT_EQUAL(LE_OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("POST", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/1 1.jpg", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);



    out = (string *) getQueryParameterOfHttpRequest(&request, "hello");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("world", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "value1");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "value2");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("    ", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "test");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("te st", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "yes");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);


    out = (string *) getHeaderOfHttpRequest(&request, "Client");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Server");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "This");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("is+a value", out->data, out->length);

    body = getBodyBufferOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    fraw = fopen("request.txt", "rb");
    
    if (NULL == fraw) {
        TEST_ASSERT_NOT_NULL(fraw);
        fclose(fparser);
        return ;
    }
    
    if (0 != fseek(fraw, strlen(rawRequest1), SEEK_SET)) {
        fclose(fraw);
        fclose(fparser);
        TEST_ASSERT_NOT_NULL(NULL);
        return ;
    } else {
        char buf[PRIVATE_BUFFER_SIZE];
        do {
            fread(&buf, sizeof (char), sizeof (char) * (body->length), fraw);
            TEST_ASSERT_EQUAL_MEMORY(&buf, body->data, body->length);
        } while (0 < readData(&request));
    }

    fclose(fraw);
    fclose(fparser);
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test_body);
    return (UnityEnd());
}
