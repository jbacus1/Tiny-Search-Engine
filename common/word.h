#ifndef __WORD_H
#define __WORD_H

#include <stdbool.h>

/*
 * Converts word to lowercase in place
 * Return true if word is length >= 3
 * Return false if word is length < 3 or NULL
 *
 */
bool normalizeWord(char* word);

#endif // __WORD_H
