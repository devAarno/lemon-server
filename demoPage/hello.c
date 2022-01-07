/*
 * Copyright (C) 2017, 2018, 2019, 2020, 2021, 2022 Parkhomenko Stanislav
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

#include <string.h>
#include <unistd.h>

#include "../src/lemonHttp/httpRequest.h"
#include "../src/net/socket.h"

static const char *response = "HTTP/1.1 200 OK\r\n\
Server: Lemon Server v0.0\r\n\
Content-Length: 88\r\n\
Content-Type: text/html\r\n\
Connection: Closed\r\n\
\r\n\
<html>\
<body>\
<h1>Hello, World!</h1>\
</body>\
</html>";

static void page(int fd, const httpRequest *r) {
    if (strncmp("hello.html", getUriOfHttpRequest(r)->data, 10)) {
        write(fd, response, strlen(response));
        close(fd);
    }
}

int main() {
    runServer(40000, page);
}
