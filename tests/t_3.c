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

#define TESTNAME Strncasecmp

static const char* rawRequest1 = "GET /1.jpg?heLLo=world&vAlue1=&value2=&TesT=test&Yes= HTTP/1.1\r\nClient:           \r\nServer:\r\nThis:   is a value  \r\n\r\n";

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

static void test_strncasecmp(void) {
    httpRequest request;
    calledCallback methodCallback;
    calledCallback uriCallback;
    calledCallback httpVersionCallback;
    calledCallback helloCallback1;
    calledCallback helloCallback2;
    calledCallback value1Callback1;
    calledCallback value1Callback2;
    calledCallback value1Callback3;
    calledCallback value2Callback1;
    calledCallback value2Callback2;
    calledCallback value2Callback3;
    calledCallback testCallback1;
    calledCallback testCallback2;
    calledCallback testCallback3;
    calledCallback yesCallback1;
    calledCallback yesCallback2;
    calledCallback yesCallback3;
    calledCallback clientCallback1;
    calledCallback clientCallback2;
    calledCallback clientCallback3;
    calledCallback serverCallback1;
    calledCallback serverCallback2;
    calledCallback serverCallback3;
    calledCallback thisCallback1;
    calledCallback thisCallback2;

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

    helloCallback1.callCounter = 0;
    helloCallback1.expectedValue.data = "world";
    helloCallback1.expectedValue.length = 5;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "hello", fakeExecute, &helloCallback1));

    helloCallback2.callCounter = 0;
    helloCallback2.expectedValue.data = "world";
    helloCallback2.expectedValue.length = 5;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "HELLO", fakeExecute, &helloCallback2));

    value1Callback1.callCounter = 0;
    value1Callback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "value1", fakeExecute, &value1Callback1));

    value1Callback2.callCounter = 0;
    value1Callback2.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "vALuE1", fakeExecute, &value1Callback2));

    value1Callback3.callCounter = 0;
    value1Callback3.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "VALUE1", fakeExecute, &value1Callback3));

    value2Callback1.callCounter = 0;
    value2Callback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "value2", fakeExecute, &value2Callback1));

    value2Callback2.callCounter = 0;
    value2Callback2.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "VALue2", fakeExecute, &value2Callback2));

    value2Callback3.callCounter = 0;
    value2Callback3.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "valUE2", fakeExecute, &value2Callback3));

    testCallback1.callCounter = 0;
    testCallback1.expectedValue.data = "test";
    testCallback1.expectedValue.length = 4;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "test", fakeExecute, &testCallback1));

    testCallback2.callCounter = 0;
    testCallback2.expectedValue.data = "test";
    testCallback2.expectedValue.length = 4;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "tESt", fakeExecute, &testCallback2));

    testCallback3.callCounter = 0;
    testCallback3.expectedValue.data = "test";
    testCallback3.expectedValue.length = 4;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "TesT", fakeExecute, &testCallback3));

    yesCallback1.callCounter = 0;
    yesCallback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "yes", fakeExecute, &yesCallback1));

    yesCallback2.callCounter = 0;
    yesCallback2.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "yeS", fakeExecute, &yesCallback2));

    yesCallback3.callCounter = 0;
    yesCallback3.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpGetParameterQueryRequest(&request, "yEs", fakeExecute, &yesCallback3));

    clientCallback1.callCounter = 0;
    clientCallback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Client", fakeExecute, &clientCallback1));

    clientCallback2.callCounter = 0;
    clientCallback2.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "ClieNT", fakeExecute, &clientCallback2));

    clientCallback3.callCounter = 0;
    clientCallback3.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "ClIEnT", fakeExecute, &clientCallback3));

    serverCallback1.callCounter = 0;
    serverCallback1.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "Server", fakeExecute, &serverCallback1));

    serverCallback2.callCounter = 0;
    serverCallback2.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "SerVER", fakeExecute, &serverCallback2));

    serverCallback3.callCounter = 0;
    serverCallback3.expectedValue = getEmptyString();
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "SERVER", fakeExecute, &serverCallback3));

    thisCallback1.callCounter = 0;
    thisCallback1.expectedValue.data = "is a value";
    thisCallback1.expectedValue.length = 10;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "this", fakeExecute, &thisCallback1));

    thisCallback2.callCounter = 0;
    thisCallback2.expectedValue.data = "is a value";
    thisCallback2.expectedValue.length = 10;
    TEST_ASSERT_EQUAL(LE_OK, appendHttpHeaderQueryRequest(&request, "This", fakeExecute, &thisCallback2));

    TEST_ASSERT_EQUAL(LE_OK, parseHTTP(&request));

    TEST_ASSERT_EQUAL(1, methodCallback.callCounter);
    TEST_ASSERT_EQUAL(1, uriCallback.callCounter);
    TEST_ASSERT_EQUAL(1, httpVersionCallback.callCounter);
    TEST_ASSERT_EQUAL(1, helloCallback1.callCounter);
    TEST_ASSERT_EQUAL(1, helloCallback2.callCounter);

    TEST_ASSERT_EQUAL(1, value1Callback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value1Callback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value1Callback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, value1Callback2.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value1Callback2.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value1Callback2.expectedValue)));

    TEST_ASSERT_EQUAL(1, value1Callback3.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value1Callback3.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value1Callback3.expectedValue)));

    TEST_ASSERT_EQUAL(1, value2Callback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value2Callback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value2Callback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, value2Callback2.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value2Callback2.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value2Callback2.expectedValue)));

    TEST_ASSERT_EQUAL(1, value2Callback3.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, value2Callback3.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(value2Callback3.expectedValue)));

    TEST_ASSERT_EQUAL(1, testCallback1.callCounter);
    TEST_ASSERT_EQUAL(1, testCallback2.callCounter);
    TEST_ASSERT_EQUAL(1, testCallback3.callCounter);

    TEST_ASSERT_EQUAL(1, yesCallback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, yesCallback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(yesCallback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, yesCallback2.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, yesCallback2.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(yesCallback2.expectedValue)));

    TEST_ASSERT_EQUAL(1, yesCallback3.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, yesCallback3.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(yesCallback3.expectedValue)));

    TEST_ASSERT_EQUAL(1, clientCallback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, clientCallback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(clientCallback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, clientCallback2.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, clientCallback2.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(clientCallback2.expectedValue)));

    TEST_ASSERT_EQUAL(1, clientCallback3.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, clientCallback3.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(clientCallback3.expectedValue)));

    TEST_ASSERT_EQUAL(1, serverCallback1.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, serverCallback1.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(serverCallback1.expectedValue)));

    TEST_ASSERT_EQUAL(1, serverCallback2.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, serverCallback2.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(serverCallback2.expectedValue)));

    TEST_ASSERT_EQUAL(1, serverCallback3.callCounter);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, serverCallback3.expectedValue.data);
    TEST_ASSERT_EQUAL(TRUE, isStringEmpty(&(serverCallback3.expectedValue)));

    TEST_ASSERT_EQUAL(1, thisCallback1.callCounter);
    TEST_ASSERT_EQUAL(1, thisCallback2.callCounter);

}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test_strncasecmp);
    return (UnityEnd());
}
