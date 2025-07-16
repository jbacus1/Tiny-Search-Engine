/*
 * indextest.c: loads index file, saves to another file
 *
 * usage: ./indextest oldIndexFilename newIndexFilename
 *
 * Author: Jacob Bacus
 * Feburary 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/index.h"

int main (int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s oldIndexFilename newIndexFilename", argv[0]);
    exit(1);
  }

  char* oldIndex = argv[1];
  char* newIndex = argv[2];

  index_t* index = index_load(oldIndex);
  if (index == NULL) {
    fprintf(stderr, "error making index file '%s'\n", oldIndex);
    exit(2);
  }

  index_save(index, newIndex);

  index_delete(index);

  return 0;
}
