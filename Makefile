# Simple Makefile
# 
# Copyright (c)2023 Malcolm Harrow

CC=gcc
RM=rm -f

BINARY=target/view
OBJ=obj/view.o
SRC=src/view.c

all: clean $(BINARY)

$(BINARY): $(SRC)
	$(CC) -o $@ $<
	./$(BINARY)

.PHONY: all clean run

all: $(BINARY)
	
clean: 
	$(RM) $(OBJ) $(BINARY)