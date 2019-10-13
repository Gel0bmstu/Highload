#pragma once 

#include <stdio.h>

#define EVENT_LOG_DBG 0
#define EVENT_LOG_MSG 1
#define EVENT_LOG_WRN 2
#define EVENT_LOG_ERR 3

void init_logger();
void _log(int messegeType, const char* messege);