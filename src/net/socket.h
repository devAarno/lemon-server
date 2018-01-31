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

#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>
#include <sys/types.h>

#include "../lemonHttp/httpRequest.h"
#include "socketError.h"

typedef void (*handle)(int fd, const httpRequest *);

ssize_t readData(httpRequest *request);

const socketError runServer(uint16_t port, const handle h);

#endif /* SOCKET_H */

