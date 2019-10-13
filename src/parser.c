#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "../headers/parser.h"
#include "../headers/http.h"
#include "../headers/logger.h"

// Вспомогательная функция, возвращающая подстроку строки s,
// которая идет до первого вхождения передаваемого в функцию символа c
char* strprv(char *s, char c){
    // получаем индекс символа 'c' в строке s (по первому вхождению)
    short c_idx = strchr(s, c) - s;
    
    char *buf = (char*)malloc(c_idx);

    strncpy(buf, s, c_idx);
    buf[c_idx] = '\0';

    return buf;
}

size_t get_headers_count(char *r) {
    char* header_pointer = strstr(r, "\r\n");
    char* header_end = strstr(r, "\r\n\r\n");
    size_t headers_count = 0;

    if (header_pointer == NULL)
    {
        _log(EVENT_LOG_ERR, "Wrong request");
        return -1;
    }

    while (header_pointer != header_end) {
       header_pointer += 2;
       headers_count++;
       header_pointer = strstr(header_pointer, "\r\n");
   }

   return headers_count;
}

struct header* get_req_headers(char* r, size_t hc) {
    struct header* h = (struct header*)malloc(hc * sizeof(struct header));

    char* h_ptr = r;
    char* delim = "\n";

    printf("\n-----\n%s\n\n", r);

    h[0].head = strtok(r, delim);
    h[0].len = strlen(h[0].head);

    size_t a = 0;
    for (size_t i = 1; i < hc; i++)
    {
        h[i].head = strtok(NULL, delim);
        h[i].len = strlen(h[i].head);
        h[i].head[h[i].len] = "\0";
    };

    return h;
}

char* parse_req(char* s, struct response* r)
{
    if (s == NULL)
    {
        return "Bad requst: empty string.";
    }

    // Получаем инфу о количестве хэдеров в реквесте 
    size_t headers_count = get_headers_count(s);

    // printf("Header counter: %d\n", headers_count);

    // Отделяем хедеры от реквеста
    char *method = strprv(s, ' ');
    char *URI = strprv(s + strlen(method) + 1, ' ');
    char *http_v = strprv(s + strlen(method) + 1 + strlen(URI) + 1, '\n');

    // char* headers = s + strlen(method) + strlen(URI) + strlen(http_v);
    // Парсим хедеры запроса
    r->headers = get_req_headers(s + strlen(method) + 1 + strlen(URI) + 1 + strlen(http_v) + 1, headers_count);

    // printf("HEADERS: \n");
    // for (size_t i = 0; i < headers_count; i++) {
    //     printf("%d: %s\n",i, headers[i].head);
    // }

    #ifdef DEBUG
        printf("Method: |%s|\nURI: |%s|\nHTTP: |%s|\nHeaders: %s\n", method, URI, http_v, "head");
    #endif

    if (strstr(method, "GET") == NULL && strstr(method, "HEAD") == NULL )
    {
        return "Wrong request method"; 
    }

    r->status = HTTP_OK_STATUS;
    r->http_v = http_v;

    return NULL;
}