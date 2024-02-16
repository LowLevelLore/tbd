CC = gcc
CFLAGS = -Wall -Wextra -O3
LDFLAGS = -lstdc++

all: main.o
	@$(CC) $(CFLAGS) build/main.o -o build/tbd
	@rm -rf main.o

main.o: src/main.c
	@mkdir -p build
	@$(CC) $(CFLAGS) -c src/main.c
	@mv main.o build/main.o