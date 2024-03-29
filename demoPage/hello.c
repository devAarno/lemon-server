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
#include <string.h>
#include <unistd.h>
#include "../src/lemonHttp/httpRequest.h"
#include "../src/net/socket.h"

static const char response[] = "HTTP/1.1 200 OK\r\n\
Server: Lemon Server v0.0\r\n\
Content-Length: 312\r\n\
Content-Type: application/xhtml+xml\r\n\
Connection: Closed\r\n\
\r\n\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\
<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">\
<head><title>Lemon Server Demo Page title</title></head>\
<body><h1>Hello, world!</h1></body>\
</html>";

typedef struct {
    string url;
    int fd;
    boolean isGet;
} parsedElements;

parsedElements elements;

static lemonError checkIsGet(const string *value, parsedElements *data) {
    if ((3 == value->length) && 0 == strncmp("GET", value->data, value->length)) {
        data->isGet = TRUE;
    }
    return LE_OK;
}

static lemonError returnPage(const string *value, parsedElements *data) {
    if ((TRUE == data->isGet) && (12 == value->length) && 0 == strncmp("/hello.xhtml", value->data, value->length)) {
        write(data->fd, response, strlen(response));
    }
    return LE_OK;
}

static void page(int fd, const httpRequest *r) {
    elements.fd = fd;
    elements.isGet = FALSE;

    if (LE_OK != appendHttpMethodRequest(r, (httpMethodExecutionHandler) checkIsGet, &elements)) {

    }

    if (LE_OK != appendHttpUriRequest(r, (httpMethodExecutionHandler) returnPage, &elements)) {

    }
}

int main() {
    runServer(40000, page);
    /* Under construction */
}
