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

ssize_t readData(httpRequest *request) {
    /*ssize_t res;
    if (NULL == request->body.data) {
        res = read(request->descriptor, &(request->privateBuffer), sizeof (request->privateBuffer));
    } else {
        res = read(request->descriptor, request->body.data, request->body.length);
    }
    return ((res >= 0) ? (request->body.length = res) : res);*/
}

static void manageConnection(int fd, const handle h) {
    
    /*if (0 == readData(&request)) {
        close(fd);
        return ;
    }

    h(fd, &request);

    if (LE_OK != parseHTTP(&request)) {
        close(fd);
        return ;
    }*/
}

const socketError runServer(const uint16_t port, const handle h) {
    httpRequest request;
    struct sockaddr_in servaddr;
    size_t iterator;
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

    if (LE_OK != initHttpRequest(&request)) {
        close(listenfd);
        return SE_LISTEN_ERROR /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */;
    }

    if (LE_OK != h(&request)) {
        close(listenfd);
        return SE_LISTEN_ERROR /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */;
    }

    for (;;) {
        const int readerfd = accept(listenfd, NULL, NULL);
        if (-1 == readerfd) {
            return SE_ACCEPT_ERROR;
        }
        /* manageConnection(readerfd, h); */
        for (iterator = 0; iterator < request.elementsCount; ++iterator) {
            if (ON_START_CALLBACK == request.elements[iterator].type) {
                if (LE_OK != request.elements[iterator].data.onStartCallback.handler(readerfd, request.elements[iterator].data.onStartCallback.data)) {
                    return SE_LISTEN_ERROR /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */;
                }
            }
        }

        read(readerfd, &(request.privateBuffer), sizeof (request.privateBuffer));

        if (LE_OK != parseHTTP(&request)) {
            close(readerfd);
            break;
        }

        request.parsedStackSize = 0;

        for (iterator = 0; iterator < request.elementsCount; ++iterator) {
            if (FINAL_ON_SUCCESS_CALLBACK == request.elements[iterator].type) {
                if (LE_OK != request.elements[iterator].data.finalOnSuccessCallback.handler(readerfd, request.elements[iterator].data.finalOnSuccessCallback.data)) {
                    return SE_LISTEN_ERROR /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */;
                }
            }
        }

        close(readerfd);
    }
}
