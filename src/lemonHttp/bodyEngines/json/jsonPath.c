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

#include "./jsonPath.h"

#include <stddef.h>
#include "../../lemonError.h"
#include "./jsonPathQueryBuffer.h"
#include "./jsonPathParser.h"
#include "../../../../3rdParty/unity/git/src/unity.h"
#include "../../../net/socket.h"

const lemonError initJsonPathRequest(jsonPathRequest *r) {
    if (NULL == r) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        r->elementsCount = 0;
        return LE_OK;
    }
}

const lemonError appendJsonPathRequest(jsonPathRequest *p, jsonPathQueryBuffer *b, jsonPathExecutonHandler handler, changingData *data) {
    if ((NULL == b) || (NULL == p) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        /*jsonPathElement *currRoot = &((p->elements)[0]);
        size_t currArrayPosition = 0;
        const lemonError err = parseJSONPath(p, b);

        * err is not checked *

        while (currArrayPosition < p->elementsCount) {
            * handler+data assign to ROOT but last element may be more convenient *
            if (ROOT == ((p->elements)[currArrayPosition]).type) {
                currRoot = &((p->elements)[currArrayPosition]);
            }
            ++currArrayPosition;
        }
        currRoot->callback.handler = handler;
        currRoot->callback.data = data;*/

        const size_t newRootPlace = p->elementsCount;
        const lemonError err = parseJSONPath(p, b);

        if (LE_OK != err) {
            return err;
        }

        if (0 != newRootPlace) {
            size_t currArrayPosition = newRootPlace - 1;

            while ((0 != currArrayPosition) && (((p->elements)[currArrayPosition]).type != ROOT)) {
                --currArrayPosition;
            }

            if ((0 == currArrayPosition) && (((p->elements)[currArrayPosition]).type != ROOT)) {
                return LE_INCORRECT_INPUT_VALUES;
            }

            (p->elements)[currArrayPosition].next = &((p->elements)[newRootPlace]);
        }

        (p->elements)[newRootPlace].callback.handler = handler;
        (p->elements)[newRootPlace].callback.data = data;
        (p->elements)[newRootPlace].next = NULL;
        (p->elements)[newRootPlace].level = 0;
        (p->elements)[newRootPlace].index = 0;
        (p->elements)[newRootPlace].containerStartPosition = NULL;
    }
    return LE_OK;
}
