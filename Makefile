BIN=00
CC=clang
STD=-std=c11
LIBS=-lSDL2 -lSDL2_image -lm
OPTIMIZE=-O3
LINT=-Weverything -pedantic
DEBUG=-ggdb
CFLAGS=$(DEBUG) $(LINT) $(OPTIMIZE) $(LIBS) $(STD)
ARTIFACTS=00 00.html 00.js 00.wasm
EMSDK_ENV=${HOME}/src/emsdk/emsdk_env.sh

EMCC_FLAGS=--preload-file assets \
						--use-preload-plugins \
						-s USE_SDL=2 \
						-s USE_SDL_IMAGE=2 \
						-s SDL2_IMAGE_FORMATS='["png"]'

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
