BIN:=00
HTML:=index.html
MAIN_C:=$(BIN).c
CC:=clang
STD:=-std=c11
LIBS:=-lSDL2 -lSDL2_image -lm
OPTIMIZE:=-O3
LINT:=-Weverything -pedantic
DEBUG:=-ggdb
CFLAGS:=$(DEBUG) $(LINT) $(OPTIMIZE) $(LIBS) $(STD)

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
BUILD_WEB_ARTIFACTS:=$(HTML) index.js index.wasm index.data
BUILD_WEB_JUNK_ARTIFACTS:=index.wasm.map index.wast
BUILD_WEB_CLEAN_ARTIFACTS:=$(BUILD_WEB_ARTIFACTS) $(BUILD_WEB_JUNK_ARTIFACTS)
CLEAN_ARTIFACTS:=$(BUILD_ARTIFACTS) $(BUILD_WEB_CLEAN_ARTIFACTS)

BUILD_WEB_TEMPDIR:=$(shell tempfile)
BUILD_WEB_PAYLOAD:=$(BUILD_WEB_ARTIFACTS) LICENSE COPYING README.md

all: $(BIN)

$(BIN): $(MAIN_C)
	$(CC) -o $@ $(MAIN_C) $(CFLAGS)

.PHONY: run
run: $(BIN)
	./$(BIN)

.PHONY: web
web: $(HTML) $(MAIN_C)

$(HTML): $(MAIN_C) $(EMSDK_ENV)
	bash -c 'source $(EMSDK_ENV); emcc -o $@ $(MAIN_C) $(EMCC_FLAGS)'

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

.PHONY: build-web
build-web: web
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
