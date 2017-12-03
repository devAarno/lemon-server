/*
 * Copyright (C) 2017 Parkhomenko Stanislav
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

#include <string.h>

#include "../3rdParty/unity/git/src/unity.h"
#include "../src/lemonHttp/parser.h"
#include "../src/lemonHttp/string.h"

#include "fakeDescriptor.h"

#define TESTNAME Strncasecmp

static const char* rawRequest1 = "GET /1.jpg?heLLo=world&vAlue1=&value2=&TesT=test&Yes= HTTP/1.1\r\nClient:           \r\nServer:\r\nThis:   is a value  \r\n\r\n";

void setUp(void) {
}

void tearDown(void) {
}

static void test_strncasecmp(void) {
    httpRequest request;
    string* out;

    initHttpRequest(&request, FAKE_DESCRIPTOR);

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(OK, parse(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/1.jpg", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);



    out = (string *) getQueryParameterOfHttpRequest(&request, "hello");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("world", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "HELLO");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("world", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "value1");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "vALuE1");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "VALUE1");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "value2");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "VALue2");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "valUE2");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "test");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("test", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "tESt");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("test", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "TesT");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("test", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "yes");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "yeS");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "yEs");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);


    out = (string *) getHeaderOfHttpRequest(&request, "Client");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "ClieNT");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "ClIEnT");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Server");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "SerVER");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "SERVER");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), TRUE);
    TEST_ASSERT_EQUAL(0, out->length);
    TEST_ASSERT_EQUAL_STRING_LEN("", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "This");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("is a value", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "this");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL(isStringEmpty(out), FALSE);
    TEST_ASSERT_EQUAL_STRING_LEN("is a value", out->data, out->length);
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test_strncasecmp);
    return (UnityEnd());
}
