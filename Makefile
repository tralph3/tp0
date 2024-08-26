CC:=gcc
INCLUDE:=-I./so-commons-library/src/commons -L./so-commons-library/src/build
CFLAGS:=-ffast-math -Wall -Wextra -pedantic -lcommons -lm $(INCLUDE)

.PHONY: prepare commons

all: prepare client server

client:
	make -C ./client all

server:
	make -C ./server all

commons:
	make -C ./so-commons-library all

prepare: commons
	mkdir -p build

%.o: %.c
	$(CC) $(CFLAGS) -c $^

clean:
	make -C ./client clean
	make -C ./server clean
	make -C ./so-commons-library clean
	rm -rf ./build
