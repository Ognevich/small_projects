CC = gcc
CFLAGS = -Wall

OBJ = duplicate_finder.o 

all: df

df: $(OBJ)
	$(CC) $(OBJ) -o df

duplicate_finder.o: duplicate_finder.c
	$(CC) $(CFLAGS) -c duplicate_finder.c 

clean:
	rm -f *.o df

.PHONY: all clean
