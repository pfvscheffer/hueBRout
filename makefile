CC=gcc
CFLAGS=-Wall -lncurses -I.
OBJ = main.o aux.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

breakout: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
