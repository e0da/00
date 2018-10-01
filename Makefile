BIN:=00
CC:=clang
STD:=-std=c11
LIBS:=-lSDL2 -lSDL2_image -lm
OPTIMIZE:=-O3
LINT:=-Weverything -pedantic
DEBUG:=-ggdb
CFLAGS:=$(DEBUG) $(LINT) $(OPTIMIZE) $(LIBS) $(STD)
ARTIFACTS:=00 00.html 00.js 00.wasm
EMSDK_ENV:=${HOME}/src/emsdk/emsdk_env.sh

EMCC_FLAGS:=--preload-file assets \
						--use-preload-plugins \
						-s USE_SDL=2 \
						-s USE_SDL_IMAGE=2 \
						-s SDL2_IMAGE_FORMATS='["png"]'

BUILD_WEB_TEMPDIR:=$(shell tempfile)
BUILD_WEB_ARTIFACTS:=index.html 00.data 00.js 00.wasm LICENSE COPYING README.md

$(BIN): $(BIN).c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: run
run: $(BIN)
	./$(BIN)

.PHONY: web
web: 00.html

00.html: $(BIN).c $(EMSDK_ENV)
	bash -c 'source $(EMSDK_ENV); emcc $(BIN).c $(EMCC_FLAGS) -o $@'

$(EMSDK_ENV):
	@echo "$(EMSDK_ENV) not found. Install emsdk to $(shell dirname $(EMSDK_ENV))"
	false

.PHONY: clean
clean:
	rm -f $(ARTIFACTS)

.PHONY: build-web
build-web: web
	git diff --quiet
	git diff --cached --quiet
	rm $(BUILD_WEB_TEMPDIR)
	mkdir $(BUILD_WEB_TEMPDIR)
	mv 00.html index.html
	cp -R $(BUILD_WEB_ARTIFACTS) $(BUILD_WEB_TEMPDIR)
	rm index.html
	git checkout gh-pages
	rm $(BUILD_WEB_ARTIFACTS)
	echo "$(BUILD_WEB_ARTIFACTS)" | tr ' ' '\n' | xargs -I% cp -R $(BUILD_WEB_TEMPDIR)/% ./
	git add --force index.html 00.data 00.js 00.wasm
	git log -n1
