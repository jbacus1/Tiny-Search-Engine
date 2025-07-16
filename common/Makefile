# Makefile for common

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

OBJS = pagedir.o word.o index.o
LIB = common.a

all: $(LIB)

# Build library
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)
	ranlib $(LIB)

# Builds from libsc50
pagedir.o: pagedir.c pagedir.h ../libcs50/webpage.h
	$(CC) $(CFLAGS) -c pagedir.c

word.o: word.c word.h
	$(CC) $(CFLAGS) -c word.c

index.o: index.c index.h ../libcs50/hashtable.h ../libcs50/counters.h
	$(CC) $(CFLAGS) -c index.c

# Removed extra files
clean:
	rm -f *~ *.o $(LIB)
