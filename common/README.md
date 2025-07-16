# common - README

This directory contains modules used across TSE components:
- **index.c / index.h**:
  - `index_new` creates a new index implemented with a hashtable
  - `index_insert` Puts a word into the index hashtable as a key, creates a counter as item at this key which is then incremented at the given docID
  - `index_find` checks if a counter exists for a word and returns it if it does
  - `index_save` saves an entire index to a file
  - `index_load` takes a filename and generates an index from it
  - `index_delete` frees all memory of an index

- **pagedir.c / pagedir.h**:
  - `pagedir_init` checks that a directory is usable for the crawler and creates a file `.crawler` to indicate that it is a crawler directory
  - `pagedir_save` writes contents of a page to the directory
  - `pagedir_validate` checks that a directory contains `.crawler` indicating that it is a good set of data to read from
  - `pagedir_load` uses a pageDirectory and docID to generate a webpage struct from the file titled docID within said pageDirectory

- **word.c / word.h**:
  - `normalizeWord` takes a pointer to a word and modifies the word to be lowercase and NULL if it is 3 or less characters

## Assumptions
- The directory given to pagedir functions `const char* pageDirectory` must already exist
- The docID given to `pagedir_load` must exist within the pageDirectory
- `pagedir_save` needs the webpage to already be fetched
- `word.c` expects regular alphabetical characters
- `index.c` expects valid directory and filename args

## Bugs
- No known bugs