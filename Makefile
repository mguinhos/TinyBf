CC= gcc
CC_INCLUDES= -Iinclude

RAYLIB_VERSION= 5.5
RAYLIB_DIR= thirdparty/raylib
RAYLIB_LIB= $(RAYLIB_DIR)/src/libraylib.a
GUI_SOURCES= src/tinybf.c $(wildcard src/gui/*.c)

RAYLIB_FLAGS= -I$(RAYLIB_DIR)/src $(RAYLIB_LIB) -lGL -lm -lpthread -ldl -lrt -lX11

all: build run

build:
	$(CC) src/main.c -o bin/main $(CC_INCLUDES)

run:
	./bin/main $(ARGS)

gui: $(RAYLIB_LIB)
	$(CC) $(GUI_SOURCES) -o bin/gui $(CC_INCLUDES) $(RAYLIB_FLAGS)

run-gui:
	./bin/gui $(ARGS)

$(RAYLIB_LIB):
	test -d $(RAYLIB_DIR) || git clone --depth 1 --branch $(RAYLIB_VERSION) https://github.com/raysan5/raylib.git $(RAYLIB_DIR)
	$(MAKE) -C $(RAYLIB_DIR)/src PLATFORM=PLATFORM_DESKTOP
