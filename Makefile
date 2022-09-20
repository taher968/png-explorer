CFLAGS?=-std=c99 -D_GNU_SOURCE -Wall -O2 -g 
CC=gcc

default: all

all: catpng findpng pnginfo

catpng: catpng.c zutil.c crc.c
	gcc -o catpng catpng.c zutil.c crc.c -lz -g

findpng: findpng.c
	$(CC) $(CFLAGS) -o findpng findpng.c

pnginfo: pnginfo.c
	$(CC) $(CFLAGS) -o pnginfo pnginfo.c crc.c

clean: 
	rm catpng findpng pnginfo all.png

