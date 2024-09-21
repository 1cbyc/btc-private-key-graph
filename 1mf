CC = gcc
CFLAGS = -Wall -Iinclude
OBJ = src/main.o src/keygen.o src/utils.o

btc_keygen: $(OBJ)
	$(CC) -o btc_keygen $(OBJ)

src/main.o: src/main.c include/keygen.h include/utils.h
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/keygen.o: src/keygen.c include/keygen.h
	$(CC) $(CFLAGS) -c src/keygen.c -o src/keygen.o

src/utils.o: src/utils.c include/utils.h
	$(CC) $(CFLAGS) -c src/utils.c -o src/utils.o

clean:
	rm -f src/*.o btc_keygen
