#include "../headers/serve.h"
#include "../headers/logger.h"


int main(int argc, char **argv) {
    init_logger(); 
    _log(EVENT_LOG_MSG, "Logger inited");
    _log(EVENT_LOG_MSG, "Server started");

    int port = 8080;

    if( argc > 1 )  port = atoi( argv[1] );
    if( port < 0 || port > 65535 )
    {
        _log(EVENT_LOG_ERR, "Wrong port number");
        return -1;
    }

    return listen_and_serve( (uint16_t)port );
}