CC = gcc
CFLAGS = -Wall -Wextra -O3
LDFLAGS = -lstdc++ -lc

all: build/main.o
	@$(CC) $(CFLAGS) build/main.o -o build/tbd
ifeq ($(OS),Windows_NT)
	@-del build/main.o
else
	@rm -rf build/main.o
endif

build/main.o: src/main.c
ifeq ($(OS),Windows_NT)
	@test -d build || mkdir build
	@$(CC) $(CFLAGS) -c src/main.c -o build/main.o
else
	@mkdir -p build
	@$(CC) $(CFLAGS) -c src/main.c
	@mv main.o build/main.o
endif
	