# Querier - Design

Author: Jacob Bacus

Date: February 2025

## User Interface

The program runs from command line as follows:

```./querier pageDirectory indexFilename```
- pageDirectory: a directory made by Crawler
- indexFilename: index file made by Indexer

After running the Querier reads from ```stdin``` until EOF, prints documents matching the query.

## Input

- Reads from command line for directory and index file.
- Reads queries from ```stdin``` with ```and``` and ```or``` operators

## Outputs

- For valid queries: print cleaned query, then matching documents with their scores and url
- For invalid queries: print error

## Decomposition

1. ### main:
   - parse arguments to ensure they're valid
   - load the index from the given file
   - read queries from stdin
2. ### query parsing:
   - read line
   - convert to lowercase
   - check for bad chars and bad syntax
   - tokenize words and process operators
3. ### query logic:
   - interpret ```and``` with higher precedence than ```or```
   - use intersection and union for counters
4. ### output:
   - Load each matching document's URL from ```pageDirectory``` and print docID, score, and URL descending in order of score

## Major Data Structures
- **Index**: A hastable of words -> counters (docID -> count)
- **Counters**: used to track the score of each docID
- **Pagedir**: loads files in crawler directory and read URL

## Testing Plan
1. ### Argument Tests:
   - Tests a series of invalid arguments to ensure the program won't run
2. ### Queries:
   - Uses single and multi-word queries, ```and```/```or```, and some invalid queries
3. ### Valgrind:
   - Run to ensure no memory leaks