.PHONY: default
default: build

include makefiles/local.make
include makefiles/hrg.make
include makefiles/misc.make

build/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c -o $@ $^

CC = g++
CPPFLAGS = -std=c++17 -Wall -Werror -O3 -I include/
TESTFLAGS = $(CPPFLAGS) -I /usr/include -I /usr/src/gtest -pthread -DMAKE_EVERYTHING_PUBLIC

.PHONY: build
build: localvc hrg misc localvcextra

.PHONY: all
all: build

.PHONY: clean
clean:
	rm -rf build/*
	rm -rf bin/*

.PHONY: rebuild
rebuild: clean build