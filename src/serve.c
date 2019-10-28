#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define DEBUG

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/thread.h>

#include "../headers/http.h"
#include "../headers/serve.h"
#include "../headers/logger.h"
#include "../headers/parser.h"

static void respond(struct evbuffer* o, struct response* r) {
    evbuffer_add(o, "HTTP/1.0\r\n\0", strlen("HTTP/1.0\r\n\0")); 
    evbuffer_add(o, " ", 1);
    evbuffer_add(o, "200 OK\r\n\0", strlen("200 OK\r\n\0"));
    evbuffer_add(o, "\r\n", 2);
    for (int i = 0; i < 3; i++) {
       evbuffer_add(o, r->headers[i].head, r->headers[i].len); 
    };

    char *date = (char*)malloc(50);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(date, 50, "Date: %a, %d %b %Y %X %Z\r\n\0", tm);

    evbuffer_add(o, date, strlen(date));
    evbuffer_add(o, "Server: highload/1.0\r\n\0", strlen("Server: highload/1.0\r\n\0"));    
    evbuffer_add(o, "\r\n", 2);

    #ifdef DEBUG
        _log(EVENT_LOG_DBG, "send!");
    #endif
};

static void set_default_headers(struct response *r)
{
    struct header *headers = (header*)malloc(3);
    headers[0].head = "Server: highload/v1.0\r\n\0";
    headers[0].len = strlen(headers[0].head);

    headers[1].head = "Connection: close\r\n\0";
    headers[1].len = strlen(headers[1].head);

    char *date = (char*)malloc(50);
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(date, 50, "Date: %a, %d %b %Y %X %Z\r\n\0", tm);

    headers[2].head = date;
    headers[2].len = strlen(date);

    r->headers = headers;
}

static void build_default_err_response(struct response *r)
{
    // set_default_heaers(r);
    r->status = HTTP_BAD_REQUEST_STATUS;
    r->http_v = HTTP_V_1_0;
};

static void read_cb( struct bufferevent *buf_ev, void *arg )
{
    // Буферы ввода/вывода 
    struct evbuffer *buf_input = bufferevent_get_input( buf_ev );
    struct evbuffer *buf_output = bufferevent_get_output( buf_ev );
    struct response res;
    struct request  req;
    char            *req_raw = "";

    // Ищем конец реквеста (строка "\r\n\r\n")
    struct evbuffer_ptr req_headers_end = evbuffer_search(buf_input, "\r\n\r\n", 4, NULL);
    if (req_headers_end.pos == -1) 
    {
        // Реквест пустой (нет хедеров или они записаны неправильно)
        _log(EVENT_LOG_ERR, "Unable to find headers end, input buffer len bytes");
        
        // Отвечаем ошибкой
        res.status = HTTP_BAD_REQUEST_STATUS;
        respond(buf_output, &res);
        return;
    }

    // Созадем строку длиной с реквест, записываем в ее конец
    // символ окончания строки
    req_raw = malloc((size_t)req_headers_end.pos + 1);
    req_raw[req_headers_end.pos] = '\0';

    // Достаем весь реквест из input буффера
    // и кладем его в виде текста в строку req_raw
    if (evbuffer_remove(buf_input, req_raw, (size_t)req_headers_end.pos) < 0) {
        _log(EVENT_LOG_ERR, "Unable to copy data from input buffer");
        free(req_raw);
        return;
    }

    #ifdef DEBUG
        _log(EVENT_LOG_DBG, req_raw);
    #endif

    // Парсим реквест -> req
    char* err = parse_req(req_raw, &res); 
    // Билдим респонз -> res 
    // ...
    
    if (err != NULL) 
    {
        // Логируем ошибку парсигна, отвечаем 405
        _log(EVENT_LOG_ERR, err);
        build_default_err_response(&res);
        respond(buf_output, &res);
    } else {
        #ifdef DEBUG
            _log(EVENT_LOG_DBG, "step 1!");
        #endif

        build_default_err_response(&res);
        // Отвечаем в случае успешного парсинга
        respond(buf_output, &res);
    }
};
 
static void event_cb( struct bufferevent *buf_ev, short events, void *arg )
{
    if( events & BEV_EVENT_ERROR )
    {
        _log(EVENT_LOG_ERR, "Object bufferevent error");
    };

    if( events & (BEV_EVENT_EOF | BEV_EVENT_ERROR) )
    {
        bufferevent_free( buf_ev );
    }
};

// Колбек старта прослушивания порта
static void accept_connection_cb( struct evconnlistener *listener,
                    evutil_socket_t fd, struct sockaddr *addr, int sock_len,
                    void *arg )
{
    struct event_base *base = evconnlistener_get_base( listener );
    struct bufferevent *buf_ev = bufferevent_socket_new( base, fd, BEV_OPT_CLOSE_ON_FREE );

    #ifdef DEBUG
        _log(EVENT_LOG_DBG, "=== Request: ===");
    #endif

    bufferevent_setcb( buf_ev, read_cb, NULL, event_cb, NULL );
    bufferevent_enable( buf_ev, (EV_READ | EV_WRITE) );
}

static void accept_error_cb( struct evconnlistener *listener, void *arg )
{
    struct event_base *base = evconnlistener_get_base( listener );
    _log( EVENT_LOG_ERR, "Connection monitor err. Termination of work.");
    event_base_loopexit( base, NULL );
    exit(-1);
}

// Выпускаем бычка
int listen_and_serve(uint16_t port) {

    struct event_base     *base;
    struct event_config   *cfg;
    struct sockaddr_in     sin;
    struct evconnlistener *listener;

    // Сетим флаг, запрещающий блокировку для event_base
    // event_config_set_flag(cfg, EVENT_BASE_FLAG_NOLOCK);

    // Сетим флаг, котрый позволяет серверу избежать ненужных системных
    // вызовов в случае, когда один и тот же fd (file descriptor - сокет) 
    // изменял свой статус более одного раза между обращением к серверу. 
    // (Это не будет работать, если мы используем не epoll)
    // event_config_set_flag(cfg, EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST);

    // Запрещаем использовать слект, потому что он блин медленный нафиг ;^(
    // event_config_avoid_method(cfg, "select");

    // Создаем eventBase с дефолтной конфигурацией
    // eventBase сам выбиет наиболее быстрый механизм
    // обрабоки событий
    // base = event_base_new_with_config(cfg);
    base = event_base_new();

    if (!base || base == NULL) {
       _log(EVENT_LOG_ERR, "Unable to open event base. Terminate.");
        printf("err");
       
       return EVENT_LOG_ERR;
    }

    // Отчищаем конфиг
    // event_config_free(cfg);

    // Открываем сокет
    memset( &sin, 0, sizeof(sin) );
    sin.sin_family = AF_INET;
    // sin.sin_addr.s_addr = htonl(0); // Принимать запросы только с 0.0.0.0
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Принимать запросы с любого адреса
    sin.sin_port = htons(port); // Слушаем на заданом порте

    // Создаем "Монитор соединений", сетим колбек для события обращения к сокету sin 
    listener = evconnlistener_new_bind( base, accept_connection_cb, NULL,
                                    (LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE),
                                    -1, (struct sockaddr *)&sin, sizeof(sin) );

    if (!listener) {
        _log(EVENT_LOG_ERR, "Unable to create eventlistener object");
        return -1;
    }

    evconnlistener_set_error_cb( listener, accept_error_cb );

    char* log_msg = (char*)malloc(100);
    sprintf(log_msg, "Libevent server started on port %d. Connection handle method is %s.", port, event_base_get_method(base));

    _log(EVENT_LOG_MSG, log_msg);

    event_base_dispatch( base );

    return 0;
}