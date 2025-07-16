/*
 * indexer.c - CS50 TSE Indexer
 *
 * usage: ./indexer pageDirectory indexFilename
 *
 * Reads a file named pageDirectory/1...N where N is the highest number in the
 *   directory. builds inverted index and writes to indexFilename
 *
 * Author: Jacob Bacus
 * Feburary 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../common/pagedir.h"
#include "../common/word.h"
#include "../common/index.h"
#include "../libcs50/webpage.h"

// function prototypes
static void parseArgs(int argc, char* argv[],
                      char** pageDirectory, char** indexFilename);
static void buildIndex(const char* pageDirectory, index_t* index);
static void indexPage(webpage_t* page, int docID, index_t* index);

int main(int argc, char* argv[])
{
  char* pageDirectory = NULL;
  char* indexFilename = NULL;
  parseArgs(argc, argv, &pageDirectory, &indexFilename);
  
  // empty index
  index_t* index = index_new(500); // 500 is slot #
  if (index == NULL) {
    fprintf(stderr, "indexer: cannot create index\n");
    exit(1);
  }

  // outsource functionality to index struct
  buildIndex(pageDirectory, index);

  index_save(index, indexFilename);

  index_delete(index);
  return 0;
}

// go through and check that arguments are valid
static void parseArgs(int argc, char* argv[],
                      char** pageDirectory, char** indexFilename)
{
  if (argc !=3) {
    fprintf(stderr, "Usage: %s pageDirectory indexFilename\n", argv[0]);
    exit(2);
  }

  *pageDirectory = argv[1];
  *indexFilename = argv[2];

  // validate address
  if (!pagedir_validate(*pageDirectory)) {
    fprintf(stderr, "Invalid pageDirectory '%s'\n", *pageDirectory);
    exit(3);
  }

  // check if you can write into indexFilename
  FILE* fp = fopen(*indexFilename, "w");
  if (fp == NULL) {
    fprintf(stderr, "Cannot open indexFilename '%s'\n", *indexFilename);
    exit(4);
  }
  fclose(fp);
}

// create indicies starting from docID 1 and incrementing
static void buildIndex(const char* pageDirectory, index_t* index)
{
  int docID = 1;
  while (true) {
    // load a page
    webpage_t* page = pagedir_load(pageDirectory, docID);

    // ran out of pages
    if (page == NULL) {
      break;
    }

    // index page
    indexPage(page, docID, index);

    // cleanup
    webpage_delete(page);
    docID++;
  }
}

// gets words from webpageand puts them into the index after normalizing them
static void indexPage(webpage_t* page, int docID, index_t* index)
{
  int pos = 0; // tracksposition
  char* word;
  while ((word = webpage_getNextWord(page, &pos)) != NULL) {
    if (normalizeWord(word)) {
      // insert word into the index
      index_insert(index, word, docID);
    }
    free(word); // allocated into webpage_getNextWord
  }
}
