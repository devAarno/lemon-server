/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022 Parkhomenko Stanislav
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

static const lemonError fakeExecuteForTest2AndTest3(const string *value, calledCallback *data) {
    printf("FAIL OOOUUUTTT %.*s\r\n", value->length, value->data);
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
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest2AndTest3, &callData2));

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
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..b";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..c";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..b..";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$..c..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "123";
    callData1.expectedValue.length = 3;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "{\"a\" : 123}";
    callData2.expectedValue.length = 11;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "{\"b\" : {\"a\" : 123}}";
    callData3.expectedValue.length = 19;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue = callData1.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue = callData2.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest2AndTest3, &callData6));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    TEST_ASSERT_EQUAL(2, callData6.callCounter);
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
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
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
    /*printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
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
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    calledCallback callData9;
    calledCallback callData10;
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

    callData10.callCounter = 0;
    callData10.expectedValue.data = "1";
    callData10.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer10, fakeExecute, &callData10));
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
    TEST_ASSERT_EQUAL(1, callData10.callCounter);
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
            {
                const string false = getFalseString();
                TEST_ASSERT_EQUAL(false.length, value->length);
                TEST_ASSERT_EQUAL_STRING_LEN(false.data, value->data, value->length);
                TEST_ASSERT_EQUAL(TRUE, isFalseString(*value));
            }
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

static const lemonError fakeExecuteForTest11Buffer234567(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            {
                const string false = getFalseString();
                TEST_ASSERT_EQUAL(false.length, value->length);
                TEST_ASSERT_EQUAL_STRING_LEN(false.data, value->data, value->length);
                TEST_ASSERT_EQUAL(TRUE, isFalseString(*value));
            }
            ++(data->callCounter);
            break;
        case 5:
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

static const lemonError fakeExecuteForTest11Buffer89(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
        {
            const string false = getFalseString();
            TEST_ASSERT_EQUAL(false.length, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN(false.data, value->data, value->length);
            TEST_ASSERT_EQUAL(TRUE, isFalseString(*value));
        }
            ++(data->callCounter);
            break;
        case 4:
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
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..a..a..a.a";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..a..a..a..a.a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..a..a..a.a..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..a..a.a..a..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$..a.a..a..a..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer7[] = "$.a..a..a..a..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer8[] = "$..a.a..a..a.a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer9[] = "$..a.a..a..a.a..a..";
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
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest11Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest11Buffer234567, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecuteForTest11Buffer234567, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = NULL;
    callData4.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecuteForTest11Buffer234567, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = NULL;
    callData5.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecuteForTest11Buffer234567, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest11Buffer234567, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = NULL;
    callData7.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecuteForTest11Buffer234567, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecuteForTest11Buffer89, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = NULL;
    callData9.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecuteForTest11Buffer89, &callData9));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(7, callData1.callCounter);
    TEST_ASSERT_EQUAL(6, callData2.callCounter);
    TEST_ASSERT_EQUAL(6, callData3.callCounter);
    TEST_ASSERT_EQUAL(6, callData4.callCounter);
    TEST_ASSERT_EQUAL(6, callData5.callCounter);
    TEST_ASSERT_EQUAL(6, callData6.callCounter);
    TEST_ASSERT_EQUAL(6, callData7.callCounter);
    TEST_ASSERT_EQUAL(5, callData8.callCounter);
    TEST_ASSERT_EQUAL(5, callData9.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest12Buffer1(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            {
                const string false = getNullString();
                TEST_ASSERT_EQUAL(false.length, value->length);
                TEST_ASSERT_EQUAL_STRING_LEN(false.data, value->data, value->length);
                TEST_ASSERT_EQUAL(TRUE, isNullString(*value));
            }
            break;
        case 10:
        case 11:
        case 12:
        case 13:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":null}", value->data, value->length);
            break;
        case 14:
            TEST_ASSERT_EQUAL(16, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":null}}", value->data, value->length);
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
        case 2:
        case 3:
        case 4:
            {
                const string false = getNullString();
                TEST_ASSERT_EQUAL(false.length, value->length);
                TEST_ASSERT_EQUAL_STRING_LEN(false.data, value->data, value->length);
                TEST_ASSERT_EQUAL(TRUE, isNullString(*value));
            }
            ++(data->callCounter);
        case 5:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":null}", value->data, value->length);
            ++(data->callCounter);
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test12(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":1}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest12Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest12Buffer2, &callData2));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(15, callData1.callCounter);
    TEST_ASSERT_EQUAL(6, callData2.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest13Buffer1(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
            {
                const string false = getNullString();
                TEST_ASSERT_EQUAL(false.length, value->length);
                TEST_ASSERT_EQUAL_STRING_LEN(false.data, value->data, value->length);
                TEST_ASSERT_EQUAL(TRUE, isNullString(*value));
            }
            ++(data->callCounter);
            break;
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":null}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            TEST_ASSERT_EQUAL(16, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":null}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 31:
        case 32:
        case 33:
            TEST_ASSERT_EQUAL(22, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":null}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 34:
            TEST_ASSERT_EQUAL(28, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":{\"a\":null}}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest13Buffer2(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
            {
                const string nullStr = getNullString();
                TEST_ASSERT_EQUAL(nullStr.length, value->length);
                TEST_ASSERT_EQUAL_STRING_LEN(nullStr.data, value->data, value->length);
                TEST_ASSERT_EQUAL(TRUE, isNullString(*value));
            }
            ++(data->callCounter);
            break;
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":null}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 55:
        case 56:
        case 57:
        case 58:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
            TEST_ASSERT_EQUAL(16, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":null}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 65:
        case 66:
        case 67:
        case 68:
            TEST_ASSERT_EQUAL(22, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":null}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 69:
            TEST_ASSERT_EQUAL(28, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":{\"a\":null}}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test13(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":null}}}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest13Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest13Buffer2, &callData2));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(35, callData1.callCounter);
    TEST_ASSERT_EQUAL(70, callData2.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest14Buffer1(const string *value, calledCallback *data) {
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

static const lemonError fakeExecuteForTest14Buffer2(const string *value, calledCallback *data) {
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

static void test14(void) {
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
                      appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest14Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK,
                      appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest14Buffer2, &callData2));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(5, callData1.callCounter);
    TEST_ASSERT_EQUAL(3, callData2.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}



static const lemonError fakeExecuteForTest15Buffer1(const string *value, calledCallback *data) {
    /*printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("56", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
        case 2:
        case 3:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":56}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
        case 5:
            TEST_ASSERT_EQUAL(14, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":56}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
            TEST_ASSERT_EQUAL(20, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":56}}}", value->data, value->length);
            ++(data->callCounter);
            break;

        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test15(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":{\"a\":56}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest15Buffer1, &callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(7, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest16Buffer1(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(9, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"d\":990}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
        case 4:
        case 5:
            TEST_ASSERT_EQUAL(15, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"c\":{\"d\":990}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
        case 7:
        case 8:
            TEST_ASSERT_EQUAL(21, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":{\"c\":{\"d\":990}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 9:
            TEST_ASSERT_EQUAL(27, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"b\":{\"c\":{\"d\":990}}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest16Buffer3And4(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(9, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"d\":990}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(15, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"c\":{\"d\":990}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(21, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":{\"c\":{\"d\":990}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test16(void) {
    const char* rawRequest = "{\"x\":{\"a\":{\"a\":{\"a\":{\"b\":{\"c\":{\"d\":990}}}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..a";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..a..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..x..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest16Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "{\"b\":{\"c\":{\"d\":990}}}";
    callData2.expectedValue.length = 21;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecuteForTest16Buffer3And4,
                                                   &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = NULL;
    callData4.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecuteForTest16Buffer3And4,
                                                   &callData4));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(10, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(3, callData3.callCounter);
    TEST_ASSERT_EQUAL(3, callData4.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest17Buffer1(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
            {
                const string nullStr = getNullString();
                TEST_ASSERT_EQUAL(nullStr.length, value->length);
                TEST_ASSERT_EQUAL_STRING_LEN(nullStr.data, value->data, value->length);
                TEST_ASSERT_EQUAL(TRUE, isNullString(*value));
            }
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"e\":null}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test17(void) {
    const char* rawRequest = "{\"a\":{\"b\":{\"c\":{\"d\":{\"e\":null}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$.a.b.c.d..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..b..c..d.."; /* Same as prev */
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest17Buffer1, &callData1));

    /* NNNNNNNNNNNOOOOOOOOOOOOOOOOOOOTTTTTTTTTTTTTTTTT FFFFFFFFIIIIIIIIINNNNNNNNNNNNNNNIIIIIIIIIIIIIIIISSSSSSSSSSHHHHHHHHHHHHHEEEEEEDDDDDDDDDDD */
    /* NNNNNNNNNNNOOOOOOOOOOOOOOOOOOOTTTTTTTTTTTTTTTTT FFFFFFFFIIIIIIIIINNNNNNNNNNNNNNNIIIIIIIIIIIIIIIISSSSSSSSSSHHHHHHHHHHHHHEEEEEEDDDDDDDDDDD */
    /* NNNNNNNNNNNOOOOOOOOOOOOOOOOOOOTTTTTTTTTTTTTTTTT FFFFFFFFIIIIIIIIINNNNNNNNNNNNNNNIIIIIIIIIIIIIIIISSSSSSSSSSHHHHHHHHHHHHHEEEEEEDDDDDDDDDDD */
    /* NNNNNNNNNNNOOOOOOOOOOOOOOOOOOOTTTTTTTTTTTTTTTTT FFFFFFFFIIIIIIIIINNNNNNNNNNNNNNNIIIIIIIIIIIIIIIISSSSSSSSSSHHHHHHHHHHHHHEEEEEEDDDDDDDDDDD */
    /* NNNNNNNNNNNOOOOOOOOOOOOOOOOOOOTTTTTTTTTTTTTTTTT FFFFFFFFIIIIIIIIINNNNNNNNNNNNNNNIIIIIIIIIIIIIIIISSSSSSSSSSHHHHHHHHHHHHHEEEEEEDDDDDDDDDDD */

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(2, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest18Buffer1(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"q\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"q\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest18Buffer3(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test18(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"q\":1}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..a..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..a..a..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..q..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest18Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "{\"q\":1}";
    callData2.expectedValue.length = 7;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecuteForTest18Buffer3, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "1";
    callData4.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "1";
    callData5.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(3, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(2, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest19Buffer1(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
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
        case 2:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"q\":{\"a\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test19(void) {
    const char* rawRequest = "{\"a\":{\"q\":{\"a\":1}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..a..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..a..q..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$..q..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest19Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "1";
    callData2.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "{\"a\":1}";
    callData3.expectedValue.length = 7;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "1";
    callData4.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "{\"a\":1}";
    callData5.expectedValue.length = 7;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = "1";
    callData6.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecute, &callData6));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(3, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    TEST_ASSERT_EQUAL(1, callData6.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest20Buffer1(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
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

static const lemonError fakeExecuteForTest20Buffer3(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
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

static const lemonError fakeExecuteForTest20Buffer4(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
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


static void test20(void) {
    const char* rawRequest = "{\"q\":{\"a\":{\"a\":1}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..q..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..q..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest20Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "1";
    callData2.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecuteForTest20Buffer3, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = NULL;
    callData4.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecuteForTest20Buffer4, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "1";
    callData5.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(2, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(2, callData3.callCounter);
    TEST_ASSERT_EQUAL(2, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest21Buffer2(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"q\":{\"a\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest21Buffer3(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test21(void) {
    const char* rawRequest = "{\"q\":{\"q\":{\"a\":1}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..q..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..q..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..q..q..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "1";
    callData1.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest21Buffer2, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecuteForTest21Buffer3, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "{\"a\":1}";
    callData4.expectedValue.length = 7;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "1";
    callData5.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(3, callData2.callCounter);
    TEST_ASSERT_EQUAL(2, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest22Buffer2(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"q\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"q\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test22(void) {
    const char* rawRequest = "{\"q\":{\"a\":{\"q\":1}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..q..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..a..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..q..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "{\"q\":1}";
    callData1.expectedValue.length = 7;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest22Buffer2, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "1";
    callData3.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "1";
    callData4.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "{\"q\":1}";
    callData5.expectedValue.length = 7;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(3, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest23Buffer1(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"q\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
        case 4:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"q\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 5:
            TEST_ASSERT_EQUAL(19, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"q\":1}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest23Buffer2(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"q\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"q\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest23Buffer5(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test23(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":{\"q\":1}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..a..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..q..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..a..q..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest23Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest23Buffer2, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "{\"q\":1}";
    callData3.expectedValue.length = 7;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "1";
    callData4.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = NULL;
    callData5.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecuteForTest23Buffer5, &callData5));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(6, callData1.callCounter);
    TEST_ASSERT_EQUAL(4, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(3, callData5.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest24(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
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
        case 4:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"q\":{\"a\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 5:
            TEST_ASSERT_EQUAL(19, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"q\":{\"a\":1}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test24(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"q\":{\"a\":1}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest24, &callData1));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(6, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest25(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
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
        case 4:
            TEST_ASSERT_EQUAL(19, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"q\":{\"a\":{\"a\":1}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test25(void) {
    const char* rawRequest = "{\"a\":{\"q\":{\"a\":{\"a\":1}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest25, &callData1));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(5, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest26(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
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

static void test26(void) {
    const char* rawRequest = "{\"q\":{\"a\":{\"a\":{\"a\":1}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest26, &callData1));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(4, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest27Buffer1(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
        case 2:
        case 3:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
        case 5:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
            TEST_ASSERT_EQUAL(19, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":1}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest27Buffer2(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
        case 4:
        case 5:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
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

static const lemonError fakeExecuteForTest27Buffer3(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
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


static void test27(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":{\"a\":1}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..a..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..a..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest27Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest27Buffer2, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecuteForTest27Buffer3, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "1";
    callData4.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(7, callData1.callCounter);
    TEST_ASSERT_EQUAL(7, callData2.callCounter);
    TEST_ASSERT_EQUAL(4, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest28Buffer1(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 5:
        case 6:
        case 7:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 8:
        case 9:
            TEST_ASSERT_EQUAL(19, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":1}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 10:
            TEST_ASSERT_EQUAL(25, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":{\"a\":1}}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest28Buffer2(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 10:
        case 11:
        case 12:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":1}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 13:
            TEST_ASSERT_EQUAL(19, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{\"a\":1}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest28Buffer3(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
        case 7:
        case 8:
        case 9:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 10:
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

static const lemonError fakeExecuteForTest28Buffer4(const string *value, calledCallback *data) {
    printf("XOOOUUUTTT %.*s\r\n", value->length, value->data);
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
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

static void test28(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":{\"a\":{\"a\":1}}}}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..a..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..a..a..a..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..a..a..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest28Buffer1, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest28Buffer2, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecuteForTest28Buffer3, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = NULL;
    callData4.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecuteForTest28Buffer4, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "1";
    callData5.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(11, callData1.callCounter);
    TEST_ASSERT_EQUAL(14, callData2.callCounter);
    TEST_ASSERT_EQUAL(11, callData3.callCounter);
    TEST_ASSERT_EQUAL(5, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest29Buffer1And7(const string *value, calledCallback *data) {
    /*printf("PAOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest29Buffer8(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static const lemonError fakeExecuteForTest29Buffer5And6(const string *value, calledCallback *data) {
    /* printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter); */
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest29Buffer9(const string *value, calledCallback *data) {
    /*printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
        case 4:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 5:
            TEST_ASSERT_EQUAL(14, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":{}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static void test29(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":{}}}}}";
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
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    calledCallback callData9;
    calledCallback callData10;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest29Buffer1And7,&callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "{}";
    callData2.expectedValue.length = 2;
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
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecuteForTest29Buffer5And6, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest29Buffer5And6, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = NULL;
    callData7.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecuteForTest29Buffer1And7,&callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecuteForTest29Buffer8,&callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = NULL;
    callData9.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecuteForTest29Buffer9, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue.data = "{}";
    callData10.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer10, fakeExecute, &callData10));
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
    TEST_ASSERT_EQUAL(4, callData8.callCounter);
    TEST_ASSERT_EQUAL(6, callData9.callCounter);
    TEST_ASSERT_EQUAL(1, callData10.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest30Buffer1And7(const string *value, calledCallback *data) {
    /*printf("PAOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest30Buffer8(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static const lemonError fakeExecuteForTest30Buffer5And6(const string *value, calledCallback *data) {
    /* printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter); */
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest30Buffer9(const string *value, calledCallback *data) {
    /*printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
        case 4:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 5:
            TEST_ASSERT_EQUAL(14, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":[]}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static void test30(void) {
    const char* rawRequest = "{\"a\":{\"a\":{\"a\":[]}}}}";
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
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    calledCallback callData9;
    calledCallback callData10;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest30Buffer1And7,&callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "[]";
    callData2.expectedValue.length = 2;
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
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecuteForTest30Buffer5And6, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest30Buffer5And6, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = NULL;
    callData7.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecuteForTest30Buffer1And7,&callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecuteForTest30Buffer8,&callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = NULL;
    callData9.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecuteForTest30Buffer9, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue.data = "[]";
    callData10.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer10, fakeExecute, &callData10));
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
    TEST_ASSERT_EQUAL(4, callData8.callCounter);
    TEST_ASSERT_EQUAL(6, callData9.callCounter);
    TEST_ASSERT_EQUAL(1, callData10.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest31Buffer1And7(const string *value, calledCallback *data) {
    /*printf("PAOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest31Buffer8(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static const lemonError fakeExecuteForTest31Buffer6(const string *value, calledCallback *data) {
    /* printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter); */
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest31Buffer9(const string *value, calledCallback *data) {
    /*printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
        case 4:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 5:
            TEST_ASSERT_EQUAL(14, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":[]}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static void test31(void) {
    const char* rawRequest = "[[[[[[{\"a\":{\"a\":{\"a\":[]}}}]]]]]]";
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
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$..a..a..a..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    calledCallback callData9;
    calledCallback callData10;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest31Buffer1And7,&callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "[]";
    callData2.expectedValue.length = 2;
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
    callData5.expectedValue.data = callData4.expectedValue.data;
    callData5.expectedValue.length = callData4.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest31Buffer6, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = NULL;
    callData7.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecuteForTest31Buffer1And7,&callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecuteForTest31Buffer8,&callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = NULL;
    callData9.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecuteForTest31Buffer9, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue.data = "[]";
    callData10.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer10, fakeExecute, &callData10));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(3, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(0, callData3.callCounter);
    TEST_ASSERT_EQUAL(0, callData4.callCounter);
    TEST_ASSERT_EQUAL(0, callData5.callCounter);
    TEST_ASSERT_EQUAL(2, callData6.callCounter);
    TEST_ASSERT_EQUAL(3, callData7.callCounter);
    TEST_ASSERT_EQUAL(4, callData8.callCounter);
    TEST_ASSERT_EQUAL(6, callData9.callCounter);
    TEST_ASSERT_EQUAL(1, callData10.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest32Buffer2(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("2", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":2}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(25, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"c\":{\"b\":1},\"d\":{\"b\":2}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest32Buffer3(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":2}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(25, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"c\":{\"b\":1},\"d\":{\"b\":2}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(31, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":{\"c\":{\"b\":1},\"d\":{\"b\":2}}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static const lemonError fakeExecuteForTest32Buffer4(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("2", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":2}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(25, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"c\":{\"b\":1},\"d\":{\"b\":2}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 5:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("3", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":3}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 7:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("4", value->data, value->length);
            ++(data->callCounter);
            break;
        case 8:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":4,\"e\":5}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest32Buffer5(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":1}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":3}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":{\"b\":3}}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":4,\"e\":5}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(43, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"d\":{\"b\":{\"b\":3}},\"b\":{\"b\":4,\"e\":5},\"f\":6}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest32Buffer6(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":2}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":3}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":{\"b\":3}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static const lemonError fakeExecuteForTest32Buffer10(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("1", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("3", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":3}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(1, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("4", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":4,\"e\":5}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test32(void) {
    /*
{
    "a" : {
        "b" : {
            "c" : {
                "b" : 1
            },
            "d" : {
                "b" : 2
            }
        }
    },
    "c" : {
        "d" : {
            "b" : {
                "b" : 3
            }
        },
        "b" : {
            "b" : 4,
            "e" : 5
        },
        "f" : 6
    }
}
     */
    const char* rawRequest = "{\"a\":{\"b\":{\"c\":{\"b\":1},\"d\":{\"b\":2}}},\"c\":{\"d\":{\"b\":{\"b\":3}},\"b\":{\"b\":4,\"e\":5},\"f\":6}}";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;

    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..c..b..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a..b..";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$..b..";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$..c..";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$..d..";
    jsonPathQueryBuffer jsonPathQueryBuffer7[] = "$..e..";
    jsonPathQueryBuffer jsonPathQueryBuffer8[] = "$..f..";
    jsonPathQueryBuffer jsonPathQueryBuffer9[] = "$..a..e..";
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$..c..b..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    calledCallback callData9;
    calledCallback callData10;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "1";
    callData1.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest32Buffer2, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecuteForTest32Buffer3, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = NULL;
    callData4.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecuteForTest32Buffer4, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = NULL;
    callData5.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecuteForTest32Buffer5, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest32Buffer6, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = "5";
    callData7.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecute, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = "6";
    callData8.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecute, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = "*** Not found (it's a fake message) ***";
    callData9.expectedValue.length = strlen(callData9.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecute, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue.data = NULL;
    callData10.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer10, fakeExecuteForTest32Buffer10, &callData10));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(5, callData2.callCounter);
    TEST_ASSERT_EQUAL(4, callData3.callCounter);
    TEST_ASSERT_EQUAL(9, callData4.callCounter);
    TEST_ASSERT_EQUAL(5, callData5.callCounter);
    TEST_ASSERT_EQUAL(3, callData6.callCounter);
    TEST_ASSERT_EQUAL(1, callData7.callCounter);
    TEST_ASSERT_EQUAL(1, callData8.callCounter);
    TEST_ASSERT_EQUAL(0, callData9.callCounter);
    TEST_ASSERT_EQUAL(5, callData10.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}



static const lemonError fakeExecuteForTest33Buffer2(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(9, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"d\":123}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(3, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("234", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(3, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("678", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
        case 5:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
            TEST_ASSERT_EQUAL(15, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"z\":[],\"b\":{}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static void test33(void) {
    const char* rawRequest = "[\"a\",{\"c\":{\"b\":{\"d\":123}}},[{\"b\":234},[\"b\",\"b\"],{\"x\":567,\"b\":678,\"p\":{\"q\":111,\"b\":{\"z\":[],\"b\":{}}}}]]";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..b..";
    calledCallback callData1;
    calledCallback callData2;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "*** Not found (it's a fake message) ***";
    callData1.expectedValue.length = strlen(callData1.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest33Buffer2, &callData2));

    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(0, callData1.callCounter);
    TEST_ASSERT_EQUAL(7, callData2.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest34Buffer1(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest34Buffer6And10(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest34Buffer7(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static const lemonError fakeExecuteForTest34Buffer8And14And16(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest34Buffer9And15(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest34Buffer12(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static const lemonError fakeExecuteForTest34Buffer13(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 3:
            TEST_ASSERT_EQUAL(16, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[{\"a\":[]}]}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test34(void) {
    const char* rawRequest = "[{\"a\":[{\"a\":[]}]}]";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$.*";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$.*.*";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$.*.*.*";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$.*.*.*..*";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$.*.*..*";
    jsonPathQueryBuffer jsonPathQueryBuffer7[] = "$.*..*";
    jsonPathQueryBuffer jsonPathQueryBuffer8[] = "$.*..*..*";
    jsonPathQueryBuffer jsonPathQueryBuffer9[] = "$..*..*..*";
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$.*.*.*..";
    jsonPathQueryBuffer jsonPathQueryBuffer11[] = "$.*.*.*.*..";
    jsonPathQueryBuffer jsonPathQueryBuffer12[] = "$..*..*..*..";
    jsonPathQueryBuffer jsonPathQueryBuffer13[] = "$..*";
    jsonPathQueryBuffer jsonPathQueryBuffer14[] = "$.*.*..*..";
    jsonPathQueryBuffer jsonPathQueryBuffer15[] = "$..*.*..*..";
    jsonPathQueryBuffer jsonPathQueryBuffer16[] = "$.*..*.*..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    calledCallback callData9;
    calledCallback callData10;
    calledCallback callData11;
    calledCallback callData12;
    calledCallback callData13;
    calledCallback callData14;
    calledCallback callData15;
    calledCallback callData16;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest34Buffer1,&callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "{\"a\":[{\"a\":[]}]}";
    callData2.expectedValue.length = 16;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "[{\"a\":[]}]";
    callData3.expectedValue.length = 10;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "{\"a\":[]}";
    callData4.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "[]";
    callData5.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest34Buffer6And10, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = NULL;
    callData7.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecuteForTest34Buffer7, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecuteForTest34Buffer8And14And16, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = NULL;
    callData9.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecuteForTest34Buffer9And15, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue.data = NULL;
    callData10.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer10, fakeExecuteForTest34Buffer6And10, &callData10));

    callData11.callCounter = 0;
    callData11.expectedValue.data = "[]";
    callData11.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer11, fakeExecute, &callData11));

    callData12.callCounter = 0;
    callData12.expectedValue.data = NULL;
    callData12.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer12, fakeExecuteForTest34Buffer12, &callData12));

    callData13.callCounter = 0;
    callData13.expectedValue.data = NULL;
    callData13.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer13, fakeExecuteForTest34Buffer13, &callData13));

    callData14.callCounter = 0;
    callData14.expectedValue.data = NULL;
    callData14.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer14, fakeExecuteForTest34Buffer8And14And16, &callData14));

    callData15.callCounter = 0;
    callData15.expectedValue.data = NULL;
    callData15.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer15, fakeExecuteForTest34Buffer9And15, &callData15));

    callData16.callCounter = 0;
    callData16.expectedValue.data = NULL;
    callData16.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer16, fakeExecuteForTest34Buffer8And14And16, &callData16));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(4, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    TEST_ASSERT_EQUAL(2, callData6.callCounter);
    TEST_ASSERT_EQUAL(3, callData7.callCounter);
    TEST_ASSERT_EQUAL(3, callData8.callCounter);
    TEST_ASSERT_EQUAL(4, callData9.callCounter);
    TEST_ASSERT_EQUAL(2, callData10.callCounter);
    TEST_ASSERT_EQUAL(1, callData11.callCounter);
    TEST_ASSERT_EQUAL(5, callData12.callCounter);
    TEST_ASSERT_EQUAL(4, callData13.callCounter);
    TEST_ASSERT_EQUAL(3, callData14.callCounter);
    TEST_ASSERT_EQUAL(4, callData15.callCounter);
    TEST_ASSERT_EQUAL(3, callData16.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest35Buffer1(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
        case 2:
        case 3:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
        case 5:
            TEST_ASSERT_EQUAL(7, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{},{}]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
            TEST_ASSERT_EQUAL(13, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[{},{}]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 7:
            TEST_ASSERT_EQUAL(15, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[{},{}]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}



static void test35(void) {
    const char* rawRequest = "[{\"a\":[{\"a\":[{},{}]}]}]";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest35Buffer1,&callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(8, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest36Buffer1(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
        case 3:
        case 5:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 6:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"c\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 7:
            TEST_ASSERT_EQUAL(28, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[]},{\"b\":[]},{\"c\":[]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static void test36(void) {
    const char* rawRequest = "[{\"a\":[{\"a\":[]},{\"b\":[]},{\"c\":[]}]}]";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest36Buffer1,&callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(8, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest37Buffer1(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
        case 3:
        case 5:
        case 6:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 7:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 8:
            TEST_ASSERT_EQUAL(28, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[]},{\"b\":[]},{\"a\":[]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static void test37(void) {
    const char* rawRequest = "[{\"a\":[{\"a\":[]},{\"b\":[]},{\"a\":[]}]}]";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest37Buffer1,&callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(9, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest38Buffer1(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
        case 3:
        case 5:
        case 6:
            TEST_ASSERT_EQUAL(5, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"a\"]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
        case 7:
            TEST_ASSERT_EQUAL(11, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[\"a\"]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(11, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":[\"a\"]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 8:
            TEST_ASSERT_EQUAL(37, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[\"a\"]},{\"b\":[\"a\"]},{\"a\":[\"a\"]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static void test38(void) {
    const char* rawRequest = "[{\"a\":[{\"a\":[\"a\"]},{\"b\":[\"a\"]},{\"a\":[\"a\"]}]}]";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest38Buffer1,&callData1));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(9, callData1.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}


static const lemonError fakeExecuteForTest39Buffer1(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
        case 1:
        case 3:
            TEST_ASSERT_EQUAL(5, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"a\"]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(11, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[\"a\"]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 4:
            TEST_ASSERT_EQUAL(11, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"b\":[\"a\"]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 5:
        case 6:
            TEST_ASSERT_EQUAL(2, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 7:
        case 8:
            TEST_ASSERT_EQUAL(4, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[[]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 9:
        case 10:
            TEST_ASSERT_EQUAL(6, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[[[]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 11:
        case 12:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[[[[]]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 13:
        case 14:
            TEST_ASSERT_EQUAL(10, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[[[[[]]]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 15:
        case 16:
            TEST_ASSERT_EQUAL(12, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[[[[[[]]]]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 17:
        case 18:
            TEST_ASSERT_EQUAL(5, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"x\"]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 19:
        case 20:
            TEST_ASSERT_EQUAL(9, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"o\":\"y\"}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 21:
        case 22:
            TEST_ASSERT_EQUAL(11, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"o\":\"y\"}]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 23:
        case 24:
            TEST_ASSERT_EQUAL(5, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"z\"]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 25:
        case 26:
            TEST_ASSERT_EQUAL(25, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 27:
        case 28:
            TEST_ASSERT_EQUAL(40, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 29:
        case 30:
            TEST_ASSERT_EQUAL(42, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 31:
        case 32:
            TEST_ASSERT_EQUAL(48, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"a\",[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 33:
            TEST_ASSERT_EQUAL(54, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":[\"a\",[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]]}", value->data, value->length);
            ++(data->callCounter);
            break;
        case 34:
            TEST_ASSERT_EQUAL(80, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[\"a\"]},{\"b\":[\"a\"]},{\"a\":[\"a\",[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static const lemonError fakeExecuteForTest39Buffer2(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(5, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"a\"]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(48, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"a\",[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(80, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[\"a\"]},{\"b\":[\"a\"]},{\"a\":[\"a\",[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}


static void test39(void) {
    const char* rawRequest = "[{\"a\":[{\"a\":[\"a\"]},{\"b\":[\"a\"]},{\"a\":[\"a\",[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]]}]}]";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$..a..";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$..a";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$..o..";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = NULL;
    callData1.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecuteForTest39Buffer1,&callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = NULL;
    callData2.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecuteForTest39Buffer2,&callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "y";
    callData3.expectedValue.length = 1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute,&callData3));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    /*TEST_ASSERT_EQUAL(35, callData1.callCounter);*/
    TEST_ASSERT_EQUAL(3, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static const lemonError fakeExecuteForTest40Buffer2(const string *value, calledCallback *data) {
    /*printf("ZXOOOUUUTTT %.*s\r\n", value->length, value->data);
    ++(data->callCounter);*/
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(5, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"a\"]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(48, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[\"a\",[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]]", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(80, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("[{\"a\":[\"a\"]},{\"b\":[\"a\"]},{\"a\":[\"a\",[[[[[[[[]]]]]],[[\"x\"],[{\"o\":\"y\"}],[\"z\"]]]]]}]", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test40(void) {
    const char* rawRequest = "[{\"a\":[{\"a\":[]}]}]";
    httpRequest request;
    jsonPathRequest jsonRequest, jsonRequest_backup;
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$[*]";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$[*][*]";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$[*][*][*]";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$[*][*][*]..[*]";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$[*][*]..[*]";
    jsonPathQueryBuffer jsonPathQueryBuffer7[] = "$[*]..[*]";
    jsonPathQueryBuffer jsonPathQueryBuffer8[] = "$[*]..[*]..[*]";
    jsonPathQueryBuffer jsonPathQueryBuffer9[] = "$..[*]..[*]..[*]";
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$[*][*][*]..";
    jsonPathQueryBuffer jsonPathQueryBuffer11[] = "$[*][*][*][*]..";
    jsonPathQueryBuffer jsonPathQueryBuffer12[] = "$..[*]..[*]..[*]..";
    jsonPathQueryBuffer jsonPathQueryBuffer13[] = "$..[*]";
    jsonPathQueryBuffer jsonPathQueryBuffer14[] = "$[*][*]..[*]..";
    jsonPathQueryBuffer jsonPathQueryBuffer15[] = "$..[*][*]..[*]..";
    jsonPathQueryBuffer jsonPathQueryBuffer16[] = "$[*]..[*][*]..";
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    calledCallback callData9;
    calledCallback callData10;
    calledCallback callData11;
    calledCallback callData12;
    calledCallback callData13;
    calledCallback callData14;
    calledCallback callData15;
    calledCallback callData16;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "{\"a\":[{\"a\":[]}]}";
    callData2.expectedValue.length = 16;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "[{\"a\":[]}]";
    callData3.expectedValue.length = 10;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "{\"a\":[]}";
    callData4.expectedValue.length = 8;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "[]";
    callData5.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecute, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = NULL;
    callData6.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecuteForTest34Buffer6And10, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = NULL;
    callData7.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecuteForTest34Buffer7, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecuteForTest34Buffer8And14And16, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = NULL;
    callData9.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecuteForTest34Buffer9And15, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue.data = NULL;
    callData10.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer10, fakeExecuteForTest34Buffer6And10, &callData10));

    callData11.callCounter = 0;
    callData11.expectedValue.data = "[]";
    callData11.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer11, fakeExecute, &callData11));

    callData12.callCounter = 0;
    callData12.expectedValue.data = NULL;
    callData12.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer12, fakeExecuteForTest34Buffer12, &callData12));

    callData13.callCounter = 0;
    callData13.expectedValue.data = NULL;
    callData13.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer13, fakeExecuteForTest34Buffer13, &callData13));

    callData14.callCounter = 0;
    callData14.expectedValue.data = NULL;
    callData14.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer14, fakeExecuteForTest34Buffer8And14And16, &callData14));

    callData15.callCounter = 0;
    callData15.expectedValue.data = NULL;
    callData15.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer15, fakeExecuteForTest34Buffer9And15, &callData15));

    callData16.callCounter = 0;
    callData16.expectedValue.data = NULL;
    callData16.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer16, fakeExecuteForTest34Buffer8And14And16, &callData16));
    memcpy(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));

    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    TEST_ASSERT_EQUAL(2, callData6.callCounter);
    TEST_ASSERT_EQUAL(3, callData7.callCounter);
    TEST_ASSERT_EQUAL(3, callData8.callCounter);
    TEST_ASSERT_EQUAL(4, callData9.callCounter);
    TEST_ASSERT_EQUAL(2, callData10.callCounter);
    TEST_ASSERT_EQUAL(1, callData11.callCounter);
    TEST_ASSERT_EQUAL(5, callData12.callCounter);
    TEST_ASSERT_EQUAL(4, callData13.callCounter);
    TEST_ASSERT_EQUAL(3, callData14.callCounter);
    TEST_ASSERT_EQUAL(4, callData15.callCounter);
    TEST_ASSERT_EQUAL(3, callData16.callCounter);
}
/*
[
    "a",
    {
        "c" : {
            "b" : {
                "d" : 123
            }
        }
    },
    [
        {
            "b" : 234
        },
        ["b", "b"],
        {
            "x": 567,
            "b": 678,
            "p": {
                "q" : 111,
                "b": {
                    "b": [],
                    "b": {}
                }
            }
        }
    ]
]
 */



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
    /*RUN_TEST(test12);*/
    /*RUN_TEST(test13);*/
    /*RUN_TEST(test17);*/
    /*RUN_TEST(test32);*/


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
    RUN_TEST(test11);

    RUN_TEST(test15);
    RUN_TEST(test16);

    RUN_TEST(test18);
    RUN_TEST(test19);
    RUN_TEST(test20);
    RUN_TEST(test21);
    RUN_TEST(test22);
    RUN_TEST(test23);
    RUN_TEST(test24);
    RUN_TEST(test25);
    RUN_TEST(test26);
    RUN_TEST(test27);
    RUN_TEST(test28);

    RUN_TEST(test29);
    RUN_TEST(test30);
    RUN_TEST(test31);

    RUN_TEST(test32);
    RUN_TEST(test33);
    RUN_TEST(test34);

    RUN_TEST(test35);
    RUN_TEST(test36);
    RUN_TEST(test37);
    RUN_TEST(test38);
    RUN_TEST(test39);
    RUN_TEST(test40);








    return (UnityEnd());
}

/* GOOD TEST:
$.b..b

{
    "b" : {
        "b" : 1
    }
}

 */
