all: basic threadedServer

basic:
	g++ -o basic thread.c -lpthread

threadedServer:
	g++ -o server threadedserver.c -lpthread -std=c++0x
clean:
	rm basic server
