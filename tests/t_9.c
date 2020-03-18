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
    char *expectedValue;
    char callCounter;
} calledCallback;

void setUp(void) {
}

void tearDown(void) {
}

static const lemonError fakeExecute(const string *value, calledCallback *data) {
    puts("Rule 34");
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue, value->data, value->length);
    ++(data->callCounter);
}

static const lemonError fakeExecuteForBuffer5(const string *value, calledCallback *data) {
    puts("Rule 35");
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
            break;
    }
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
                              "      \"number\": \"0123-4567-8888\"\n"
                              "    },\n"
                              "    {\n"
                              "      \"type\"  : \"home\",\n"
                              "      \"number\": \"0123-4567-8910\"\n"
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
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue = "doe";
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue = "John";
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer2, fakeExecute, &callData2));

    callData3.callCounter = 0;
    callData3.expectedValue = "Nara";
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer3, fakeExecute, &callData3));

    /*callData4.callCounter = 0;
    callData4.expectedValue = "*** Not found (it's a fake message) ***";
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer4, fakeExecute, &callData4));


    callData5.callCounter = 0;
    callData5.expectedValue = NULL;
    TEST_ASSERT_EQUAL(LE_OK,
                      appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer5, fakeExecuteForBuffer5, &callData5));*/

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));
    TEST_ASSERT_EQUAL(1, callData1.callCounter);
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
    TEST_ASSERT_EQUAL(1, callData3.callCounter);
    /*TEST_ASSERT_EQUAL(0, callData4.callCounter);
    TEST_ASSERT_EQUAL(3, callData5.callCounter);*/
}

static void test2(void) {
    const char* rawRequest1 = "{\n"
                              "  \"a\": {\n"
                              "    \"b\": \"c\",\n"
                              "    \"d\": {\n"
                              "      \"e\": {\n"
                              "        \"f\": \"g\",\n"
                              "        \"h\": \"i\"\n"
                              "      },\n"
                              "      \"j\": \"k\"\n"
                              "    },\n"
                              "    \"l\": \"m\"\n"
                              "  },\n"
                              "  \"n\": {\n"
                              "    \"o\": \"p\",\n"
                              "    \"q\": \"r\"\n"
                              "  }\n"
                              "}";
    httpRequest request;
    jsonPathRequest jsonRequest;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$";
    calledCallback callData1;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue = "";
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));
    TEST_ASSERT_EQUAL(1, callData1.callCounter);
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test1);
    /*RUN_TEST(test2);*/
    return (UnityEnd());
}
