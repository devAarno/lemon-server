/*
 * Copyright (C) 2017, 2018, 2019, 2020 Parkhomenko Stanislav
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

#define TESTNAME TestJSONPathByCallbacks

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
}

static const lemonError fakeExecuteForBuffer5(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(12, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("naist street", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(4, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("Nara", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(8, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("630-0192", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
}

static const lemonError fakeExecuteForBuffer8(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(14, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("0123-4567-8888", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(14, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("0123-4567-8910", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
}

static const lemonError fakeExecuteForTrueValue(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    TEST_ASSERT_EQUAL(TRUE, isTrueString(*value));
    ++(data->callCounter);
}

static const lemonError fakeExecuteForFalseValue(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    TEST_ASSERT_EQUAL(TRUE, isFalseString(*value));
    ++(data->callCounter);
}

static const lemonError fakeExecuteForNullValue(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    TEST_ASSERT_EQUAL(TRUE, isNullString(*value));
    ++(data->callCounter);
}

static void test1(void) {
    const char* rawRequest1 = "{\n"
                              "  \"firstName\": \"John\",\n"
                              "  \"lastName\" : \"doe\",\n"
                              "  \"age\"      : 26,\n"
                              "  \"address\"  : {\n"
                              "    \"streetAddress\": \"naist street\",\n"
                              "    \"city\"         : \"Nara\",\n"
                              "    \"postalCode\"   : \"630-0192\"\n"
                              "  },\n"
                              "  \"phoneNumbers\": [\n"
                              "    {\n"
                              "      \"type\"  : \"iPhone\",\n"
                              "      \"number\": \"0123-4567-8888\",\n"
                              "      \"isPrimary\": false,\n"
                              "      \"comments\": null\n"
                              "    },\n"
                              "    {\n"
                              "      \"isPrimary\": false,\n"
                              "      \"comments\": \"Do not disturb at night!\",\n"
                              "      \"type\"  : \"home\",\n"
                              "      \"number\": \"0123-4567-8910\"\n"
                              "    }\n"
                              "  ],\n"
                              "  \"workNumbers\": [\n"
                              "    {\n"
                              "      \"comments\": null,\n"
                              "      \"type\"  : \"iPhone\",\n"
                              "      \"isPrimary\": true,\n"
                              "      \"number\": \"1123-4567-8888\"\n"
                              "    },\n"
                              "    {\n"
                              "      \"type\"  : \"WhatsApp\",\n"
                              "      \"comments\": null,\n"
                              "      \"isPrimary\": false,\n"
                              "      \"number\": \"+91797744784\"\n"
                              "    }\n"
                              "  ]\n"
                              "}";
    httpRequest request;
    jsonPathRequest jsonRequest;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$.lastName";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$.firstName";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$.*.city";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$.*.type";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$.address.*";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$.phoneNumbers[0].type";
    jsonPathQueryBuffer jsonPathQueryBuffer7[] = "$.phoneNumbers[1].number";
    jsonPathQueryBuffer jsonPathQueryBuffer8[] = "$.phoneNumbers[*].number";
    jsonPathQueryBuffer jsonPathQueryBuffer9[] = "$.workNumbers[0].isPrimary";
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$.workNumbers[1].isPrimary";
    jsonPathQueryBuffer jsonPathQueryBuffer11[] = "$.workNumbers[1].comments";
    jsonPathQueryBuffer jsonPathQueryBuffer12[] = "$.workNumbers[0]";
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
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "doe";
    callData1.expectedValue.length = strlen(callData1.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "John";
    callData2.expectedValue.length = strlen(callData2.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "Nara";
    callData3.expectedValue.length = strlen(callData3.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "*** Not found (it's a fake message) ***";
    callData4.expectedValue.length = strlen(callData4.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = NULL;
    callData5.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK,
                      appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecuteForBuffer5, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = "iPhone";
    callData6.expectedValue.length = strlen(callData6.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer6, fakeExecute, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = "0123-4567-8910";
    callData7.expectedValue.length = strlen(callData7.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer7, fakeExecute, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer8, fakeExecuteForBuffer8, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue = getTrueString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer9, fakeExecuteForTrueValue, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue = getFalseString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer10, fakeExecuteForFalseValue, &callData10));

    callData11.callCounter = 0;
    callData11.expectedValue = getNullString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer11, fakeExecuteForNullValue, &callData11));

    /*callData12.callCounter = 0;
    callData12.expectedValue.data = "{\n"
                                    "      \"comments\": null,\n"
                                    "      \"type\"  : \"iPhone\",\n"
                                    "      \"isPrimary\": true,\n"
                                    "      \"number\": \"1123-4567-8888\"\n"
                                    "    }"; * Be careful with spaces! *
    callData12.expectedValue.length = strlen(callData12.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer12, fakeExecute, &callData12));*/

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));
    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    TEST_ASSERT_EQUAL(0, callData4.callCounter);
    TEST_ASSERT_EQUAL(3, callData5.callCounter);
    TEST_ASSERT_EQUAL(1, callData6.callCounter);
    TEST_ASSERT_EQUAL(1, callData7.callCounter);
    TEST_ASSERT_EQUAL(2, callData8.callCounter);
    TEST_ASSERT_EQUAL(1, callData9.callCounter);
    TEST_ASSERT_EQUAL(1, callData10.callCounter);
    TEST_ASSERT_EQUAL(1, callData11.callCounter);
    /*TEST_ASSERT_EQUAL(1, callData12.callCounter);*/
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test1);
    return (UnityEnd());
}
