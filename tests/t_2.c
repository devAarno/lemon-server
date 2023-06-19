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

#define TESTNAME EmptyOrNotFound

static const char* rawRequest1 = "GET /1.jpg?hello=world&value1=&value2=&test=test&yes= HTTP/1.1\r\nClient:           \r\nServer:\r\nThis:   is a value  \r\n\r\n";

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

static void test_empty(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;
    calledCallback helloCallback;
    calledCallback value1Callback;
    calledCallback value2Callback;
    calledCallback testCallback;
    calledCallback yesCallback;
    calledCallback clientCallback;
    calledCallback serverCallback;
    calledCallback thisCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

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

    helloCallback.callCounter = 0;
    helloCallback.expectedValue.data = "world";
    helloCallback.expectedValue.length = 5;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "hello", fakeExecute, &helloCallback));

    value1Callback.callCounter = 0;
    value1Callback.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "value1", fakeExecute, &value1Callback));

    value2Callback.callCounter = 0;
    value2Callback.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "value2", fakeExecute, &value2Callback));

    testCallback.callCounter = 0;
    testCallback.expectedValue.data = "test";
    testCallback.expectedValue.length = 4;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "test", fakeExecute, &testCallback));

    yesCallback.callCounter = 0;
    yesCallback.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "yes", fakeExecute, &yesCallback));

    clientCallback.callCounter = 0;
    clientCallback.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Client", fakeExecute, &clientCallback));

    serverCallback.callCounter = 0;
    serverCallback.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Server", fakeExecute, &serverCallback));

    thisCallback.callCounter = 0;
    thisCallback.expectedValue.data = "is a value";
    thisCallback.expectedValue.length = 10;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "This", fakeExecute, &thisCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
    TEST_ASSERT_EQUAL(1, helloCallback.callCounter);

    TEST_ASSERT_EQUAL(1, value1Callback.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value1Callback.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value1Callback.expectedValue)));

    TEST_ASSERT_EQUAL(1, value2Callback.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value2Callback.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value2Callback.expectedValue)));

    TEST_ASSERT_EQUAL(1, testCallback.callCounter);

    TEST_ASSERT_EQUAL(1, yesCallback.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, yesCallback.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(yesCallback.expectedValue)));

    TEST_ASSERT_EQUAL(1, clientCallback.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, clientCallback.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(clientCallback.expectedValue)));

    TEST_ASSERT_EQUAL(1, serverCallback.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, serverCallback.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(serverCallback.expectedValue)));

    TEST_ASSERT_EQUAL(1, thisCallback.callCounter);
}


static void test_absent(void) {
    httpRequest request;
    calledCallback absentGetParamCallback;
    calledCallback absentHeaderParamCallback;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));

    absentGetParamCallback.callCounter = 0;
    absentGetParamCallback.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "any", fakeExecute, &absentGetParamCallback));

    absentHeaderParamCallback.callCounter = 0;
    absentHeaderParamCallback.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "any", fakeExecute, &absentHeaderParamCallback));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(0, absentGetParamCallback.callCounter);
    TEST_ASSERT_EQUAL(0, absentHeaderParamCallback.callCounter);
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test_empty);
    RUN_TEST(test_absent);
    return (UnityEnd());
}
