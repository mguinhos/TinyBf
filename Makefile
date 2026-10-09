CC= gcc
CC_FLAGS= -Wall -Wextra
CC_INCLUDES= -Iinclude

RAYLIB_VERSION= 5.5
RAYLIB_DIR= thirdparty/raylib
RAYLIB_LIB= $(RAYLIB_DIR)/src/libraylib.a
RAYLIB_FLAGS= -I$(RAYLIB_DIR)/src $(RAYLIB_LIB) -lGL -lm -lpthread -ldl -lrt -lX11

SOURCES= src/main.c $(wildcard src/tbf-vm/*.c) $(wildcard src/tbf-gui/*.c)

all: build run

build: $(RAYLIB_LIB)
	$(CC) $(CC_FLAGS) $(SOURCES) -o bin/tbf $(CC_INCLUDES) $(RAYLIB_FLAGS)

run:
	./bin/tbf $(ARGS)

run-gui:
	./bin/tbf --gui $(ARGS)

clean:
	rm -f bin/tbf

$(RAYLIB_LIB):
	test -d $(RAYLIB_DIR) || git clone --depth 1 --branch $(RAYLIB_VERSION) https://github.com/raysan5/raylib.git $(RAYLIB_DIR)
	$(MAKE) -C $(RAYLIB_DIR)/src PLATFORM=PLATFORM_DESKTOP

.PHONY: all build run run-gui clean
