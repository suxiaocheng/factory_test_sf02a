# CC:=arm-linux-gnueabi-gcc
CC:=gcc
CFLAGS:=-g -c
LDFLAGS:=-static

SRC := main.c factory_test.c
OBJ := $(patsubst %.c,%.o,${SRC})

all:factory_test

#include depend

factory_test: main.o factory_test.o
	$(CC) $(LDFLAGS) -o $@ $^

depend:
	${CC} -MM ${SRC} > depend

.PHONY: clean
clean:
	rm *.o ${OBJ} depend -rf
