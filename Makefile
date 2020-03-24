tcpsevr: error_wrap.o socket_wrap.o condition.o thread_pool.o epoll_wrap.o logger.o min_heap.o timer.o tasks.o main.o 
	gcc error_wrap.o socket_wrap.o condition.o thread_pool.o epoll_wrap.o logger.o min_heap.o timer.o tasks.o main.o -o tcpsevr -lpthread

error_wrap.o: error_wrap.c error_wrap.h
	gcc -c error_wrap.c -o error_wrap.o

socket_wrap.o: socket_wrap.c socket_wrap.h
	gcc -c socket_wrap.c -o socket_wrap.o

condition.o: condition.c condition.h
	gcc -c condition.c -o condition.o -lpthread

thread_pool.o: thread_pool.c thread_pool.h
	gcc -c thread_pool.c -o thread_pool.o -lpthread

epoll_wrap.o: epoll_wrap.c epoll_wrap.h
	gcc -c epoll_wrap.c -o epoll_wrap.o

logger.o: logger.c logger.h
	gcc -c logger.c -o logger.o

min_heap.o: min_heap.c min_heap.h
	gcc -c min_heap.c -o min_heap.o

timer.o: timer.c timer.h
	gcc -c timer.c -o timer.o

tasks.o: tasks.c tasks.h
	gcc -c tasks.c -o tasks.o

main.o: main.c main.h
	gcc -c main.c -o main.o

clean:
	rm -rf *.o main
