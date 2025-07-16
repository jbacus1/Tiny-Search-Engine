/* pagedir.c - CS50 TSE page directory
 * 
 * see pagedir.h for more information on functions
 *
 * Author: Jacob Bacus
 * Date: Feburary 2025
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pagedir.h"
#include "../libcs50/webpage.h"

// function prototypes
static bool isWritableDirectory (const char* pageDirectory);
static char* makeFilePath(const char* dir, const int docID);

// creates the pageDirectory

bool pagedir_init(const char* pageDirectory)
{
  if (pageDirectory == NULL) {
    fprintf(stderr, "Page Directory is NULL\n");
    return false;
  }

  // use function to check it can be written into by making test file
  if (!isWritableDirectory(pageDirectory)) {
    fprintf(stderr, "directory '%s' is not writable.\n", pageDirectory);
    return false;
  }

  char filename[1024];
  snprintf(filename, sizeof(filename), "%s/.crawler", pageDirectory);

  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "cannot create filepointer");
    return false;
  }
  fclose(fp);

  return true;
}

// save webpage to directory
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
  // check for bad parameters
  if (page == NULL || pageDirectory == NULL || docID <= 0) {
    return;
  }

  // grab page data
  const char* url = webpage_getURL(page);
  const int depth = webpage_getDepth(page);
  const char* html = webpage_getHTML(page);

  // failed to grab
  if (url == NULL || html == NULL) {
    return;
  }

  char * filename = makeFilePath(pageDirectory, docID);
  // failed to make a filename
  if (filename == NULL) {
    return;
  }

  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "cannot create file to write page to\n");
    free(filename);
    return;
  }

  // print data to file
  fprintf(fp, "%s\n", url);
  fprintf(fp, "%d\n", depth);
  fprintf(fp, "%s\n", html);

  fclose(fp);
  free(filename);
}

// checks that the directory given has a .crawler file
bool pagedir_validate(const char* pageDirectory)
{
  // check provided
  if (pageDirectory == NULL) {
    return false;
  }
  
  // check that pageDirectory/.crawler is openable

  //make the path
  char crawlerFile[1024];
  snprintf(crawlerFile, sizeof(crawlerFile), "%s/.crawler", pageDirectory);

  FILE* fp = fopen(crawlerFile, "r");
  if (fp == NULL) {
    return false; // DNE
  }
  fclose(fp);
  return true;
}

// returns a webpage based on given pageDirectory or docID
webpage_t* pagedir_load(const char* pageDirectory, const int docID)
{
  if (pageDirectory == NULL || docID <= 0) {
    return NULL;
  }

  // make filepath
  char filename[1024];
  snprintf(filename, sizeof(filename), "%s/%d", pageDirectory, docID);

  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL; // can't open!
  }

  // read the URL
  char* url = NULL;
  size_t len = 0;
  
  // if it fails
  if (getline(&url, &len, fp) == -1) {
    free(url);
    fclose(fp);
    return NULL;
  }
  
  // remove newline
  char* newline = strchr(url, '\n');
  if (newline) {
    *newline = '\0';
  }

  // read depth
  int depth = 0;

  // if it fails
  if (fscanf(fp, "%d\n", &depth) != 1) {
    free(url);
    fclose(fp);
    return NULL;
  }

  // read remaining html line by line
  char* html = NULL;
  size_t html_len = 0;
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    size_t chunkLen = strlen(buffer);

    // reallocate memory as you go to accomodate new data (+ 1 is for NULL byte)
    char* newHtml = realloc(html, html_len + chunkLen + 1);

    if (newHtml == NULL) {
      // out of memory probably
      free(html);
      free(url);
      fclose(fp);
      return NULL;
    }
    html = newHtml;
    memcpy(html + html_len, buffer, chunkLen + 1); // copy data and '\0'
    html_len += chunkLen;
  }
  fclose(fp);

  // createa a webpage
  webpage_t* page = webpage_new(url, depth, html);
  if (page == NULL) {
    // webpage failed :(
    free(url);
    free(html);
  }
  return page;
}

// attempts to write into directory by making a test file
static bool isWritableDirectory (const char* pageDirectory)
{
  char tmpName[1024];
  snprintf(tmpName, sizeof(tmpName), "%s/.crawler.temp", pageDirectory);

  // tries to write into file that was made
  FILE* fp = fopen(tmpName, "w");
  if (fp == NULL) {
    return false;
  }

  // cleans and returns true
  fclose(fp);
  remove(tmpName);
  return true;
}

// makes a path to the file
static char* makeFilePath(const char* dir, const int docID) {

  // creates a path of the length of the directory plus an arbitrary amount of
  //   space for the docID
  char* path = malloc(strlen(dir) + 50);
  if (path != NULL) {
    sprintf(path, "%s/%d", dir, docID);
  }
  return path;
}
