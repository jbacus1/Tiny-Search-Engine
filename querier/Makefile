# Makefile for Querier
#
# Author: Jacob Bacus
# Date: February 2025

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

PROG = querier
OBJS = querier.o
LIBS = ../common/common.a ../libcs50/libcs50.a

all: $(PROG)

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

temp.o: querier.c
	$(CC) $(CFLAGS) -c querier.c

clean:
	rm -f *~ *.o $(PROG)

test: all
	bash -v testing.sh &> testing.out

.PHONY: all clean test
