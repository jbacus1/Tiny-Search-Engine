# Indexer - README

**Author**: Jacob Bacus
**Date**: Feburary 2025

## Description

This directory contains the implementation of the Indexer portion of the Tiny Search Engine.
1. The Indexer takes 2 command-line arguments `pageDirectory` and `indexFilename`.
2. The Indexer reads from files built by Crawler in `pageDirectory` and creates an inverted index of a word to a count of how many times it occurs in each docID
3. Saves the results to a file `indexFilename`

## Files
- **indexer.c** implements the logic of the indexer
- **Makefile** builds `indexer` and `indextest`
- **testing.sh**: testing script for indexer
- **testing.out**: output from running `make test` or `./testing.sh`
- **valgrind.out**: output from running `make test` or `./testing.sh` for valgrind
- **README.md**: this file
- **IMPLEMENTATION.md**: explains the implementation of indexer
- **indextest**: loads an index file and saves it to another file

## Bugs
- No currently known bugs

## Usage
1. Run `make` within this directory or in upper directories
2. Run the executable `indexer` with correct arguments
3. Check file for output

4. Alternatively run `make test` to check for expected outputs (output is automatically sent to testing.out file)
5. Check `valgrind.out` for detecting memory leaks
6. Check the indexes file in data for appropriate files and that testing.out shows appropriate logging
