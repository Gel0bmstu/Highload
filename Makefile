all: server

server:
	gcc -std=c11 -D_GNU_SOURCE -Wall -Wextra -Werror -Iinclude -levent -levent_pthreads -lpthread \
		src/main.c
		src/serve.c 
		
		-o bin/highload
