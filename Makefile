CC=gcc
CFLAGS=-Wall -Werror

slides:
	$(CC) -o slides slides.c slide-input.c
