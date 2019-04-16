/*
 * Copyright (C) 2017, 2018, 2019 Parkhomenko Stanislav
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

#define TESTNAME TestJSONParsing

static const char* rawRequest1 = "{}";

void setUp(void) {
}

void tearDown(void) {
}

static void test_body(void) {
    /*JsonPathPromise p1;
    JsonPathPromise out;

    p1 = createPromise(&request, "$.array[1].dictionary.b");
    while (out = scanBodyStream(&body, &promises)) {
        switch (out) {
            case p1:
                printf("qqq");
                break;
            default:
                printf("zzz");
                break;
        }
    }*/

    httpRequest request;

    TEST_ASSERT_EQUAL(LE_OK, initHttpRequest(&request, FAKE_DESCRIPTOR));

    strncpy(request.privateBuffer, rawRequest1, sizeof (request.privateBuffer));


    TEST_ASSERT_EQUAL(LE_OK, parseJSON(&request));
}

int main() {
    UnityBegin(__FILE__);
    RUN_TEST(test_body);
    return (UnityEnd());
}
