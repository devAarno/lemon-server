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

#include "socket.h"

#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "../lemonHttp/httpRequest.h"
#include "../lemonHttp/parser.h"
#include "../lemonHttp/lemonError.h"

#include "socketError.h"

socketError runServer(const uint16_t port, httpRequest *request) {
    struct sockaddr_in servaddr;
    int const listenfd = socket(AF_INET, SOCK_STREAM, 0);


    if (-1 == listenfd) {
        return SE_SOCKET_ERROR;
    }

    memset(&servaddr, 0, sizeof (servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (0 != bind(listenfd, (const struct sockaddr *) &servaddr, sizeof (servaddr))) {
        return SE_BIND_ERROR;
    }

    if (0 != listen(listenfd, 128)) {
        return SE_LISTEN_ERROR;
    }

    for (;;) {
        size_t iterator = 0;
        const int readerfd = accept(listenfd, NULL, NULL);

        if (-1 == readerfd) {
            return SE_ACCEPT_ERROR;
        }
        /* manageConnection(readerfd, request); */
        for (iterator = 0; iterator < request->elementsCount; ++iterator) {
            const requestElement element = request->elements[iterator];
            if (ON_START_CALLBACK == element.type) {
                if (LE_OK != element.data.onStartCallback.handler(readerfd, element.data.onStartCallback.data)) {
                    return SE_LISTEN_ERROR /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */;
                }
            }
        }

        read(readerfd, &(request->privateBuffer), sizeof (request->privateBuffer));

        if (LE_OK != parseHTTP(request)) {
            close(readerfd);
            break;
        }

        /* request->parsedStackSize = 0; */

        for (iterator = 0; iterator < request->elementsCount; ++iterator) {
            const requestElement element = request->elements[iterator];
            if (FINAL_ON_SUCCESS_CALLBACK == element.type) {
                if (LE_OK != element.data.finalOnSuccessCallback.handler(readerfd, element.data.finalOnSuccessCallback.data)) {
                    return SE_LISTEN_ERROR /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */;
                }
            }
        }

        close(readerfd);
    }
}
