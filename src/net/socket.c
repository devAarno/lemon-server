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

#include "socket.h"

#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "../lemonHttp/http_request.h"
#include "../lemonHttp/parser.h"

ssize_t readData(httpRequest *request) {
    ssize_t res;
    if (NULL == request->body.data) {
        res = read(request->descriptor, &(request->privateBuffer), sizeof (request->privateBuffer));
    } else {
        res = read(request->descriptor, request->body.data, request->body.length);
    }
    return ((res >= 0) ? (request->body.length = res) : res);
}

static void manageConnection(int fd, const handle h) {
    httpRequest request;
    ssize_t actualRead;
    
    initHttpRequest(&request, fd);
    
    actualRead = readData(&request);
    puts(request.privateBuffer);
    parse(&request);
    h(fd, &request);
}

void runServer(uint16_t port, const handle h) {
    int listenfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in readeraddr;
    socklen_t readersize;
    int readerfd;
    

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listenfd) {
        /*return SOCKETERROR;*/
    }

    memset(&servaddr, 0, sizeof (servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (0 != bind(listenfd, (const struct sockaddr *) &servaddr, sizeof (servaddr))) {
        /*return SOCKETERROR;*/
    }

    if (0 != listen(listenfd, 128)) {
        /*return SOCKETERROR;*/
    }

    for (;;) {
        readerfd = accept(listenfd, (struct sockaddr *) &readeraddr, &readersize);
        manageConnection(readerfd, h);
    }
}
