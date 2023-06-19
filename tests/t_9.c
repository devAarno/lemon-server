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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../3rdParty/unity/git/src/unity.h"
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

static lemonError fakeExecute(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    ++(data->callCounter);
    return LE_OK;
}

static lemonError fakeExecuteForTest1Buffer5(const string *value, calledCallback *data) {
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
    return LE_OK;
}

static lemonError fakeExecuteForTest1Buffer8(const string *value, calledCallback *data) {
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
    return LE_OK;
}

static lemonError fakeExecuteForTrueValue(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    TEST_ASSERT_EQUAL(TRUE, isTrueString(*value));
    ++(data->callCounter);
    return LE_OK;
}

static lemonError fakeExecuteForFalseValue(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    TEST_ASSERT_EQUAL(TRUE, isFalseString(*value));
    ++(data->callCounter);
    return LE_OK;
}

static lemonError fakeExecuteForNullValue(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL(data->expectedValue.length, value->length);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue.data, value->data, value->length);
    TEST_ASSERT_EQUAL(TRUE, isNullString(*value));
    ++(data->callCounter);
    return LE_OK;
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
    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "doe";
    callData1.expectedValue.length = strlen(callData1.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "John";
    callData2.expectedValue.length = strlen(callData2.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "Nara";
    callData3.expectedValue.length = strlen(callData3.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "*** Not found (it's a fake message) ***";
    callData4.expectedValue.length = strlen(callData4.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = NULL;
    callData5.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer5, fakeExecuteForTest1Buffer5, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = "iPhone";
    callData6.expectedValue.length = strlen(callData6.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer6, fakeExecute, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = "0123-4567-8910";
    callData7.expectedValue.length = strlen(callData7.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer7, fakeExecute, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = NULL;
    callData8.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer8, fakeExecuteForTest1Buffer8, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue = getTrueString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer9, fakeExecuteForTrueValue, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue = getFalseString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer10, fakeExecuteForFalseValue, &callData10));

    callData11.callCounter = 0;
    callData11.expectedValue = getNullString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer11, fakeExecuteForNullValue, &callData11));

    callData12.callCounter = 0;
    callData12.expectedValue.data = "{\n"
                                    "      \"comments\": null,\n"
                                    "      \"type\"  : \"iPhone\",\n"
                                    "      \"isPrimary\": true,\n"
                                    "      \"number\": \"1123-4567-8888\"\n"
                                    "    }"; /* Be careful with spaces! */
    callData12.expectedValue.length = strlen(callData12.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer12, fakeExecute, &callData12));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));
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
    TEST_ASSERT_EQUAL(1, callData12.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static void test2(void) {
    const char* rawRequest1 = "{\"a\":{}, \"b\": [], \"c\" : {\"x\" : \"y\", \"o\"  :  \"oo\"}, \"d\" : [true, false, null ]}";
    httpRequest request;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$.a";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$.b";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$.c";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$.d";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$.d[2]";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$.d[0]";
    jsonPathQueryBuffer jsonPathQueryBuffer7[] = "$.d[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer8[] = "$.a.";
    jsonPathQueryBuffer jsonPathQueryBuffer9[] = "$.b.";
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$.c.";
    jsonPathQueryBuffer jsonPathQueryBuffer11[] = "$.d.";
    jsonPathQueryBuffer jsonPathQueryBuffer12[] = "$.d[2].";
    jsonPathQueryBuffer jsonPathQueryBuffer13[] = "$.d[0].";
    jsonPathQueryBuffer jsonPathQueryBuffer14[] = "$.d[1].";
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
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    callData1.callCounter = 0;
    callData1.expectedValue.data = "{}";
    callData1.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "[]";
    callData2.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = "{\"x\" : \"y\", \"o\"  :  \"oo\"}";
    callData3.expectedValue.length = strlen(callData3.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer3, fakeExecute, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "[true, false, null ]";
    callData4.expectedValue.length = strlen(callData4.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue = getNullString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer5, fakeExecuteForNullValue, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue = getTrueString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer6, fakeExecuteForTrueValue, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue = getFalseString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer7, fakeExecuteForFalseValue, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue = callData1.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer8, fakeExecute, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue = callData2.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer9, fakeExecute, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue = callData3.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer10, fakeExecute, &callData10));

    callData11.callCounter = 0;
    callData11.expectedValue = callData4.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer11, fakeExecute, &callData11));

    callData12.callCounter = 0;
    callData12.expectedValue = callData5.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer12, fakeExecuteForNullValue, &callData12));

    callData13.callCounter = 0;
    callData13.expectedValue = callData6.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer13, fakeExecuteForTrueValue, &callData13));

    callData14.callCounter = 0;
    callData14.expectedValue = callData7.expectedValue;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer14, fakeExecuteForFalseValue, &callData14));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    TEST_ASSERT_EQUAL(1, callData6.callCounter);
    TEST_ASSERT_EQUAL(1, callData7.callCounter);
    TEST_ASSERT_EQUAL(1, callData8.callCounter);
    TEST_ASSERT_EQUAL(1, callData9.callCounter);
    TEST_ASSERT_EQUAL(1, callData10.callCounter);
    TEST_ASSERT_EQUAL(1, callData11.callCounter);
    TEST_ASSERT_EQUAL(1, callData12.callCounter);
    TEST_ASSERT_EQUAL(1, callData13.callCounter);
    TEST_ASSERT_EQUAL(1, callData14.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static lemonError fakeExecuteForTest3Buffer3(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(3, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("a12", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(3, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("a22", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static lemonError fakeExecuteForTest3Buffer11(const string *value, calledCallback *data) {
    switch (data->callCounter) {
        case 0:
            TEST_ASSERT_EQUAL(3, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("a11", value->data, value->length);
            ++(data->callCounter);
            break;
        case 1:
            TEST_ASSERT_EQUAL(3, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("a12", value->data, value->length);
            ++(data->callCounter);
            break;
        case 2:
            TEST_ASSERT_EQUAL(23, value->length);
            TEST_ASSERT_EQUAL_STRING_LEN("{\"a\":{\"a\":[\"a1\",\"a2\"]}}", value->data, value->length);
            ++(data->callCounter);
            break;
        default:
            TEST_FAIL_MESSAGE("Incorrect callCounter");
            break;
    }
    return LE_OK;
}

static void test3(void) {
    /* [false,[["a11", "a12"],["a21","a22"]],[],[{"a":{"a":{"a":["a1","a2"]}}}]] */
    const char* rawRequest1 = "[false, [[\"a11\", \"a12\"],[\"a21\",\"a22\"]],[],[{\"a\":{\"a\":{\"a\":[\"a1\",\"a2\"]}}}]]";
    httpRequest request;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$[0]";
    jsonPathQueryBuffer jsonPathQueryBuffer2[] = "$[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer3[] = "$[1][*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer4[] = "$[3][0].a";
    jsonPathQueryBuffer jsonPathQueryBuffer5[] = "$[3][0]";
    jsonPathQueryBuffer jsonPathQueryBuffer6[] = "$[3][0].*";
    jsonPathQueryBuffer jsonPathQueryBuffer7[] = "$[*][*].*.*.*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer8[] = "$[*][*].*.*.*";
    jsonPathQueryBuffer jsonPathQueryBuffer9[] = "$[*][*].*.*";
    jsonPathQueryBuffer jsonPathQueryBuffer10[] = "$[*][0].a.a";
    jsonPathQueryBuffer jsonPathQueryBuffer11[] = "$[*][0][*]";
    jsonPathQueryBuffer jsonPathQueryBuffer12[] = "$[2]";
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
    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    callData1.callCounter = 0;
    callData1.expectedValue = getFalseString();
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer1, fakeExecuteForFalseValue, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue.data = "[[\"a11\", \"a12\"],[\"a21\",\"a22\"]]";
    callData2.expectedValue.length = strlen(callData2.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue.data = NULL;
    callData3.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer3, fakeExecuteForTest3Buffer3, &callData3));

    callData4.callCounter = 0;
    callData4.expectedValue.data = "{\"a\":{\"a\":[\"a1\",\"a2\"]}}";
    callData4.expectedValue.length = strlen(callData4.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer4, fakeExecute, &callData4));

    callData5.callCounter = 0;
    callData5.expectedValue.data = "{\"a\":{\"a\":{\"a\":[\"a1\",\"a2\"]}}}";
    callData5.expectedValue.length = strlen(callData5.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer5, fakeExecute, &callData5));

    callData6.callCounter = 0;
    callData6.expectedValue.data = callData4.expectedValue.data;
    callData6.expectedValue.length = callData4.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer6, fakeExecute, &callData6));

    callData7.callCounter = 0;
    callData7.expectedValue.data = "a2";
    callData7.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer7, fakeExecute, &callData7));

    callData8.callCounter = 0;
    callData8.expectedValue.data = "[\"a1\",\"a2\"]";
    callData8.expectedValue.length = strlen(callData8.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer8, fakeExecute, &callData8));

    callData9.callCounter = 0;
    callData9.expectedValue.data = "{\"a\":[\"a1\",\"a2\"]}";
    callData9.expectedValue.length = strlen(callData9.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer9, fakeExecute, &callData9));

    callData10.callCounter = 0;
    callData10.expectedValue.data = "{\"a\":[\"a1\",\"a2\"]}";
    callData10.expectedValue.length = strlen(callData10.expectedValue.data);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer10, fakeExecute, &callData10));

    callData11.callCounter = 0;
    callData11.expectedValue.data = NULL;
    callData11.expectedValue.length = 0;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer11, fakeExecuteForTest3Buffer11, &callData11));

    callData12.callCounter = 0;
    callData12.expectedValue.data = "[]";
    callData12.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer12, fakeExecute, &callData12));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(2, callData3.callCounter);
    TEST_ASSERT_EQUAL(1, callData4.callCounter);
    TEST_ASSERT_EQUAL(1, callData5.callCounter);
    TEST_ASSERT_EQUAL(1, callData6.callCounter);
    TEST_ASSERT_EQUAL(1, callData7.callCounter);
    TEST_ASSERT_EQUAL(1, callData8.callCounter);
    TEST_ASSERT_EQUAL(1, callData9.callCounter);
    TEST_ASSERT_EQUAL(1, callData10.callCounter);
    TEST_ASSERT_EQUAL(3, callData11.callCounter);
    TEST_ASSERT_EQUAL(1, callData12.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

static void test4(void) {
    /* [false,[["a11", "a12"],["a21","a22"]],[],[{"a":{"a":{"a":["a1","a2"]}}}]] */
    const char* rawRequest1 = "[false, [[\"a11\", \"a12\"],[\"a21\",\"a22\"]],[],[{\"a\":{\"a\":{\"a\":[\"a1\",\"a2\"]}}}]]";
    httpRequest request;
    jsonPathQueryBuffer jsonPathQueryBuffer13[] = "$[*][0][*].*[*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer14[] = "$[*][0][*][*][*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer15[] = "$.*[*][*][*][*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer16[] = "$.*[*].*[*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer17[] = "$[*][*][*][*][*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer18[] = "$[*].*[*].*[*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer19[] = "$.*[*][*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer20[] = "$.*[0][*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer21[] = "$[3][0][*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer22[] = "$[3][*][*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer23[] = "$[3].*[*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer24[] = "$[3].*.*[*].*[1]";
    calledCallback callData13;
    calledCallback callData14;
    calledCallback callData15;
    calledCallback callData16;
    calledCallback callData17;
    calledCallback callData18;
    calledCallback callData19;
    calledCallback callData20;
    calledCallback callData21;
    calledCallback callData22;
    calledCallback callData23;
    calledCallback callData24;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    callData13.callCounter = 0;
    callData13.expectedValue.data = "a2";
    callData13.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer13, fakeExecute, &callData13));

    callData14.callCounter = 0;
    callData14.expectedValue.data = callData13.expectedValue.data;
    callData14.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer14, fakeExecute, &callData14));

    callData15.callCounter = 0;
    callData15.expectedValue.data = callData13.expectedValue.data;
    callData15.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer15, fakeExecute, &callData15));

    callData16.callCounter = 0;
    callData16.expectedValue.data = callData13.expectedValue.data;
    callData16.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer16, fakeExecute, &callData16));

    callData17.callCounter = 0;
    callData17.expectedValue.data = callData13.expectedValue.data;
    callData17.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer17, fakeExecute, &callData17));

    callData18.callCounter = 0;
    callData18.expectedValue.data = callData13.expectedValue.data;
    callData18.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer18, fakeExecute, &callData18));

    callData19.callCounter = 0;
    callData19.expectedValue.data = callData13.expectedValue.data;
    callData19.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer19, fakeExecute, &callData19));

    callData20.callCounter = 0;
    callData20.expectedValue.data = callData13.expectedValue.data;
    callData20.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer20, fakeExecute, &callData20));

    callData21.callCounter = 0;
    callData21.expectedValue.data = callData13.expectedValue.data;
    callData21.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer21, fakeExecute, &callData21));

    callData22.callCounter = 0;
    callData22.expectedValue.data = callData13.expectedValue.data;
    callData22.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer22, fakeExecute, &callData22));

    callData23.callCounter = 0;
    callData23.expectedValue.data = callData13.expectedValue.data;
    callData23.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer23, fakeExecute, &callData23));

    callData24.callCounter = 0;
    callData24.expectedValue.data = callData13.expectedValue.data;
    callData24.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer24, fakeExecute, &callData24));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    TEST_ASSERT_EQUAL(1, callData13.callCounter);
    TEST_ASSERT_EQUAL(1, callData14.callCounter);
    TEST_ASSERT_EQUAL(1, callData15.callCounter);
    TEST_ASSERT_EQUAL(1, callData16.callCounter);
    TEST_ASSERT_EQUAL(1, callData17.callCounter);
    TEST_ASSERT_EQUAL(1, callData18.callCounter);
    TEST_ASSERT_EQUAL(1, callData19.callCounter);
    TEST_ASSERT_EQUAL(1, callData20.callCounter);
    TEST_ASSERT_EQUAL(1, callData21.callCounter);
    TEST_ASSERT_EQUAL(1, callData22.callCounter);
    TEST_ASSERT_EQUAL(1, callData23.callCounter);
    TEST_ASSERT_EQUAL(1, callData24.callCounter);
    /*TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest));*/
}

static void test5(void) {
    const char* rawRequest1 = "[]";
    const char* rawRequest2 = "{}";
    const char* rawRequest3 = "[false,[[\"a11\",\"a12\"],[\"a21\",\"a22\"]],[],[{\"a\":{\"a\":{\"a\":[\"a1\"]}}}]]";
    const char* rawRequest4 = "[false,[[\"a11\",\"a12\"],[\"a21\",\"a22\"]],[],[{\"a\":{\"a\":[\"a1\",\"a2\"]}}]]";
    const char* rawRequest5 = "[false,[[\"a11\",\"a12\"],[\"a21\",\"a22\"]],[],[{\"a\":{\"a\":{\"a\":{\"a\":[\"a1\",\"a2\"]}}}}]]";
    const char* rawRequest6 = "[false,[[\"a11\",\"a12\"],[\"a21\",\"a22\"]],[{\"a\":{\"a\":{\"a\":\"a2\"}}}],[{\"a\":{\"a\":{\"a\":[\"a1\"]}}}]]";
    httpRequest request;
    jsonPathQueryBuffer jsonPathQueryBuffer13[] = "$[*][0][*].*[*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer14[] = "$[*][0][*][*][*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer15[] = "$.*[*][*][*][*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer16[] = "$.*[*].*[*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer17[] = "$[*][*][*][*][*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer18[] = "$[*].*[*].*[*][1]";
    jsonPathQueryBuffer jsonPathQueryBuffer19[] = "$.*[*][*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer20[] = "$.*[0][*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer21[] = "$[3][0][*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer22[] = "$[3][*][*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer23[] = "$[3].*[*][*].*[1]";
    jsonPathQueryBuffer jsonPathQueryBuffer24[] = "$[3].*.*[*].*[1]";
    calledCallback callData13;
    calledCallback callData14;
    calledCallback callData15;
    calledCallback callData16;
    calledCallback callData17;
    calledCallback callData18;
    calledCallback callData19;
    calledCallback callData20;
    calledCallback callData21;
    calledCallback callData22;
    calledCallback callData23;
    calledCallback callData24;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request));

    callData13.callCounter = 0;
    callData13.expectedValue.data = "a2";
    callData13.expectedValue.length = 2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer13, fakeExecute, &callData13));

    callData14.callCounter = 0;
    callData14.expectedValue.data = callData13.expectedValue.data;
    callData14.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer14, fakeExecute, &callData14));

    callData15.callCounter = 0;
    callData15.expectedValue.data = callData13.expectedValue.data;
    callData15.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer15, fakeExecute, &callData15));

    callData16.callCounter = 0;
    callData16.expectedValue.data = callData13.expectedValue.data;
    callData16.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer16, fakeExecute, &callData16));

    callData17.callCounter = 0;
    callData17.expectedValue.data = callData13.expectedValue.data;
    callData17.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer17, fakeExecute, &callData17));

    callData18.callCounter = 0;
    callData18.expectedValue.data = callData13.expectedValue.data;
    callData18.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer18, fakeExecute, &callData18));

    callData19.callCounter = 0;
    callData19.expectedValue.data = callData13.expectedValue.data;
    callData19.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer19, fakeExecute, &callData19));

    callData20.callCounter = 0;
    callData20.expectedValue.data = callData13.expectedValue.data;
    callData20.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer20, fakeExecute, &callData20));

    callData21.callCounter = 0;
    callData21.expectedValue.data = callData13.expectedValue.data;
    callData21.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer21, fakeExecute, &callData21));

    callData22.callCounter = 0;
    callData22.expectedValue.data = callData13.expectedValue.data;
    callData22.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer22, fakeExecute, &callData22));

    callData23.callCounter = 0;
    callData23.expectedValue.data = callData13.expectedValue.data;
    callData23.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer23, fakeExecute, &callData23));

    callData24.callCounter = 0;
    callData24.expectedValue.data = callData13.expectedValue.data;
    callData24.expectedValue.length = callData13.expectedValue.length;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&request, jsonPathQueryBuffer24, fakeExecute, &callData24));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    strncpy(request.privateBuffer, rawRequest2, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    strncpy(request.privateBuffer, rawRequest3, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    strncpy(request.privateBuffer, rawRequest4, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    strncpy(request.privateBuffer, rawRequest5, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    strncpy(request.privateBuffer, rawRequest6, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));

    TEST_ASSERT_EQUAL(0, callData13.callCounter);
    TEST_ASSERT_EQUAL(0, callData14.callCounter);
    TEST_ASSERT_EQUAL(0, callData15.callCounter);
    TEST_ASSERT_EQUAL(0, callData16.callCounter);
    TEST_ASSERT_EQUAL(0, callData17.callCounter);
    TEST_ASSERT_EQUAL(0, callData18.callCounter);
    TEST_ASSERT_EQUAL(0, callData19.callCounter);
    TEST_ASSERT_EQUAL(0, callData20.callCounter);
    TEST_ASSERT_EQUAL(0, callData21.callCounter);
    TEST_ASSERT_EQUAL(0, callData22.callCounter);
    TEST_ASSERT_EQUAL(0, callData23.callCounter);
    TEST_ASSERT_EQUAL(0, callData24.callCounter);
    /* TEST_ASSERT_EQUAL_MEMORY(&jsonRequest_backup, &jsonRequest, sizeof(jsonRequest)); */
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test1);
    RUN_TEST(test2);

    /* `test3` and `test4` have been split due to MAX_ELEMENTS limit */
    RUN_TEST(test3);
    RUN_TEST(test4);
    RUN_TEST(test5);
    /* need more tests with deep json */

    return (UnityEnd());
}
