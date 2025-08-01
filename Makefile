CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lssl -lcrypto -lm
OBJ = src/main.o src/keygen.o src/crypto.o src/address.o src/utils.o

TARGET = btc_keygen
VERSION = 2.0.0

.PHONY: all clean install test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

src/main.o: src/main.c include/keygen.h include/crypto.h include/utils.h
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/keygen.o: src/keygen.c include/keygen.h include/crypto.h include/address.h include/utils.h
	$(CC) $(CFLAGS) -c src/keygen.c -o src/keygen.o

src/crypto.o: src/crypto.c include/crypto.h include/utils.h
	$(CC) $(CFLAGS) -c src/crypto.c -o src/crypto.o

src/address.o: src/address.c include/address.h include/utils.h
	$(CC) $(CFLAGS) -c src/address.c -o src/address.o

src/utils.o: src/utils.c include/utils.h
	$(CC) $(CFLAGS) -c src/utils.c -o src/utils.o

clean:
	rm -f src/*.o $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

test: $(TARGET)
	./$(TARGET) --version
	./$(TARGET) -c 5 -v
	./$(TARGET) -f wif -a
	./$(TARGET) -p -a

dist: clean
	mkdir -p $(TARGET)-$(VERSION)
	cp -r src include docs Makefile README.md LICENSE .gitignore $(TARGET)-$(VERSION)/
	tar -czf $(TARGET)-$(VERSION).tar.gz $(TARGET)-$(VERSION)/
	rm -rf $(TARGET)-$(VERSION)

check-deps:
	@echo "Checking dependencies..."
	@which gcc > /dev/null || (echo "gcc not found" && exit 1)
	@pkg-config --exists openssl || (echo "OpenSSL development libraries not found" && exit 1)
	@echo "All dependencies found"

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

release: CFLAGS += -DNDEBUG
release: $(TARGET)
