SRC = src/*.c
FLAGS = -Wall
OUT = bin/sink
LANG_STANDARD = c99
CC = clang
LDFLAGS = -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib

all: build run

build:
	${CC} -o ${OUT} ${FLAGS} ${LDFLAGS} -std=${LANG_STANDARD} ${SRC}

run:
	./bin/sink

clean:
	rm -f bin/*

.PHONY: all build run clean
