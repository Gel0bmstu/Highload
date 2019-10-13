#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "../headers/logger.h"

#define CONSOLE_LOG
// #define FILE_LOG

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GRN "\x1b[32m"
#define ANSI_COLOR_YLL "\x1b[33m"

#define ANSI_COLOR_WHT "\x1b[0m"

FILE* logFile;

// Открываем лог файл, пишем в него данные текущей сессии 
void init_logger() {
    logFile = fopen("../log.dat", "a");

    // Получаем текущую дату-время
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Пишем дату-время в логфайл
	fprintf(logFile, "\n\n-------------------- %d-%d-%d %d:%d:%d ---------------------\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// Логирование в файл 
void _log(int messegeType, const char* messege) {
    if (!logFile) {
        fprintf(stderr, "Log file open error \n");
    } else {

        char *s, *c;
        FILE* stream;

        switch (messegeType)
        {
        case EVENT_LOG_DBG:
            s = "DEB";
            c = ANSI_COLOR_WHT;
            stream = stdout;
            break;
        case EVENT_LOG_MSG:
            s = "INF";
            c = ANSI_COLOR_GRN;
            stream = stdout;
            break;
        case EVENT_LOG_WRN:
            s = "WRN";
            c = ANSI_COLOR_YLL;
            stream = stdout;            
            break;
        case EVENT_LOG_ERR:
            s = "ERR";
            c = ANSI_COLOR_RED;
            stream = stderr;
            break;        
        default:
            break;
        }

        // Получаем текущую дату-время
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        #ifdef CONSOLE_LOG
            fprintf(stream, c);
            fprintf(stream, "%d:%d:%d: [%s] %s\n", tm.tm_hour, tm.tm_min, tm.tm_sec, s, messege);
            fprintf(stream, ANSI_COLOR_WHT);
        #endif

        #ifdef FILE_LOG
            fprintf(logFile, "%d:%d:%d: [%s] %s\n", tm.tm_hour, tm.tm_min, tm.tm_sec, s, messege);
        #endif
    }
}