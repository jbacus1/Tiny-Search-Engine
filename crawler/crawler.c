/*
 * crawler.c - Crawler portion of CS50 TSE
 *
 * Usage: ./crawler seedURL pageDirectory maxDepth
 *
 * The crawler begins at a seedURL and travels links until it reaches a maxDepth
 *   the traversed webpages are saved to pageDirectory
 *
 *
 * Author: Jacob Bacus
 * Feburary 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../libcs50/webpage.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/bag.h"
#include "../common/pagedir.h"

// struct that holds wepage and depth which is given to bag

typedef struct crawler_page {
  webpage_t* page;
  int docID;
} crawler_page_t;

// function prototypes
static void parseArgs (const int argc, char* argv[],
                       char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen,
                     int maxDepth);


// runs the crawler
int main(const int argc, char* argv[])
{
  char* seedURL = NULL;
  char* pageDirectory = NULL;
  int maxDepth = 0;

  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  crawl(seedURL, pageDirectory, maxDepth);
  
  //  free(seedURL);

  return 0; // everything worked!
}

// read arguments from main and attempt to parse into usable forms
static void parseArgs (const int argc, char* argv[],
                       char** seedURL, char** pageDirectory, int* maxDepth)
{
  // check for correct argument count
  if (argc != 4) {
    fprintf(stderr, "Usage: %s seedURL pageDirectory maxDepth\n", argv[0]);
    exit(1);
  }

  // parse seedURL
  const char* givenURL = argv[1];

  // try to normalize URL provided
  char* normalized = normalizeURL(givenURL);
  if (normalized == NULL) {
    fprintf(stderr, "Error: seedURL '%s' cannot be normalized.\n", givenURL);
    exit(2);
  }

  // checks that the URL is within the cs50 servers
  if (!isInternalURL(normalized)) {
    fprintf(stderr, "Error: seedURL '%s' is not internal.\n", givenURL);
    free(normalized);
    exit(3);
  }
  
  *seedURL = normalized; // use the normalized url

  // parse pageDirectory
  const char* dir = argv[2];
  if (!pagedir_init(dir)) {
    fprintf(stderr, "Error: cannot intialize director '%s' \n", dir);
    free(normalized); // necessary free since URL was created
    exit(4);
  }
  *pageDirectory = (char*)dir;

  // parse maxDepth
  char* end; // points to first non-numeric char
  long depth = strtol(argv[3], &end, 10);
  if (*end != '\0' || depth < 0 || depth > 10) {
    fprintf(stderr, "Error: invalid maxDepth '%s'\n", argv[3]);
    free(normalized);
    exit(5);
  }
  *maxDepth = (int)depth;
}

// loop for crawling webpages
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  // make a hashtable to track seen URLs
  hashtable_t* pagesSeen = hashtable_new(200);
  if (pagesSeen == NULL) {
    fprintf(stderr, "could not create hashtable.\n");
    exit(6);
  }

  //place seedURL into the hashtable
  bool inserted = hashtable_insert(pagesSeen, seedURL, "");
  if (!inserted) {
    fprintf(stderr, "Error: could not insert seedURL.");
    hashtable_delete(pagesSeen, NULL);
    exit(7);
  }

  // create bag of pages that need to be crawled
  bag_t* pagesToCrawl = bag_new();
  if (pagesToCrawl == NULL) {
    fprintf(stderr, "Could not create bag.\n");
    hashtable_delete(pagesSeen, NULL);
    exit(8);
  }

  // make the webpage for teh seed
  webpage_t* seedPage = webpage_new(seedURL, 0, NULL);
  if (seedPage == NULL) {
    fprintf(stderr, "Could not create seed webpage.\n");
    bag_delete(pagesToCrawl, NULL);
    hashtable_delete(pagesSeen, NULL);
    exit(9);
  }

  bag_insert(pagesToCrawl, seedPage);

  int docID = 1; // start docID at 1
  
  // crawling loop

  webpage_t* page;
  while ((page = bag_extract(pagesToCrawl)) != NULL) {
    int depth = webpage_getDepth(page);
    
    // get HTML for page
    if (webpage_fetch(page)) {
      // print fetching message
      printf("%2d   Fetched: %s\n", depth, webpage_getURL(page));
      
      // save the page
      pagedir_save(page, pageDirectory, docID);
      docID++;
      
      // get URLs if page isn't at maxdepth
      if (depth < maxDepth) {
        printf("%2d  Scanning: %s\n", depth, webpage_getURL(page));
        pageScan(page, pagesToCrawl, pagesSeen, maxDepth);
      }
    }

    // frees portions
    webpage_delete(page);
    //free(page);
  }

  // don't need data structures
  bag_delete(pagesToCrawl, NULL);
  hashtable_delete(pagesSeen, NULL);
}


// finds urls to add to pagesToCrawl
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen,
                     int maxDepth)
{
  //check if parameters are valid
  if (page == NULL || pagesToCrawl == NULL || pagesSeen == NULL) {
    return;
  }

  int depth = webpage_getDepth(page);
  int pos = 0;
  char* foundURL = NULL;
  
  while ((foundURL = webpage_getNextURL(page, &pos)) != NULL) {
    // print message for finding URL
    printf("%2d     Found: %s\n", depth, foundURL);

    // normalize url
    char* normURL = normalizeURL(foundURL);
    if (normURL == NULL) {
      free(foundURL);
      continue;
    }

    // check if URL is in cs50 server
    if (!isInternalURL(normURL)) {
      printf("%2d  IgnExtrn: %s\n", depth, normURL);
      free(normURL);
    }
    else {
      if (!hashtable_insert(pagesSeen, normURL, "")) {
        // duplicate
        printf("%2d   Added: %s\n", depth, normURL);
        free(normURL);
      } else {
        // new added page
        webpage_t* newPage = webpage_new(normURL, depth+1, NULL);
        if (newPage != NULL) {
          bag_insert(pagesToCrawl, newPage);
        }
        else {
            free(normURL);
        }
      }
    }
    free(foundURL);
  }
}
