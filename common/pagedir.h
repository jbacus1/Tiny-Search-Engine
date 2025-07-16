/* pagedir.h - header file for CS50 TSE page directory
 *
 * Gives two functions, one for creating a file for crawler use
 *   and another for saving a webpage to a directory
 *
 *
 * Author: Jacob Bacus
 * Date: Feburary 2025
 */

#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdbool.h>
#include "../libcs50/webpage.h"

/*
 * Creates a file called ".crawler" in given pageDirectory to
 * indicate that it is a directory produced by the crawler.
 *
 * Returns:
 *   true on a successful run
 *   false otherwise
 */
bool pagedir_init(const char* pageDirectory);


/*
 * Saves a webpage to pageDirectory/docID. The file will contain:
 *   The webpage URL
 *   The webpage depth
 *   subsequent lines: the webpage's HTML content
 *
 * Caller provides:
 *  valid page
 *  valid directory
 *  positive integer docID
 *
 * Will not do anything if parameters are bad
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/*
 * Returns true if a directory is created by a crawler
 * Works by checking for a ".crawler" file
 *
 * The user provide a valid pageDirectory
 * We return true of false depending on if the ".crawler" file is present
 */
bool pagedir_validate(const char* pageDirectory);

/*
 * Loads the file specified by "pageDirectory/docID"
 *   (loads as a webpage)
 * 
 * The user provides a valid pageDirectory and docID for within crawler
 *
 * We return a pointer to newly allocated webpage_t or NULL on failure
 *
 * The user is responsible for freeing the memory in the webpage with
 *   webpage_delete()
 */
webpage_t* pagedir_load(const char* pageDirectory, const int docID);

#endif
