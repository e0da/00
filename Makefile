BIN=00
CC:=clang
LIBS:=-lSDL2 -lSDL2_image
CFLAGS:= -std=c11 -O3 -Weverything -pedantic -ggdb

EMSDK_ENV:=${HOME}/src/emsdk/emsdk_env.sh

EMCC_FLAGS:=--preload-file assets \
						--use-preload-plugins \
						-O3 \
						-g4 \
						-s USE_SDL=2 \
						-s USE_SDL_IMAGE=2 \
						-s WASM=1 \
						-s SDL2_IMAGE_FORMATS='["png"]'

BUILD_ARTIFACTS:=$(BIN)
BUILD_WEB_ARTIFACTS:=index.html index.js index.wasm index.data
BUILD_WEB_JUNK_ARTIFACTS:=index.wasm.map index.wast
BUILD_WEB_CLEAN_ARTIFACTS:=$(BUILD_WEB_ARTIFACTS) $(BUILD_WEB_JUNK_ARTIFACTS) *.o
CLEAN_ARTIFACTS:=$(BIN) index.* *.o

BUILD_WEB_TEMPDIR:=$(shell tempfile)
BUILD_WEB_PAYLOAD:=$(BUILD_WEB_ARTIFACTS) LICENSE COPYING README.md

BIN_OBJS=$(BIN).o Bug.o

all: $(BIN)

$(BIN): $(BIN_OBJS)
	$(CC) -o $@ $(BIN_OBJS) $(CFLAGS) $(LIBS)

$(BIN).o: main.c Bug.h logging.h
	$(CC) -c -o $@ main.c $(CFLAGS)

Bug.o: Bug.h Bug.c logging.h
	$(CC) -c -o $@ Bug.c $(CFLAGS)

.PHONY: run
run: $(BIN)
	./$(BIN)

.PHONY: web
web: index.html

index.html: $(EMSDK_ENV) main.c Bug.c Bug.h logging.h
	bash -c 'source $(EMSDK_ENV); emcc -o $@ main.c Bug.c $(EMCC_FLAGS)'

.PHONY: serve
serve: web
	# If http-server fails, install it with `npm i -g http-server`
	http-server

$(EMSDK_ENV):
	@echo "$(EMSDK_ENV) not found. Install emsdk to $(shell dirname $(EMSDK_ENV))"
	false

.PHONY: clean
clean:
	rm -f $(CLEAN_ARTIFACTS)

.PHONY: web-build
web-build: clean web
	git diff --quiet
	git diff --cached --quiet
	rm $(BUILD_WEB_TEMPDIR)
	mkdir $(BUILD_WEB_TEMPDIR)
	cp -R $(BUILD_WEB_ARTIFACTS) $(BUILD_WEB_TEMPDIR)
	git checkout gh-pages
	rm -f $(BUILD_WEB_CLEAN_ARTIFACTS)
	echo "$(BUILD_WEB_ARTIFACTS)" | tr ' ' '\n' | xargs -I% cp -R $(BUILD_WEB_TEMPDIR)/% ./
	git add --force $(BUILD_WEB_PAYLOAD)
	git log -n1
