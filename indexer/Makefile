# Makefile for Indexer

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

PROGS = indexer indextest
OBJS_INDEXER = indexer.o
OBJS_INDEXTEST = indextest.o

LIBS = ../common/common.a ../libcs50/libcs50.a

all: $(PROGS)

indexer: $(OBJS_INDEXER) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS_INDEXER) $(LIBS) -o $@

indextest: $(OBJS_INDEXTEST) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS_INDEXTEST) $(LIBS) -o $@

indexer.o: indexer.c
	$(CC) $(CFLAGS) -c indexer.c

indextest.o: indextest.c
	$(CC) $(CFLAGS) -c indextest.c

clean:
	rm -f *~ *.o $(PROGS)

test: all
	bash -v testing.sh &> testing.out

.PHONY: all clean test
