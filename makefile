CC=gcc
CFLAGS=-Wall -lncurses
OBJ = main.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

breakout: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
