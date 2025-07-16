/* querier.c - The Querier module of TSE
 *
 * Usage: ./querier pageDirectory indexFilename
 *
 * This program reads an index file produced by indexer and a pageDirectory produced
 *   by crawler that corresponds to it. After this it accepts queries from stdin.
 *   The queries are words and 'and'/'or' operators ('and' takes precedence), querier
 *   returns set of matching documents.
 *
 * (Ranking not implemented)
 *
 * Author: Jacob Bacus
 * Feburary 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h> // for isatty
#include "../common/index.h"
#include "../common/pagedir.h"
#include "../common/word.h"
#include "../libcs50/counters.h"
#include "../libcs50/mem.h"

// local constants used for max lengths
#define MAX_QUERY_LINE 1000
#define MAX_QUERY_WORDS 200

// store docID with score for sorting
typedef struct docscore {
  int docID;
  int score;
} docscore_t;

// function prototypes
static void parseArgs(int argc, char*argv[], char** pageDir, char** indexFilename);
static void prompt(void);
static bool readQueryLine(char* buffer, FILE* fp);
static char** parseQuery(char* line, int* nwords);
static bool validateQuery(char** words, int nwords);
static counters_t* handleQuery(char** words, int nwords, index_t* index);
static counters_t* parseOrSequence(char** words, int* pos,
                                   int nwords, index_t* index);
static counters_t* parseAndSequence(char** words, int* pos,
                                    int nwords, index_t* index);
static counters_t* getCountersForWord(const char* word, index_t* index);
static void countersAndCombine(counters_t* dest, counters_t* src);
static void countersOrCombine(counters_t* dest, counters_t* src);
static void printResults(counters_t* results, const char* pageDir);

// counters_iterate functions
static void foundNonZeroFn(void* arg, const int key, const int count);
static void copyCountFn(void* arg, const int key, const int count);
static void andCombineHelper1(void* arg, const int key, const int countDest);
static void andCombineHelper2(void* arg, const int key, const int countSrc);
static void orCombineHelper(void* arg, const int key, const int countSrc);

// helper functions for sorting docscore, counting docs
static int countNonZeroDocs(counters_t* ctrs);
static void addIfNonZero(void* arg, const int docID, const int score);
static int compareDocscore(const void* a, const void* b);

// struct for counters_iterate that fills docscore array
typedef struct fillData {
  docscore_t* array;
  int index;
} fillData_t;

// main function that runs querier
int main(int argc, char* argv[])
{
  char* pageDirectory = NULL;
  char* indexFilename = NULL;

  // read arguments
  parseArgs(argc, argv, &pageDirectory, &indexFilename);

  // load the index given by user
  index_t* idx = index_load(indexFilename);
  if (idx == NULL) {
    fprintf(stderr, "Error: could not load index from file '%s'\n", indexFilename);
    exit(4);
  }

  // read queries from stdin
  char buffer[MAX_QUERY_LINE + 2];
  while (true) {
    prompt();
    if (!readQueryLine(buffer, stdin)) {
      //EOF causes break or error so it will happen
      break;
    }

    // turn line into array of words
    int nwords = 0;
    char** words = parseQuery(buffer, &nwords);
    if (words == NULL || nwords == 0) {
      // if there's a parse error
      free(words);
      continue;
    }

    // make sure query is acceptable
    if (!validateQuery(words, nwords)) {
      //error message print inside validateQuery
      free(words);
      continue;
    }

    // give the user their final query
    printf("Query:");
    for (int i = 0; i < nwords; i++) {
      printf(" %s", words[i]);
    }
    printf("\n");

    // performs the query analysis; creates counters object mapping docIDs to scores
    counters_t* results = handleQuery(words, nwords, idx);

    // print the results
    if (results == NULL) {
      printf("No documents match.\n");
    } else {
      printResults(results, pageDirectory);
      counters_delete(results);
    }

    // line :)
    printf("----------------------------------------\n");

    free(words);
  }

  // finished
  index_delete(idx);
  return 0;
}


static void parseArgs(int argc, char* argv[], char** pageDir, char** indexFilename)
{
  if (argc != 3) {
    fprintf(stderr, "Usage: %s pageDirectory indexFilename\n", argv[0]);
    exit(1);
  }
  *pageDir = argv[1];
  *indexFilename = argv[2];

  // validate pageDirectory with method from common
  if (!pagedir_validate(*pageDir)) {
    fprintf(stderr, "Error: invalid crawler directory '%s'\n", *pageDir);
    exit(2);
  }

  // check indexFilename by opening for read
  FILE* fp = fopen(*indexFilename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: cannot open index file '%s'\n", *indexFilename);
    exit(3);
  }
  fclose(fp);
}

// print a prompt
static void prompt(void)
{
  printf("Query? ");
  fflush(stdout);
}

/*
 * Reads a line from given File (expect stdin)
 * Return false if EOF or error and true on success
 * If line is longer than defined MAX_QUERY_Line error
 */
static bool readQueryLine(char* buffer, FILE* fp)
{
  if (fgets(buffer, MAX_QUERY_LINE + 2, fp) == NULL) {
    return false;
  }

  // if line is too long, skip through end
  if (strchr(buffer, '\n') == NULL) {
    fprintf(stderr, "Error: query exceeds max length (limit %d chars)\n", 
           MAX_QUERY_LINE);
    // skip rest of line
    int ch;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) { }
    buffer[0] = '\0';
    return true;
  }

  return true;
}

/*
 * Convert the line to an array of lowercase words
 * Return NULL if no words are valid
 * Nwords is updated via the passed pointer to nwords
 */
static char** parseQuery(char* line, int* nwords)
{
  // strip whitespace and convert to lower
  // check for bad characters
  int length = strlen(line);
  for (int i = 0; i < length; i++) {
    if (isupper((unsigned char)line[i])) {
      line[i] = tolower(line[i]);
    } else if (!isalpha((unsigned char)line[i]) && !isspace((unsigned char)line[i])) {
      // this chracter is not valid
      if (line[i] != '\0' && line[i] != '\n') {
        fprintf(stderr, "Error: bad char '%c' in query.\n", line[i]);
      }

      // disregard rest of line
      line[0] = '\0';
      *nwords = 0;
      return NULL;
    }
  }

  // use white space to make tokens
  char** words = calloc(MAX_QUERY_WORDS, sizeof(char*));
  if (words == NULL) {
    fprintf(stderr, "Error: out of memory (could not allocate word array).\n");
    *nwords = 0;
    return NULL;
  }

  int count = 0;
  char* token = strtok(line, " \t\r\n"); // tokenizes based on whitespace (r is carriage return)
  while (token != NULL) {
    if (count >= MAX_QUERY_WORDS) {
      fprintf(stderr, "Error: too many words in query (max %d)", MAX_QUERY_WORDS);
      free(words);
      *nwords = 0;
      return NULL;
    }
    words[count] = token;
    count++;
    token = strtok(NULL, " \t\r\n"); // repeat loop!
  }
  *nwords = count;
  if (count == 0) {
    // empty
    free(words);
    return NULL;
  }
  return words;
}

/*
 * Check the query for syntax errors:
 *
 * - first and last words can never be 'and'/'or' operators
 * - operators cannot be adjacent
 */
static bool validateQuery(char** words, int nwords)
{
  // no query haha
  if (nwords < 1) return false;

  //check first word
  if (strcmp(words[0], "and") == 0) {
    fprintf(stderr, "Error: 'and' cannot be first word\n");
    return false;
  }
  if (strcmp(words[0], "or") == 0) {
    fprintf(stderr, "Error: 'or' cannot be first word\n");
    return false;
  }

  // check last word
  if (strcmp(words[nwords-1], "and") == 0) {
    fprintf(stderr, "Error: 'and' cannot be last word\n");
    return false;
  }
  if (strcmp(words[nwords-1], "or") == 0) {
    fprintf(stderr, "Error: 'or' cannot be last word\n");
    return false;
  }

  // loop and check adjacency
  for (int i = 0; i < nwords-1; i++) {
    if ((strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) &&
        (strcmp(words[i+1], "and") == 0 || strcmp(words[i+1], "or") == 0)) {
      fprintf(stderr, "Error: '%s' and '%s' cannot be adjacent in query\n",
              words[i], words[i+1]);
      return false;
    }
  }

  return true; // all good
}

/*
 * Takes input of an array of words and an index
 * Returns counters of docIDs with score for each docID
 *
 * Evaluatess and operators first by calling parseOrSequence
 *   whic calls parseAndSequence
 */
static counters_t* handleQuery(char** words, int nwords, index_t* index)
{
  int pos = 0;
  counters_t* result = parseOrSequence(words, &pos, nwords, index);

  if (result == NULL) {
    return NULL; // failed to get a result
  }

  // see if any docID has non-zer count
  bool anyNonZero = false;

  // check for non zeros
  counters_iterate(result, &anyNonZero, foundNonZeroFn);
  if (!anyNonZero) {
    counters_delete(result);
    return NULL;
  }

  return result;
}

/*
 * Parse 'and' blocks separated by 'or'
 *   (doesn't necessarily mean there is an 'and' present)
 * 
 * Unions counters
 *
 */
static counters_t* parseOrSequence(char** words, int* pos, int nwords, index_t* index)
{
  //start with first 'and' block
  counters_t* result = parseAndSequence(words, pos, nwords, index);

  // while next word is 'or' apply union with next 'and' block
  while (*pos < nwords && strcmp(words[*pos], "or") == 0) {
    (*pos)++; //skips the actual world 'or'

    // constructs counters for next section
    counters_t* next = parseAndSequence(words, pos, nwords, index);
    if (result == NULL) {
      result = next; // just add it if it's first
    } else if (next != NULL) {
      countersOrCombine(result, next); // combine the next portion
      counters_delete(next);
    }
  }

  return result;
}

/*
 * Parse words separated by 'and'
 *
 * 'and' is assumed to be operator when others are lacking
 *
 * intersects counters of blocks separated by 'and'
 */
static counters_t* parseAndSequence(char** words, int* pos,
                                    int nwords, index_t* index)
{
  // deal with first word
  while (*pos < nwords &&
         (strcmp(words[*pos], "and") == 0 || strcmp(words[*pos], "or") == 0)) {
    // skip extra operators
    (*pos)++;
  }

  // out of bounds
  if (*pos >= nwords) {
    return NULL;
  }

  // get counters for a given word
  counters_t* result = getCountersForWord(words[*pos], index);
  (*pos)++;

  // unless next word is or, interpret as and
  while (*pos < nwords &&  strcmp(words[*pos], "or") != 0) {
    if (strcmp(words[*pos], "and") == 0) {
      // skip!
      (*pos)++;
      if (*pos >= nwords) break; // at end
    }

    // combine with next word
    counters_t* next = getCountersForWord(words[*pos], index);
    countersAndCombine(result, next);
    counters_delete(next);
    (*pos)++;
  }

  return result;
}

/*
 * For a given word, find the counters that represents that words
 *   appearance in a given document
 *
 */
static counters_t* getCountersForWord(const char* word, index_t* index)
{
  // if 'and' or 'or', NULL
  if (strcmp(word, "and") == 0 || strcmp(word, "or") == 0) {
    return NULL;
  }

  // already normalized, but check length
  if (strlen(word) < 3) {
    return counters_new(); // there will be no match so it is empty
  }

  counters_t* ctrs = index_find(index, word);
  if (ctrs == NULL) {
    // no matches
    return counters_new(); // empty
  }

  // create a copy so original is preserved
  counters_t* copy = counters_new();
  if (copy == NULL) return NULL; // bad generation

  // iterates over counters and calls copyCountFn
  counters_iterate(ctrs, copy, copyCountFn);
  return copy;
}

/*
 * Inserct for the 'and' operator:
 *   for each docID in dest, the new value is the min of
 *   the old count and src
 *
 *   (if it's not in src, then just 0)
 */
static void countersAndCombine(counters_t* dest, counters_t* src)
{
  if (dest == NULL || src == NULL) {
    return; // empty parameter
  }

  // update src to hold new intersection coutns
  counters_iterate(dest, src, andCombineHelper1);

  // copy src into dest
  counters_iterate(src, dest, andCombineHelper2);
}

/*
 * Combines counters via the 'or' operator using union
 *
 * Simply sums the values of each docID in src and dest
 * (if a value is not in src, but in dest, the sum is just the value
 *  already in dest)
 *
 */
static void countersOrCombine(counters_t* dest, counters_t* src)
{
  if (dest == NULL || src == NULL) {
    return; // empty!
  }

  // use orCombineHelper to union
  counters_iterate(src, dest, orCombineHelper);
}

/**************Functions for counters_iterate******************/

// if you find a non-zero docID, mark bool to true
static void foundNonZeroFn(void* arg, const int key, const int count)
{
  bool* found = (bool*) arg;
  if (count > 0) {
    *found = true;
  }
}

// Copy from one counter to another
static void copyCountFn(void* arg, const int key, const int count)
{
  counters_t* dst = (counters_t*) arg;
  counters_set(dst, key, count);
}

// logic for intersection of counters takes the lowest of two counts
static void andCombineHelper1(void* arg, const int key, const int countDest)
{
  // arg is src
  counters_t* srcCtrs = (counters_t*) arg;
  int countSrc = counters_get(srcCtrs, key);
  int newCount = (countDest < countSrc) ? countDest : countSrc;
  if (countSrc == 0) { // if not present we can assume 0
    newCount = 0;
  }
  counters_set(srcCtrs, key, newCount); // updates source
}

// actually copies source into destination
static void andCombineHelper2(void* arg, const int key, const int countSrc)
{
  counters_t* destCtrs = (counters_t*) arg;
  counters_set(destCtrs, key, countSrc);
}

// union logic for adding values of two counters
static void orCombineHelper(void* arg, const int key, const int countSrc)
{
  counters_t* destCtrs = (counters_t*) arg;
  int countDest = counters_get(destCtrs, key);
  int newCount = countDest + countSrc;
  counters_set(destCtrs, key, newCount);
}



/*
 * Takes a final counters of results, and prints them out for the user
 *   also prints out the URL for the user to see.
 *
 * The results are not currently ordered by how well they match.
 */
static void printResults(counters_t* results, const char* pageDir)
{
  if (results == NULL) {
    printf("No documents match.\n");
    return;
  }

  // check that we don't have 0 docs with a match
  bool anyNonZero = false;
  counters_iterate(results, &anyNonZero, foundNonZeroFn);
  if (!anyNonZero) {
    printf("No documents match.\n");
    return;
  }

  // for creating the proper size struct
  int nDocs = countNonZeroDocs(results);
  if (nDocs == 0) {
    printf("No documents match.\n");
    return;
  }

  docscore_t* array = calloc(nDocs, sizeof(docscore_t));
  if (!array) {
    fprintf(stderr, "Error: out of memory for docscore array.\n");
    return;
  }

  // fills the array
  fillData_t data;
  data.array = array;
  data.index = 0;
  counters_iterate(results, &data, addIfNonZero);

  // actually sorts the array
  qsort(array, nDocs, sizeof(docscore_t), compareDocscore);

  printf("Matches %d documents (sorted by score in order):\n", nDocs);

  // loop through and print values
  for (int i = 0; i < nDocs; i++) {
    int docID = array[i].docID;
    int score = array[i].score;

    // print filename
    char filename[1024];
    snprintf(filename, sizeof(filename), "%s/%d", pageDir, docID);

    FILE* fp = fopen(filename, "r");
    if (!fp) continue;
    char url[1024];
    if (!fgets(url, sizeof(url), fp)) {
      fclose(fp);
      continue;
    }
    fclose(fp);
    // strip newline
    char* nl = strchr(url, '\n');
    if (nl) *nl = '\0';

    // actually print the score and URL
    printf("score\t%d doc %3d: %s\n", score, docID, url);
  }

  free(array);
}

// helps iterate through and see if score is nonZero value with simple check
static void countNonZeroHelper(void* arg, const int docID, const int score)
{
  if (score > 0) {
    int* c = arg;
    (*c)++;
  }
}

// Counts the number of non zero docs by iterating
static int countNonZeroDocs(counters_t* ctrs)
{
  int count = 0;
  counters_iterate(ctrs, &count, countNonZeroHelper);
  return count;
}

// adds a doc to data's array if it is nonZero score
static void addIfNonZero(void* arg, const int docID, const int score)
{
  fillData_t* data = arg;
  if (score > 0) {
    data->array[data->index].docID = docID;
    data->array[data->index].score = score;
    data->index++;
  }
}

// compares the scores of two docs to see which is bigger
static int compareDocscore(const void* a, const void* b)
{
  const docscore_t* da = a;
  const docscore_t* db = b;
  return (db->score - da->score); // descending by score
}
