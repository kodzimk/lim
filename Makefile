CFLAGS = -Wall 
LIBS = 

.PHONY: all
all: lim

lim: src/main.c src/lim.h
	$(CC) $(CFLAGS) -o lim src/main.c -lm $(LIBS)