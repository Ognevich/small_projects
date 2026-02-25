CC = gcc
CFLAGS = -Wall
LDFLAGS = -lssl -lcrypto

# ---- df ----
OBJ = duplicate_finder.o 

# ---- my_arc ----
ARC_OBJ = my_arc.o

all: df my_arc

# ===== df =====
df: $(OBJ)
	$(CC) $(OBJ) -o df $(LDFLAGS)

duplicate_finder.o: duplicate_finder.c
	$(CC) $(CFLAGS) -c duplicate_finder.c

# ===== my_arc =====
my_arc: $(ARC_OBJ)
	$(CC) $(ARC_OBJ) -o my_arc

my_arc.o: my_arc.c
	$(CC) $(CFLAGS) -c my_arc.c

# ===== clean =====
clean:
	rm -f *.o df my_arc

.PHONY: all clean
