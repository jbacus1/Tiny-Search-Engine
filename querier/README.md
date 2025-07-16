# Querier - README

**Author**: Jacob Bacus

**Date**: Feburary 2025

## Description

This directory contains the implementation of the Querier portion of the Tiny Search Engine.
1. The Querier takes 2 command-line arguments `pageDirectory` and `indexFilename`.
2. The Querier reconstructs an index from indexFilename
3. The program takes input from the user using words along with 'and' and 'or' operators separating them
4. The Querier outputs urls and scores based on how often words appear in page files. The output appears in descending order of score

## Files
- **querier.c** implements the logic of the querier
- **Makefile** builds `querier`
- **testing.sh**: testing script for querier
- **testing.out**: output from running `make test` or `./testing.sh`
- **README.md**: this file
- **IMPLEMENTATION.md**: explains the implementation of querier
- **DESIGN.md**: explains the design of querier
- **.gitignore**

## Bugs
- No currently known bugs

## Usage
1. Run `make` within this directory or in upper directories
2. Run the executable `querier` with correct arguments for directory and file
3. Input search query
4. Check fro correct url and score output

4. Alternatively run `make test` to check for expected outputs (output is automatically sent to testing.out file)
5. Cross reference output to actual occurences of words in index file