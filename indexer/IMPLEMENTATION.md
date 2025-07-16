# CS50 TSE Indexer
## Implementation Spec

Here we focus on the core subset:

-  Data structures
-  Control flow for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling
-  Testing plan

## Data structures 

The indexer primarily utilizes an inverted index to store its data. This is a hashtable with strings (a word) as keys. Each key is mapped to one counters object which is keyed by a docID to store a count of how many times each word occurs in a docID.\

- This approach leverages `hashtable.h` and `counters.h` from `libcs50`

## Control flow

The Indexer is implemented in one file `indexer.c`, with four functions.

### main

The `main` function simply calls `parseArgs`, creates a new index, uses `buildIndex` to populate the index, and saves the index in `index_save` (contained in `index.c`) then exits zero.

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `pageDirectory`, confirm that it is a crawler directory
* for `indexFilename`, confirm the file is writable
* if any trouble is found, print an error to stderr and exit non-zero.

### buildIndex

Reads through each file in `pageDirectory` by incrementing docID and add it ot the index

### indexPage

Takes words from a webpage one at a time, normalizes them (with `word` module), then inserts them into an index using `index_insert`

## Other modules

### pagedir

Utilize functions that checks that directories were created by crawler and loads files as webpage structs in `pagedir_load`

### word

Contains a single function that normalizes words by reading through string, checking length and convertin to lowercase.

### index

Contains the implementation of the index struct. Utilizes `hashtable` and `counters` to keep track of the inverted index. Includes methods to add to the index, save it to files, insert into the index, and load from a file.

### libcs50

We leverage the modules of libcs50, most notably `counters`, `hashtable`, and `webpage`.
See that directory for module interfaces.

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
int main(int argc, char* argv[])
static void parseArgs(int argc, char* argv[], char** pageDirectory, char** indexFilename)
static void buildIndex(const char* pageDirectory, index_t* index)
static void indexPage(webpage_t* page, int docID, index_t* index)
```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `pagedir.h` and is not repeated here. Only the two utilized functions are listed.

```c
bool pagedir_validate(const char* pageDirectory);
webpage_t* pagedir_load(const char* pageDirectory, const int docID);
```

### word

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `word.h` and is not repeated here.

```c
bool normalizeWord(char* word);
```

### index

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `index.h` and is not repeated here.

```c
index_t* index_new(const int slots);
bool index_insert(index_t* idx, const char* word, const int docID);
counters_t* index_find(index_t* idx, const char* word);
void index_save(index_t* idx, const char* filename);
index_t* index_load(const char* filename);
void index_delete(index_t* idx);
```

## Error handling and recovery

All functions check parameters robustly to avoid invalid parameters. In the case of an invalid parameter, either an error is printed or nothing is returned. Memory is free'd throughout functions to prevent any memory leaks. Valgrind testing verifies full memory safety.

## Testing plan

- Utilize premade crawler files
- Test for various extraneous arguments to ensure full functionality.
- Perform a Valgrind test to check for memory leaks on a moderate directory
- Test the ability to create indexes on various directories
- Utilize `indextest` to load an index from an index file and write it to a new file