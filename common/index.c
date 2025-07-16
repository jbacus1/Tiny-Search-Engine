/* index.c - CS50 TSE index module
 *
 * The index is a hashtable that maps words to counters.
 *   the counters maps docID to count of word occurrence.
 *
 * Full and extensive documentation is in index.h
 *
 * Author: Jacob Bacus
 * Feburary 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "index.h"

// defines an index
typedef struct index {
  hashtable_t* table; // word to counters
} index_t;


// function prototypes
static void index_delete_item(void* item);
static void index_save_helper(void* fp, const char* key, void* item);
static void counters_save_helper(void* fp, const int key, const int count);
static void index_load_insert(index_t* idx, const char* word, FILE* fp);

// create a new index
index_t* index_new(const int slots)
{
  index_t* idx = malloc(sizeof(index_t));

  // malloc fails
  if (idx == NULL) {
    return NULL;
  }

  // creatae the hashtable for index
  idx->table = hashtable_new(slots);
  if (idx->table == NULL) {
    free(idx);
    return NULL;
  }
  return idx;
}

// place a word into the index, incrementing the respective counter
bool index_insert(index_t* idx, const char* word, const int docID)
{
  // bad parameters
  if (idx == NULL || word == NULL || docID <= 0) {
    return false;
  }

  // check if word is in hashtable already
  counters_t* ctrs = hashtable_find(idx->table, word);
  if (ctrs == NULL) {
    // word not found so make a counters
    counters_t* newCtrs = counters_new();
    if (newCtrs == NULL) {
      return false; // failed to create
    }
    
    // insert into hashtable
    if (!hashtable_insert(idx->table, word, newCtrs)) {
      counters_delete(newCtrs);
      return false;
    }

    //increment docID value
    counters_add(newCtrs, docID);
  } else {
    // word was found
    counters_add(ctrs, docID);
  }
  return true;
}

// checks if a word is in the index
counters_t* index_find(index_t* idx, const char* word)
{
  if (idx == NULL || word == NULL) {
    return NULL; // bad parameters
  }
  return hashtable_find(idx->table, word);
}

// saves index to a file
void index_save(index_t* idx, const char* filename)
{
  if (idx == NULL || filename == NULL) {
    return; // bad arguments
  }

  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "index_save: cannot open file '%s'\n", filename);
    return;
  }

  // iterate through hashtable for printing
  hashtable_iterate(idx->table, fp, index_save_helper);
  fclose(fp);
}

/*
 * Called by hashtable_iterate in the index_save function
 *   prints the word and iterates through counters.
 */
static void index_save_helper(void* arg, const char* key, void* item)
{
  FILE* fp = arg;
  counters_t* ctrs = item;
  if (fp == NULL || key == NULL || ctrs == NULL) {
    return; // bad args
  }

  // print word
  fprintf(fp, "%s", key);

  // call docID-pair printer
  counters_iterate(ctrs, fp, counters_save_helper);

  // end line
  fprintf(fp, "\n");
}

/*
 * Called by index_save_helper to go through counters in index
 *   prints docID-count pairs to show how many times words show up in files
 */
static void counters_save_helper(void* arg, const int key, const int count)
{
  FILE* fp = arg;
  if (fp == NULL) {
    return;
  }
  fprintf(fp, " %d %d", key, count);
}

// Generates an index from a properly formatted file
index_t* index_load(const char* filename)
{
  if (filename == NULL) {
    return NULL;
  }
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL;
  }

  // Do not know number of words in file, but can assume 500 is sufficient as it is
  //   default set in indexer
  index_t* idx = index_new(500);
  if (idx == NULL) {
    fclose(fp);
    return NULL; // failed to intialize
  }

  // read one word, followed by pairs of docID and count

  
  char word[200]; // max word size of 200 chars
  while (fscanf(fp, "%s", word) == 1) {
    index_load_insert(idx, word, fp);
  }

  fclose(fp);
  return idx;
}

/*
 * Helper function called by index_load 
 *   Reads the the pairs of docID and count
 */
static void index_load_insert(index_t* idx, const char* word, FILE* fp)
{
  while (true) {
    int docID, count;
    int nread = fscanf(fp, "%d %d", &docID, &count);
    if (nread != 2) {
      break; // line is out of words
    }

    counters_t* ctrs = hashtable_find(idx->table, word);
    if (ctrs == NULL) {
      // new counters for this word
      ctrs = counters_new();
      hashtable_insert(idx->table, word, ctrs);
    }

    for (int i = 0; i < count; i++) {
      counters_add(ctrs, docID);
    }
  }
}

// deletes the index
void index_delete(index_t* idx)
{
  if (idx == NULL) {
    return;
  }
  hashtable_delete(idx->table, index_delete_item);
  free(idx);
}

/*
 * Helper function called by index_delete that removes the counters in index
 */
static void index_delete_item(void* item)
{
  if (item == NULL) {
    return;
  }
  // item is a counters
  counters_delete(item);
}
