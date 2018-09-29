BIN=00
CC=clang
STD=-std=c11
LIBS=-lSDL2
OPTIMIZE=-O3
LINT=-Weverything -pedantic
DEBUG=-ggdb
CFLAGS=$(DEBUG) $(LINT) $(OPTIMIZE) $(LIBS) $(STD)

$(BIN): $(BIN).c
	$(CC) -o $@ $^ $(CFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm $(BIN)
