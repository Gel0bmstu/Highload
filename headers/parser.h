#pragma once

#include <string.h>

#include "../src/parser.c"

char* parse_req(char* s, struct response* r);