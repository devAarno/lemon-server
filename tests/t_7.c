/*
 * Copyright (C) 2017, 2018 Parkhomenko Stanislav
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
#include "../src/lemonHttp/httpRequest.h"
#include "../src/lemonHttp/string.h"

#define TESTNAME TestJSONPath

void setUp(void) {
}

void tearDown(void) {
}

static void test_body(void) {
    httpRequest fakeRequest;
    
    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, "$.store.book[*].author"));
    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, "$.store.*"));
    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, "$.book[2]"));
    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, "$.book[2].zzz.xxx.yyy"));
    
    TEST_ASSERT_EQUAL(LE_OK, parseJSONPath(&fakeRequest, "$['store']['book'][0]['title']"));
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test_body);
    return (UnityEnd());
}
