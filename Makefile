# CC := clang
CC := g++
# CFLAGS := -std=c++17 -g -Wall -Werror -Wextra -I include
CFLAGS := -std=c++17 -g -I include -I src


bin/ip_cmd_sampler.o: src/ip_cmd_sampler.cpp src/ip_cmd_sampler.h include/*
	@mkdir -p bin
	$(CC) $(CFLAGS) -c -o $@ $<

bin/main: src/main.cpp bin/ip_cmd_sampler.o
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^


all: bin/main
	bin/main

clean:
	rm -rf bin/*


.DEFAULT_GOAL := all
.PHONY: all clean
