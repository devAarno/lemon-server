/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021 Parkhomenko Stanislav
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
#include "../src/lemonHttp/lemonError.h"

#include "fakeDescriptor.h"

#define TESTNAME SimpleParserTest

static const char* rawRequest1 = "GET /1.jpg HTTP/1.1\r\n\r\n";
static const char* rawRequest2 = "GET / HTTP/1.1\r\nHost: myserver:40000\r\nUser-Agent: Browser/5.0 (OS 10.0; os64; x64; rv:54.0) HTMLEngine/20100101 Browser/54.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8\r\nAccept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\n\r\n";
static const char* rawRequest3 = "POST /index.html?i=100&j=-7&temp=Kelvin HTTP/1.1\r\nHost:          localhost\r\ncomplex: test1;test2;test3    groovy  mmm   \r\nHozt:z=z&z?z:z127.0.0.1\r\n\r\n";

void setUp(void) {
}

void tearDown(void) {
}

static void test_byRawRequest1(void) {
    httpRequest request;
    string* out;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/1.jpg", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);
}

static void test_byRawRequest2(void) {
    httpRequest request;
    string* out;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest2, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("GET", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Host");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("myserver:40000", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "User-Agent");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("Browser/5.0 (OS 10.0; os64; x64; rv:54.0) HTMLEngine/20100101 Browser/54.0", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Accept");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Accept-Language");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Accept-Encoding");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("gzip, deflate", out->data, out->length);
    /* Already checked */

    out = (string *) getHeaderOfHttpRequest(&request, "Connection");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("keep-alive", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Upgrade-Insecure-Requests");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("1", out->data, out->length);
}

static void test_byRawRequest3(void) {
    httpRequest request;
    string* out;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest3, sizeof (request.privateBuffer));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    out = (string *) getMethodOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("POST", out->data, out->length);

    out = (string *) getUriOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("/index.html", out->data, out->length);

    out = (string *) getVersionOfHttpRequest(&request);
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("HTTP/1.1", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "i");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("100", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "j");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("-7", out->data, out->length);

    out = (string *) getQueryParameterOfHttpRequest(&request, "temp");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("Kelvin", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Host");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("localhost", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "complex");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("test1;test2;test3    groovy  mmm", out->data, out->length);

    out = (string *) getHeaderOfHttpRequest(&request, "Hozt");
    TEST_ASSERT_NOT_NULL(out);
    TEST_ASSERT_EQUAL_STRING_LEN("z=z&z?z:z127.0.0.1", out->data, out->length);
}

int main(void) {
    UnityBegin(__FILE__);
    RUN_TEST(test_byRawRequest1);
    RUN_TEST(test_byRawRequest2);
    RUN_TEST(test_byRawRequest3);
    return (UnityEnd());
}

