CFLAGS = -Wall -std=c11
LIBS = 

.PHONY: all
all: lim

lim: src/main.c src/lim.h
	$(CC) $(CFLAGS) -o lim src/main.c $(LIBS)