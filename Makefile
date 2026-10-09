CC= gcc
CC_FLAGS= -Wall -Wextra
CC_INCLUDES= -Iinclude

RAYLIB_VERSION= 5.5
RAYLIB_DIR= thirdparty/raylib
RAYLIB_LIB= $(RAYLIB_DIR)/src/libraylib.a
RAYLIB_FLAGS= -I$(RAYLIB_DIR)/src $(RAYLIB_LIB) -lGL -lm -lpthread -ldl -lrt -lX11

FONTS_DIR= thirdparty/fonts
FONTS_CDN= https://cdn.jsdelivr.net/fontsource/fonts
FONTS= $(FONTS_DIR)/Roboto-Regular.ttf $(FONTS_DIR)/Roboto-Medium.ttf $(FONTS_DIR)/RobotoMono-Regular.ttf $(FONTS_DIR)/MaterialIcons-Regular.ttf

SOURCES= src/main.c $(wildcard src/tbf-vm/*.c) $(wildcard src/tbf-math/*.c) $(wildcard src/tbf-gui/*.c)

all: build run

build: $(RAYLIB_LIB) $(FONTS)
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

$(FONTS_DIR)/Roboto-Regular.ttf:
	curl -sSfL --create-dirs -o $@ $(FONTS_CDN)/roboto@latest/latin-400-normal.ttf

$(FONTS_DIR)/Roboto-Medium.ttf:
	curl -sSfL --create-dirs -o $@ $(FONTS_CDN)/roboto@latest/latin-500-normal.ttf

$(FONTS_DIR)/RobotoMono-Regular.ttf:
	curl -sSfL --create-dirs -o $@ $(FONTS_CDN)/roboto-mono@latest/latin-400-normal.ttf

$(FONTS_DIR)/MaterialIcons-Regular.ttf:
	curl -sSfL --create-dirs -o $@ https://github.com/google/material-design-icons/raw/master/font/MaterialIcons-Regular.ttf

.PHONY: all build run run-gui clean
