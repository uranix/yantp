CC=gcc
CFLAGS=-Wall -Werror -g -O0
LIBS=-lm
OBJS=yantpserver.o yantpclient.o packet.o test.o

.PHONY: all clean

all: yantpserver yantpclient

clean:
	${RM} yantpserver yantpclient test ${OBJS}

yantpserver: packet.o yantpserver.o
	${CC} -o $@ $^ ${LIBS}

yantpclient: packet.o yantpclient.o
	${CC} -o $@ $^ ${LIBS}
