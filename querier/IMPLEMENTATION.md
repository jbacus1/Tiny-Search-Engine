# Querier - Implementation

Author: Jacob Bacus

Date: February 2025

## Data Structures
1. ### index_t:
   - Hashtable keyed with word, storing counters_t of docID->count for every word
   - Loads from ```indexFilename```
2. ### counters_t:
   - Stores a score for each docID
   - Tracks partial results of each ```and``` / ```or``` operation
3. ### word tokens:
   - an array of strings from user's query
4. ### docscore
   - a simple struct that contains a docID and a score
5. ### fillData
   - a struct that implements an array of docscores

## Control Flow

1. ### ```main```:
   - Validates arguments, loads the index from file, loops reading queries
2. ### ```readQueryLine```:
   - Takes a line from stdin, dtects EOF if present
3. ### ```parseQuery```:
   - Tokenizes line, checks for bad characters, and normalizes
4. ### ```validateQuery```:
   - Checks that first/last words are not ```and/or```, checks they are not adjacent too
5. ### ```handleQuery```:
   - Interprets the tokens with ```and``` over ```or```
   - Merges partial results using intersection for ```and``` and union for ```or```
6. ### ```printResults```:
   - If there are no results matching, print "No documents match."
   - Otherwise, for each matching docID, load the doc's url from ```pageDirectory/docID and``` print ```(score, docID, URL)```.
   - The order of printing is determined by sorting docs in order of their score using a simple datastructure that contains a score and docID

## Function Prototypes
```c
int main(int argc, char* argv[])
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
static void foundNonZeroFn(void* arg, const int key, const int count);
static void copyCountFn(void* arg, const int key, const int count);
static void andCombineHelper1(void* arg, const int key, const int countDest);
static void andCombineHelper2(void* arg, const int key, const int countSrc);
static void orCombineHelper(void* arg, const int key, const int countSrc);
static int  countNonZeroDocs(counters_t* ctrs);
static void addIfNonZero(void* arg, const int docID, const int score);
static int  compareDocscore(const void* a, const void* b);
```

## Detailed Error Handling
- ### Command Line:
  - If argc is not 3 or ```indexFilename``` or ```pageDirectory``` is invalid, and error is printed and the program exits non-zero
- ### Memory:
  - If allocation fails, and error is printed and the program exits non-zero
- ### Queries:
  - If the line is too long, it is skipped
  - If characters or syntax is bad, prints an error and asks for another query
- ### Missing docs:
  - If a doc cannnot be read from ```pageDirectory``` it is skipped

## Testing

A ```testing.sh``` script runs:
- Bad arguments
- Queries from files that are valid/invalid
- Valgrind test