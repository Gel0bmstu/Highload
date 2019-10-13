#pragma once

#include "../src/http.c"

// STATUS

// 200
#define HTTP_OK_STATUS "200 OK\0"

// 400
#define HTTP_BAD_REQUEST_STATUS "400 Bad Request\0"
#define HTTP_FORBIDEN_STATUS "403 Forbidden\0"
#define HTTP_NOT_FOUND_STATUS "404 Not Found\0"
#define HTTP_METHOD_NOT_STATUS "405 Method Not Allowed\0"

// 500
#define HTTP_INTERNAL_SERVER_ERROR_STATUS "500 Internal Server Error\0"
// ------------------------------------------------------------------------------

// HTTP VER

#define HTTP_V_1_0 "HTTP/1.0\0"
#define HTTP_V_1_1 "HTTP/1.1\0"
// ------------------------------------------------------------------------------

// HEADERS

#define STR_CONNECTION_CLOSE_HEADER "Connection: close\r\n\0"
#define STR_DEFAULT_DATE_HEADER "Date: Thu, 1 Jan 1970 00:00:00 GMT\r\n\0"
#define STR_SERVER_HEADER "Server: "APP_NAME"/"VERSION"\r\n\0"
#define STR_CONTENT_TYPE_HEADER "Content-Type: \0"
#define STR_CONTENT_LENGTH_HEADER "Content-Length: \0"
// ------------------------------------------------------------------------------

// STRUCTURES

#define EMPTY_RES_STRUCT {0, NULL, NULL}
#define EMPTY_REQ_STRUCT {NULL, NULL, NULL, NULL}

typedef struct header
{
    char* head;
    short len;
} header;

typedef struct request
{
    char* http_v;
    char* method;
    char* URI;
    char* headers;
} request;

typedef struct response
{
    char* status;
    char* http_v;
    struct header* headers;
} response;
