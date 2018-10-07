SRC = src/*.c
FLAGS = -Wall
OUT = bin/sink
LANG_STANDARD = c99
CC = clang

all: build run

build:
	${CC} -o ${OUT} ${FLAGS} -std=${LANG_STANDARD} ${SRC}

run:
	./bin/sink

clean:
	rm -f bin/*

.PHONY: all build run clean
