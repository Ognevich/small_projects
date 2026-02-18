CC = gcc
CFLAGS = -Wall
LDFLAGS = -lssl -lcrypto

OBJ = duplicate_finder.o 

all: df

df: $(OBJ)
	$(CC) $(OBJ) -o df $(LDFLAGS)

duplicate_finder.o: duplicate_finder.c
	$(CC) $(CFLAGS) -c duplicate_finder.c 

clean:
	rm -f *.o df

.PHONY: all clean
