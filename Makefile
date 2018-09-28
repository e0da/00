BIN=00
CC=clang
CFLAGS=-g -Weverything -O3

$(BIN): $(BIN).c
	$(CC) -o $@ $^ $(CFLAGS)

run: $(BIN)
	./$(BIN)

clean:
	rm $(BIN)
