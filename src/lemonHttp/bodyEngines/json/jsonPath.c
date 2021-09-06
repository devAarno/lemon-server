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
        r->parsedStackSize = 0;
        return LE_OK;
    }
}

const lemonError appendJsonPathRequest(jsonPathRequest *p, jsonPathQueryBuffer *b, jsonPathExecutionHandler handler, changingData *data) {
    if ((NULL == b) || (NULL == p) || (NULL == handler) || (NULL == data)) {
        return LE_NULL_IN_INPUT_VALUES;
    }
    {
        const size_t newRootPlace = p->elementsCount;
        const lemonError err = parseJSONPath(p, b);

        if (LE_OK != err) {
            return err;
        }

        (p->elements)[newRootPlace].data.root.callback.handler = handler;
        (p->elements)[newRootPlace].data.root.callback.data = data;
        (p->elements)[newRootPlace].data.root.ruleSize = p->elementsCount - newRootPlace;
    }
    return LE_OK;
}
