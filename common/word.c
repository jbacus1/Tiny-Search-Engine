/*
 * word.c - creates a normalized word
 *
 * Full details in word.h
 *
 * Author: Jacob Bacus
 *
 * Feburary 2025
 */

#include <ctype.h>
#include <string.h>
#include "word.h"
#include <stdbool.h>

bool normalizeWord(char* word)
{
  if (word == NULL) {
    return false;
  }
  size_t length = strlen(word);

  // word is trivial and ignorable
  if (length < 3) {
    return false;
  }
  // convert to lower-case
  for (size_t i = 0; i < length; i++) {
    word[i] = tolower((unsigned char) word[i]);
  }

  return true;
  
}
