CC=gcc
CFLAGS=-Wall -Werror

slides: slides.c slide-input.c slide-input.h
	$(CC) -o slides slides.c slide-input.c
