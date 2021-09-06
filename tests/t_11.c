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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../3rdParty/unity/git/src/unity.h"
#include "../src/lemonHttp/bodyEngines/json/jsonParser.h"
#include "../src/lemonHttp/string.h"
#include "../src/lemonHttp/parser.h"

#include "fakeDescriptor.h"

#define TESTNAME TestJSONPathRecursionByCallbacks

typedef struct {
    string expectedValue;
    char callCounter;
} calledCallback;

void setUp(void) {
}

void tearDown(void) {
}

static const lemonError fakeExecute(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    ++(data->callCounter);
    return LE_OK;
}

static void test1(void) {
    const char* rawRequest = "{\"a\" : 123}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "123";
    callData1.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest2(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(11, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\" : 123}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(19, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\" : {\"a\" : 123}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test2(void) {
    const char* rawRequest = "{\"b\" : {\"a\" : 123}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..";
    calledCallback callData1;
    calledCallback callData2;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "123";
    callData1.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest2, &callData2));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(2, callData2.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static void test3(void) {
    const char* rawRequest = "{\"c\" : {\"b\" : {\"a\" : 123}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "123";
    callData1.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static void test4(void) {
    const char* rawRequest = "{\"b\" : {\"a\" : {\"c\" : 123}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "{\"c\" : 123}";
    callData1.expectedValue.length = 11;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest5(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(3, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("123", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(22, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"c\"  : {\"a\" : 123}  }", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test5(void) {
    const char* rawRequest = "{\"b\" : {\"a\" : {\"c\"  : {\"a\" : 123}  }}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest5, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(2, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static void test6(void) {
    const char* rawRequest = "{\"b\" : {\"a\" : {\"x\" : {\"c\" : {\"a\" : {\"x\" : 123}}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a.x..a.x";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "123";
    callData1.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest7(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(3, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("123", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(27, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"c\" : {\"a\" : {\"x\" : 123}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test7(void) {
    const char* rawRequest = "{\"b\" : {\"a\" : {\"x\" : {\"c\" : {\"a\" : {\"x\" : 123}}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a.x";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest7, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(2, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static void test8(void) {
    const char* rawRequest = "{\"b\" : {\"a\" : {\"x\" : {\"c\" : {\"a\" : {\"c\" : {\"x\" : 123}}}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a.x";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "{\"c\" : {\"a\" : {\"c\" : {\"x\" : 123}}}}";
    callData1.expectedValue.length = 35;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest9Buffer1(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("q", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("3", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(32, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[false, true, { \"a\" : 3}, false]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(41, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{ \"a\" : [false, true, { \"a\" : 3}, false]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest9Buffer2(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("3", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("3", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(32, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[false, true, { \"a\" : 3}, false]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test9(void) {
    /*
{
    "b" : "a",
    "a" : "q",
    "c" : {
        "d" : {
            "a" : 1
        },
        "e" : {
            "f" : 2
        },
        "g" : {
            "a" : {
                "a" : [false, true, { "a" : 3}, false]
            }
        }
    }
}
     */
    const char* rawRequest = "{\n"
                             "    \"b\" : \"a\",\n"
                             "    \"a\" : \"q\",\n"
                             "    \"c\" : {\n"
                             "        \"d\" : {\n"
                             "            \"a\" : 1\n"
                             "        },\n"
                             "        \"e\" : {\n"
                             "            \"f\" : 2\n"
                             "        },\n"
                             "        \"g\" : {\n"
                             "            \"a\" : { \"a\" : [false, true, { \"a\" : 3}, false]}\n"
                             "        }\n"
                             "    }\n"
                             "}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a";
    calledCallback callData1;
    calledCallback callData2;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK,
                      appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest9Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK,
                      appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest9Buffer2, &callData2));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(5, callData1.callCounter);
    TEST_ASSERT_EQUAL(3, callData2.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test1);
    RUN_TEST(test2);
    RUN_TEST(test3);
    RUN_TEST(test4);
    RUN_TEST(test5);
    RUN_TEST(test6);
    RUN_TEST(test7);
    RUN_TEST(test8);
    RUN_TEST(test9);
    return (UnityEnd());
}
