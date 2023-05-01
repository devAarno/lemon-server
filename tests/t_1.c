/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022, 2023 Parkhomenko Stanislav
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

typedef struct {
    string expectedValue;
    char callCounter;
} calledCallback;

void setUp(void) {
}

void tearDown(void) {
}

static lemonError fakeExecute(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    ++(data->callCounter);
    return LE_OK;
}

static void test_byRawRequest1(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/1.jpg";
    uriCallback.expectedValue.length = 6;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
}

static void test_byRawRequest2(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;
    calledCallback hostHeaderCallback;
    calledCallback userAgentHeaderCallback;
    calledCallback acceptHeaderCallback;
    calledCallback acceptLanguageHeaderCallback;
    calledCallback acceptEncodingHeaderCallback;
    calledCallback connectionHeaderCallback;
    calledCallback upgradeInsecureRequestsHeaderCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest2, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "GET";
    methodCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/";
    uriCallback.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    hostHeaderCallback.callCounter = 0;
    hostHeaderCallback.expectedValue.data = "myserver:40000";
    hostHeaderCallback.expectedValue.length = 14;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Host", fakeExecute, &hostHeaderCallback));

    userAgentHeaderCallback.callCounter = 0;
    userAgentHeaderCallback.expectedValue.data = "Browser/5.0 (OS 10.0; os64; x64; rv:54.0) HTMLEngine/20100101 Browser/54.0";
    userAgentHeaderCallback.expectedValue.length = 74;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "User-Agent", fakeExecute, &userAgentHeaderCallback));

    acceptHeaderCallback.callCounter = 0;
    acceptHeaderCallback.expectedValue.data = "text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8";
    acceptHeaderCallback.expectedValue.length = 64;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Accept", fakeExecute, &acceptHeaderCallback));

    acceptLanguageHeaderCallback.callCounter = 0;
    acceptLanguageHeaderCallback.expectedValue.data = "ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3";
    acceptLanguageHeaderCallback.expectedValue.length = 35;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Accept-Language", fakeExecute, &acceptLanguageHeaderCallback));

    acceptEncodingHeaderCallback.callCounter = 0;
    acceptEncodingHeaderCallback.expectedValue.data = "gzip, deflate";
    acceptEncodingHeaderCallback.expectedValue.length = 13;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Accept-Encoding", fakeExecute, &acceptEncodingHeaderCallback));

    connectionHeaderCallback.callCounter = 0;
    connectionHeaderCallback.expectedValue.data = "keep-alive";
    connectionHeaderCallback.expectedValue.length = 10;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Connection", fakeExecute, &connectionHeaderCallback));

    upgradeInsecureRequestsHeaderCallback.callCounter = 0;
    upgradeInsecureRequestsHeaderCallback.expectedValue.data = "1";
    upgradeInsecureRequestsHeaderCallback.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Upgrade-Insecure-Requests", fakeExecute, &upgradeInsecureRequestsHeaderCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
    TEST_ASSERT_EQUAL(1, hostHeaderCallback.callCounter);
    TEST_ASSERT_EQUAL(1, userAgentHeaderCallback.callCounter);
    TEST_ASSERT_EQUAL(1, acceptHeaderCallback.callCounter);
    TEST_ASSERT_EQUAL(1, acceptLanguageHeaderCallback.callCounter);
    TEST_ASSERT_EQUAL(1, acceptEncodingHeaderCallback.callCounter);
    TEST_ASSERT_EQUAL(1, connectionHeaderCallback.callCounter);
    TEST_ASSERT_EQUAL(1, upgradeInsecureRequestsHeaderCallback.callCounter);
}

static void test_byRawRequest3(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;
    calledCallback iCallback;
    calledCallback jCallback;
    calledCallback tempCallback;
    calledCallback hostHeaderCallback;
    calledCallback complexHeaderCallback;
    calledCallback hoztHeaderCallback;

    /*
    POST /index.html?i=100&j=-7&temp=Kelvin HTTP/1.1
    Host:          localhost
    complex: test1;test2;test3    groovy  mmm
    Hozt:z=z&z?z:z127.0.0.1
    */


    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest3, sizeof (request.privateBuffer));

    methodCallback.callCounter = 0;
    methodCallback.expectedValue.data = "POST";
    methodCallback.expectedValue.length = 4;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpMethodRequest(&request, fakeExecute, &methodCallback));

    uriCallback.callCounter = 0;
    uriCallback.expectedValue.data = "/index.html";
    uriCallback.expectedValue.length = 11;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpUriRequest(&request, fakeExecute, &uriCallback));

    httpVersionCallback.callCounter = 0;
    httpVersionCallback.expectedValue.data = "HTTP/1.1";
    httpVersionCallback.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpVersionRequest(&request, fakeExecute, &httpVersionCallback));

    iCallback.callCounter = 0;
    iCallback.expectedValue.data = "100";
    iCallback.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "i", fakeExecute, &iCallback));

    jCallback.callCounter = 0;
    jCallback.expectedValue.data = "-7";
    jCallback.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "j", fakeExecute, &jCallback));

    tempCallback.callCounter = 0;
    tempCallback.expectedValue.data = "Kelvin";
    tempCallback.expectedValue.length = 6;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "temp", fakeExecute, &tempCallback));

    hostHeaderCallback.callCounter = 0;
    hostHeaderCallback.expectedValue.data = "localhost";
    hostHeaderCallback.expectedValue.length = 9;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Host", fakeExecute, &hostHeaderCallback));

    complexHeaderCallback.callCounter = 0;
    complexHeaderCallback.expectedValue.data = "test1;test2;test3    groovy  mmm";
    complexHeaderCallback.expectedValue.length = 32;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "complex", fakeExecute, &complexHeaderCallback));

    hoztHeaderCallback.callCounter = 0;
    hoztHeaderCallback.expectedValue.data = "z=z&z?z:z127.0.0.1";
    hoztHeaderCallback.expectedValue.length = 18;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Hozt", fakeExecute, &hoztHeaderCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
    TEST_ASSERT_EQUAL(1, iCallback.callCounter);
    TEST_ASSERT_EQUAL(1, jCallback.callCounter);
    TEST_ASSERT_EQUAL(1, tempCallback.callCounter);
    TEST_ASSERT_EQUAL(1, hostHeaderCallback.callCounter);
    TEST_ASSERT_EQUAL(1, complexHeaderCallback.callCounter);
    TEST_ASSERT_EQUAL(1, hoztHeaderCallback.callCounter);
}

int main(void) {
    UnityBegin(__FILE__);
    RUN_TEST(test_byRawRequest1);
    RUN_TEST(test_byRawRequest2);
    RUN_TEST(test_byRawRequest3);
    return (UnityEnd());
}

