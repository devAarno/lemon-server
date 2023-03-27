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

#define TESTNAME TestJSONPathByCallbacksRoot

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

static void commonTest(const char* rawRequest, const char* expected) {
    httpRequest request;
    jsonPathRequest jsonRequest;
    jsonPathQueryBuffer jsonPathQueryBuffer1[] = "$";
    calledCallback callData;
    /* Fake json path request */
    TEST_ASSERT_EQUAL(LE_OK, initJsonPathRequest(&jsonRequest));

    callData.callCounter = 0;
    callData.expectedValue.data = expected;
    callData.expectedValue.length = strlen(expected);
    TEST_ASSERT_EQUAL(LE_OK, appendJsonPathRequest(&jsonRequest, jsonPathQueryBuffer1, fakeExecute, &callData));

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest, sizeof (request.privateBuffer));
    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request, &jsonRequest));
    TEST_ASSERT_EQUAL(1, callData.callCounter);
}

static void test1(void) {
    const char* json = "{}";
    commonTest(json, json);
}

static void test2(void) {
    const char* json = "[]";
    commonTest(json, json);
}

static void test3(void) {
    const char* json = "true";
    commonTest(json, json);
}

static void test4(void) {
    const char* json = "false";
    commonTest(json, json);
}

static void test5(void) {
    const char* json = "123";
    commonTest(json, json);
}

static void test6(void) {
    const char* json = "123";
    commonTest(json, json);
}

static void test7(void) {
    const char* json = "0.3e10";
    commonTest(json, json);
}

static void test8(void) {
    const char* json = "-1.3E-2";
    commonTest(json, json);
}

static void test9(void) {
    commonTest("\"true\"", "true");
}

static void test10(void) {
    const char* json = "[\"true\", \"false\"]";
    commonTest(json, json);
}

static void test11(void) {
    const char* json = "[true, false, \"true\", \"false\", -1.3E-2      \r  \n    , {   \"demo\" : [   \"hello\"   \n   , \"world\"    ]    } \r  ,{\"extra\" : {\"empty\":null}}, null   ]";
    commonTest(json, json);
}

static void test12(void) {
    const char* json = "{ \"hello\" : \"world\"}";
    commonTest(json, json);
}

static void test13(void) {
    commonTest("    null     ", "null");
}

static void test14(void) {
    commonTest("    -1.3E-2     ", "-1.3E-2");
}

static void test15(void) {
    commonTest("    0.00000006     ", "0.00000006");
}

static void test16(void) {
    const char* json = "{\n"
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
    commonTest(json, json);
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
    RUN_TEST(test10);
    RUN_TEST(test11);
    RUN_TEST(test12);
    RUN_TEST(test13);
    RUN_TEST(test14);
    RUN_TEST(test15);
    return (UnityEnd());
}
