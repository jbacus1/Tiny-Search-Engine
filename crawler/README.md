# Crawler - README

**Author**: Jacob Bacus
**Date**: Feburary 2025

## Description

This directory contains the implementation of the Crawler portion of the Tiny Search Engine.
1. The Crawler takes 3 command-line arguments `seedURL`, `pageDirectory`, `maxDepth`.
2. Crawls all URLS reachable from `seedURL` until it reaches `maxDepth` ignoring URLS outside of cs50 server.
3. Saves each page to `pageDirectory` with a unique ID

## Files
- **crawler.c** implements the logic of the crawler
- **Makefile** builds `crawler`
- **testing.sh**: testing script for crawler
- **testing.out**: output from running `make test` or `./testing.sh`
- **valgrind.out**: output from running `make test` or `./testing.sh` for valgrind
- **README.md**: this file

## Design
- `webpage.h` uses a 1-second delay between fetches
- `pagedir.h` in common creates directories and saves pages
- `maxDepth` cannot be greater than 10 (or less than 0)

## Bugs
- No currently known bugs

## Usage
1. Run `make` within this directory or in upper directories
2. Run the executable `crawler` with correct arguments
3. Check file for output

4. Alternatively run `make test` to check for expected outputs (output can be sent to testing.out file)
5. Check `valgrind.out` for detecting memory leaks
6. Check that data files have expected files and that testing.out shows appropriate logging