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

static const lemonError fakeExecuteForTest9AndTest10Buffer5And6(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test9(void) {
    const char* rawRequest = "{\"a\":{\"a\":1}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest9AndTest10Buffer5And6, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(2, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest10Buffer1And7And8(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest10Buffer9(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test10(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":1}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..b..a";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..a..a..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$.a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$..a.a";
    jsonPathQueryBuffer jsonPathQueryBuffer7[] = "$..a..a.";
    jsonPathQueryBuffer jsonPathQueryBuffer8[] = "$..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer9[] = "$..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    calledCallback callData9;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest10Buffer1And7And8, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "1";
    callData2.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "*** Not found (it's a fake message) ***";
    callData3.expectedValue.length = strlen(callData3.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "*** Not found (it's a fake message) ***";
    callData4.expectedValue.length = strlen(callData4.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = NULL;
    callData5.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecuteForTest9AndTest10Buffer5And6, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest9AndTest10Buffer5And6, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = NULL;
    callData7.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecuteForTest10Buffer1And7And8, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecuteForTest10Buffer1And7And8, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = NULL;
    callData9.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecuteForTest10Buffer9, &callData9));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(3, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(0, callData3.callCounter);
    TEST_ASSERT_EQUAL(0, callData4.callCounter);
    TEST_ASSERT_EQUAL(2, callData5.callCounter);
    TEST_ASSERT_EQUAL(2, callData6.callCounter);
    TEST_ASSERT_EQUAL(3, callData7.callCounter);
    TEST_ASSERT_EQUAL(3, callData8.callCounter);
    TEST_ASSERT_EQUAL(4, callData9.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest11Buffer1(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            /* TEST_ASSERT_EQUAL(5, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);*/
            TEST_ASSERT_EQUAL(TRUE, isFalseString(*value));
            ++(data->callCounter);
            break;
        case 6:
            TEST_ASSERT_EQUAL(11, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":false}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test11(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":false}}}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..a..a..a..a..a";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest11Buffer1, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(7, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest12Buffer1(const string *value, calledCallback *data) {
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

static const lemonError fakeExecuteForTest12Buffer2(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
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

static void test12(void) {
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
                      appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest12Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK,
                      appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest12Buffer2, &callData2));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(5, callData1.callCounter);
    TEST_ASSERT_EQUAL(3, callData2.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}
/*

{
    "a" : {
        "b1" : {
            "c11" : [
                "d111",
                {"e112" : ["f1121"]},
                "d113"
                ],
            "c12" : [
                "d121",
                "d122",
                "d123"
                ]
        },
        "b2" : {
            "c21" : [
                {"e211" : ["f2111", "f2112", ["f21121", {"g21122":null},"f21123"]]},
                "d212",
                "d213"
                ],
            "c22" : [
                "d221",
                "e222",
                "d223"
                ]
        }
    }
}

 $.a..











 [1,2,[5,6,{"7": [8,9,10]},8],4]
 $[2][2]..
 */

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
    RUN_TEST(test10);
    return (UnityEnd());
}
