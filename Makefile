CC= gcc
CC_INCLUDES= -Iinclude

all: build run

build:
	$(CC) src/main.c -o bin/main $(CC_INCLUDES)

run:
	./bin/main $(ARGS)