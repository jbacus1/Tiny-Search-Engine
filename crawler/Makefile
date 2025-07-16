# Makefile for Crawler

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

PROG = crawler
OBJS = crawler.o
LIBS = ../common/common.a ../libcs50/libcs50.a

all: $(PROG)

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

crawler.o: crawler.c
	$(CC) $(CFLAGS) -c crawler.c

# Removes excess files
clean:
	rm -f *~ *.o $(PROG)

# Runs the test files
test: $(PROG)
	./testing.sh
