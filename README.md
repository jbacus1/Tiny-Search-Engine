# Tiny Search Engine

## Overview

This project implements a complete search engine system with three integrated components: a web crawler, an indexer, and a querier. The system was constructed through a design-to-implementation pipeline, beginning with specific requirements  and progressing through detailed design specifications to final implementation. By utilizing efficient data structures and algorithms, the Tiny Search Engine can crawl web pages, build inverted indexes, and process complex queries with AND/OR operators to deliver ranked search results.

---

## Key Features

### Web Crawler (`crawler`)

- **Breadth-First Traversal**: Systematically explores web pages starting from a seed URL up to a specified maximum depth.
- **Hashtable-Based Deduplication**: Uses a hashtable to track visited URLs, preventing duplicate crawling and infinite loops.
- **Bag Data Structure**: Implements a bag to manage the queue of pages to be crawled, ensuring efficient page processing.
- **Respectful Crawling**: Includes a 1-second delay between fetches to avoid overwhelming servers.

### Indexer (`indexer`)

- **Inverted Index Construction**: Builds an efficient inverted index mapping each word to its occurrences across all documents.
- **Hashtable Storage**: Utilizes hashtables with word keys mapping to counters that track word frequency per document.
- **Normalized Word Processing**: Converts all words to lowercase and filters out words shorter than 3 characters.
- **Persistent Storage**: Saves the complete index to disk for use by the querier component.

### Querier (`querier`)

- **Boolean Query Processing**: Supports complex queries with AND/OR operators, where AND takes precedence over OR.
- **Ranking Algorithm**: Scores documents based on word frequency and returns results in descending order of relevance.
- **Interactive Interface**: Provides a command-line interface for real-time query processing.
- **Query Validation**: Ensures proper syntax and handles edge cases in user input.

### Design Principles

- **Modular Architecture**: Each component is independently testable and maintainable.
- **Separation of Concerns**: Clear interfaces between crawler, indexer, and querier components.
- **Robust Error Handling**: Comprehensive validation and graceful failure modes.
- **Memory Management**: Careful allocation and deallocation using consistent patterns.

---

## System Components

### Crawler
- Starts from a seed URL and explores linked pages within the CS50 server domain.
- Saves each crawled page to a designated directory with unique document IDs.
- Maintains crawling boundaries with configurable maximum depth (≤ 10 for project scope).

### Indexer
- Reads crawled pages from the crawler's output directory.
- Processes HTML content to extract and normalize words.
- Constructs an inverted index and writes it to a structured file format.

### Querier
- Loads the index file created by the indexer.
- Processes user queries with support for AND/OR boolean operations.
- Returns ranked results with document URLs and relevance scores.

### Common Library
- Shared utilities for page directory management, word processing, and index operations.
- Leverages the CS50 library for fundamental data structures (hashtables, counters, bags).

---

## How It Works

1. **Crawling Phase**:
   - Initialize with a seed URL and maximum depth parameter.
   - Use breadth-first search to discover and fetch web pages.
   - Save each page with a unique document ID for indexer processing.

2. **Indexing Phase**:
   - Read all crawled pages from the specified directory.
   - Extract and normalize words from HTML content.
   - Build an inverted index mapping words to document frequencies.

3. **Querying Phase**:
   - Load the pre-built index into memory.
   - Parse user queries and validate syntax.
   - Process boolean operations and rank results by relevance.

---

## Environment Setup

### Build Requirements
- GCC compiler with C99 support
- CS50 library (libcs50) included in the project
- Unix-like environment for Makefile compilation

### Usage

1. **Build the system**:
   ```bash
   make
   ```

2. **Run the crawler**:
   ```bash
   ./crawler/crawler [seedURL] [pageDirectory] [maxDepth]
   ```
   Example:
   ```bash
   ./crawler/crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ./data/letters 2
   ```

3. **Run the indexer**:
   ```bash
   ./indexer/indexer [pageDirectory] [indexFilename]
   ```
   Example:
   ```bash
   ./indexer/indexer ./data/letters ./data/letters.index
   ```

4. **Run the querier**:
   ```bash
   ./querier/querier [pageDirectory] [indexFilename]
   ```
   Example:
   ```bash
   ./querier/querier ./data/letters ./data/letters.index
   ```
   Then enter queries interactively:
   ```
   Query? computer science and dartmouth
   ```

### Testing
- Each component includes comprehensive testing scripts
- Run `make test` in any component directory for automated testing
- Valgrind integration for memory leak detection

---

## Acknowledgments

This project was developed as part of the Dartmouth CS50 curriculum, Winter 2025.

---

## License

This project is distributed under the MIT License. Refer to the LICENSE file for detailed terms and conditions.
