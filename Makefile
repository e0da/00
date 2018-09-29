BIN=00
CC=clang
STD=-std=c11
LIBS=-lSDL2
OPTIMIZE=-O3
LINT=-Weverything -pedantic
DEBUG=-ggdb
CFLAGS=$(DEBUG) $(LINT) $(OPTIMIZE) $(LIBS) $(STD)
ARTIFACTS=00 00.html 00.js 00.wasm
EMSDK_ENV=${HOME}/src/emsdk/emsdk_env.sh

$(BIN): $(BIN).c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: run
run: $(BIN)
	./$(BIN)

.PHONY: web
web: 00.html

00.html: $(BIN).c $(EMSDK_ENV)
	bash -c 'source $(EMSDK_ENV); emcc $(BIN).c -s USE_SDL=2 -o $@'

$(EMSDK_ENV):
	@echo "emsdk_env.sh not found. Install emsdk to $(shell dirname $(EMSDK_ENV))"
	false

.PHONY: clean
clean:
	rm -f $(ARTIFACTS)
