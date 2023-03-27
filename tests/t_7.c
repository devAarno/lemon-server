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
#include "../src/lemonHttp/bodyEngines/json/jsonPathParser.h"
#include "../src/lemonHttp/string.h"

#define TESTNAME TestJSONPath

static char jsonPath1[] = "$.sto\\*re.au\\u0074hor";
static char jsonPath2[] = "$.store.*";
static char jsonPath3[] = "$.book[2]";
static char jsonPath4[] = "$.book[2].zzz.xxx.yyy";
static char jsonPath5[] = "$['store']['book'][0]['title']";
static char jsonPath6[] = "$[*].*[*].*[*].tool";
static char jsonPath7[] = "$..horse..[*]..*[2]";
static char jsonPath8[] = "$..";
static char jsonPath9[] = "$..horse..[*]..";
static char jsonPath10[] = "$..horse..[*]";
static char jsonPath11[] = "$..horse..";
static char jsonPath12[] = "$..horse..*";
static char jsonPath13[] = "$..*";

void setUp(void) {
}

void tearDown(void) {
}

/* static void test1(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath1));

    TEST_ASSERT_EQUAL(3, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_STRING_LEN("sto*re", fakeRequest.elements[1].value.data, fakeRequest.elements[1].value.length);
    TEST_ASSERT_EQUAL(6, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("author", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(6, fakeRequest.elements[2].value.length);
}

static void test2(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath2));

    TEST_ASSERT_EQUAL(3, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_STRING_LEN("store", fakeRequest.elements[1].value.data, fakeRequest.elements[1].value.length);
    TEST_ASSERT_EQUAL(5, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANY, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[2].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[2].value.length);
}

static void test3(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath3));

    TEST_ASSERT_EQUAL(3, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_STRING_LEN("book", fakeRequest.elements[1].value.data, fakeRequest.elements[1].value.length);
    TEST_ASSERT_EQUAL(4, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_INDEX, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("2", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(1, fakeRequest.elements[2].value.length);
}

static void test4(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath4));

    TEST_ASSERT_EQUAL(6, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_STRING_LEN("book", fakeRequest.elements[1].value.data, fakeRequest.elements[1].value.length);
    TEST_ASSERT_EQUAL(4, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_INDEX, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("2", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(1, fakeRequest.elements[2].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[3].type);
    TEST_ASSERT_EQUAL_STRING_LEN("zzz", fakeRequest.elements[3].value.data, fakeRequest.elements[3].value.length);
    TEST_ASSERT_EQUAL(3, fakeRequest.elements[3].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[4].type);
    TEST_ASSERT_EQUAL_STRING_LEN("xxx", fakeRequest.elements[4].value.data, fakeRequest.elements[4].value.length);
    TEST_ASSERT_EQUAL(3, fakeRequest.elements[4].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[5].type);
    TEST_ASSERT_EQUAL_STRING_LEN("yyy", fakeRequest.elements[5].value.data, fakeRequest.elements[5].value.length);
    TEST_ASSERT_EQUAL(3, fakeRequest.elements[5].value.length);
}

static void test5(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath5));

    TEST_ASSERT_EQUAL(5, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_STRING_LEN("store", fakeRequest.elements[1].value.data, fakeRequest.elements[1].value.length);
    TEST_ASSERT_EQUAL(5, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("book", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(4, fakeRequest.elements[2].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_INDEX, fakeRequest.elements[3].type);
    TEST_ASSERT_EQUAL_STRING_LEN("0", fakeRequest.elements[3].value.data, fakeRequest.elements[3].value.length);
    TEST_ASSERT_EQUAL(1, fakeRequest.elements[3].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[4].type);
    TEST_ASSERT_EQUAL_STRING_LEN("title", fakeRequest.elements[4].value.data, fakeRequest.elements[4].value.length);
    TEST_ASSERT_EQUAL(5, fakeRequest.elements[4].value.length);
}

static void test6(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath6));

    TEST_ASSERT_EQUAL(7, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANYINDEX, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[1].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANY, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[2].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[2].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANYINDEX, fakeRequest.elements[3].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[3].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[3].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANY, fakeRequest.elements[4].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[4].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[4].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANYINDEX, fakeRequest.elements[5].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[5].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[5].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[6].type);
    TEST_ASSERT_EQUAL_STRING_LEN("tool", fakeRequest.elements[6].value.data, fakeRequest.elements[6].value.length);
    TEST_ASSERT_EQUAL(4, fakeRequest.elements[6].value.length);
}

static void test7(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath7));

    TEST_ASSERT_EQUAL(8, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[1].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("horse", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(5, fakeRequest.elements[2].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[3].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[3].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[3].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANYINDEX, fakeRequest.elements[4].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[4].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[4].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[5].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[5].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[5].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANY, fakeRequest.elements[6].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[6].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[6].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_INDEX, fakeRequest.elements[7].type);
    TEST_ASSERT_EQUAL_STRING_LEN("2", fakeRequest.elements[7].value.data, fakeRequest.elements[7].value.length);
    TEST_ASSERT_EQUAL(1, fakeRequest.elements[7].value.length);
}

static void test8(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath8));

    TEST_ASSERT_EQUAL(2, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[1].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[1].value.length);
}

static void test9(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath9));

    TEST_ASSERT_EQUAL(6, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[1].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("horse", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(5, fakeRequest.elements[2].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[3].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[3].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[3].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANYINDEX, fakeRequest.elements[4].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[4].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[4].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[5].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[5].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[5].value.length);
}

static void test10(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath10));

    TEST_ASSERT_EQUAL(5, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[1].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("horse", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(5, fakeRequest.elements[2].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[3].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[3].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[3].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANYINDEX, fakeRequest.elements[4].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[4].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[4].value.length);
}

static void test11(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath11));

    TEST_ASSERT_EQUAL(4, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[1].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("horse", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(5, fakeRequest.elements[2].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[3].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[3].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[3].value.length);
}

static void test12(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath12));

    TEST_ASSERT_EQUAL(5, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[1].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_NAME, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_STRING_LEN("horse", fakeRequest.elements[2].value.data, fakeRequest.elements[2].value.length);
    TEST_ASSERT_EQUAL(5, fakeRequest.elements[2].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[3].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[3].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[3].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANY, fakeRequest.elements[4].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[4].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[4].value.length);
}

static void test13(void) {
    jsonPathRequest fakeRequest;

    initJsonPathRequest(&fakeRequest);

    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, jsonPath13));

    TEST_ASSERT_EQUAL(3, fakeRequest.elementsCount);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ROOT, fakeRequest.elements[0].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[0].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[0].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_RECURSIVE, fakeRequest.elements[1].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[1].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[1].value.length);

    TEST_ASSERT_EQUAL(JSONPATH_REQUEST_ANY, fakeRequest.elements[2].type);
    TEST_ASSERT_EQUAL_PTR(getEmptyString().data, fakeRequest.elements[2].value.data);
    TEST_ASSERT_EQUAL(getEmptyString().length, fakeRequest.elements[2].value.length);
}*/

int main() {
    UnityBegin(__FILE__);
    /* RUN_TEST(test1);
    RUN_TEST(test2);
    RUN_TEST(test3);
    RUN_TEST(test4);
    RUN_TEST(test5);
    RUN_TEST(test6);
    RUN_TEST(test7);
    RUN_TEST(test8);
    RUN_TEST(test9);
    RUN_TEST(test10);
    RUN_TEST(test11); */
    return (UnityEnd());
}
