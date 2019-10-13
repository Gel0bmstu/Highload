#pragma once    

#include "../src/serve.c"

#include <stdio.h>

static void echo_read_cb( struct bufferevent *buf_ev, void *arg );
static void echo_event_cb( struct bufferevent *buf_ev, short events, void *arg );
static void accept_connection_cb( struct evconnlistener *listener,
                   evutil_socket_t fd, struct sockaddr *addr, int sock_len,
                   void *arg );
static void accept_error_cb( struct evconnlistener *listener, void *arg );

int listen_and_serve(uint16_t port);
void build_default_err_response(struct response *r);
