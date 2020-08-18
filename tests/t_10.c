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

#define TESTNAME TestJSONPathByCallbacksRoot

typedef struct {
    char *expectedValue;
    char callCounter;
} calledCallback;

void setUp(void) {
}

void tearDown(void) {
}

static const lemonError fakeExecute(const string *value, calledCallback *data) {
    printf("OOOUUUTTT %.*s\r\n", value->length, value->data);
    TEST_ASSERT_EQUAL_STRING_LEN(data->expectedValue, value->data, value->length);
    ++(data->callCounter);
}

static void test1(void) {
    const char* rawRequest1 = "{}";
    const char* rawRequest2 = "[]";
    const char* rawRequest3 = "true";
    const char* rawRequest4 = "false";
    const char* rawRequest5 = "123";
    const char* rawRequest6 = "3.14159";
    const char* rawRequest7 = "0.3e10";
    const char* rawRequest8 = "-1.3E-2";
    const char* rawRequest9 = "\"true\"";
    const char* rawRequest10 = "[\"true\", \"false\"]";
    const char* rawRequest11 = "[true, false, \"true\", \"false\", -1.3E-2      \r  \n    , {   \"demo\" : [   \"hello\"   \n   , \"world\"    ]    } \r  ,{\"extra\" : {\"empty\":null}}, null   ]";
    const char* rawRequest12 = "{ \"hello\" : \"world\"}";
    httpRequest request;
    jsonPathRequest jsonRequest;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$";
    calledCallback callData1;
    calledCallback callData2;
    calledCallback callData3;
    calledCallback callData4;
    calledCallback callData5;
    calledCallback callData6;
    calledCallback callData7;
    calledCallback callData8;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData1.callCounter = 0;
    callData1.expectedValue = rawRequest1;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData1));

    callData2.callCounter = 0;
    callData2.expectedValue = rawRequest2;
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData2));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));
    TEST_ASSERT_EQUAL(1, callData1.callCounter);

    strncpy(request.privateBuffer, rawRequest2, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));
    TEST_ASSERT_EQUAL(1, callData2.callCounter);
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test1);
    return (UnityEnd());
}
