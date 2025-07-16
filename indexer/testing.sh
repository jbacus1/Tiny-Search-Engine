#!/bin/bash
# testing.sh - test script for TSE Indexer
#
# Calls indexer with variety of arguments:
#   1. Argument tests
#   2. Valgrind test
#   3. Indexer tests on various directories
#   4. Uses indextest to load each index file, saves to another file,
#      then compares them with indexcmp.
#
# Usage:
#   bash -v testing.sh
#     (can redirect output to testing.out)
#
# Author: Jacob Bacus
# Date:   February 2025

PROGRAM_INDEXER=./indexer
PROGRAM_TESTER=./indextest

# crawler produced directory locations
DATADIR=../data
# for new index files
INDEXDIR=$DATADIR/indexes
# path to indexcmp
INDEXCMP=~/cs50-dev/shared/tse/indexcmp

# Ensure $INDEXDIR exists
mkdir -p "$INDEXDIR"

# 1. Argument tests
######################################
echo
echo "----- ARGUMENT TESTS -----"

echo "1) No arguments:"
$PROGRAM_INDEXER

echo
echo "2) Only one argument (missing indexFilename):"
$PROGRAM_INDEXER $DATADIR/letters-0

echo
echo "3) Non-existent directory:"
$PROGRAM_INDEXER $DATADIR/bad $INDEXDIR/out.index

echo
echo "4) Directory not produced by crawler (missing .crawler):"
mkdir -p $DATADIR/notcrawler
$PROGRAM_INDEXER $DATADIR/notcrawler $INDEXDIR/test.index

echo
echo "5) Invalid indexFilename (cannot open for writing):"
$PROGRAM_INDEXER $DATADIR/letters-0 /no-permission-dir/index.out

# 2. Valgrind test
################################
echo
echo "---- VALGRIND TEST (toscrape-1) ----"
VALGRIND_LOG=valgrind.out
rm -f "$VALGRIND_LOG"

valgrind --leak-check=full --show-leak-kinds=all --log-file="$VALGRIND_LOG" \
         $PROGRAM_INDEXER $DATADIR/toscrape-1 $INDEXDIR/toscrape-1-val.index

echo "Valgrind output saved in $VALGRIND_LOG"
echo "Index file is $INDEXDIR/toscrape-1-val.index"

# run indextest to load that file, create a copy, compare them
COPYFILE=$INDEXDIR/toscrape-1-val-copy.index
rm -f "$COPYFILE"
$PROGRAM_TESTER $INDEXDIR/toscrape-1-val.index $COPYFILE

echo "Comparing $INDEXDIR/toscrape-1-val.index to $COPYFILE"
if [ -x "$INDEXCMP" ]; then
    $INDEXCMP $INDEXDIR/toscrape-1-val.index $COPYFILE
else
    diff -q $INDEXDIR/toscrape-1-val.index $COPYFILE
fi


# 3. Indexer tests
########################################
# index each directory, run indextest, then compare.

ALL_DIRS=(
  letters-0 letters-1 letters-2 letters-10
  toscrape-0 toscrape-1 toscrape-3
  wikipedia-0 wikipedia-2
)

echo
echo "----- INDEXER TESTS -----"

for dir in "${ALL_DIRS[@]}"; do
  PAGESDIR="$DATADIR/$dir"
  OUTFILE="$INDEXDIR/$dir.index"

  echo
  echo "Indexing $PAGESDIR -> $OUTFILE"
  rm -f "$OUTFILE"

  # run the indexer
  $PROGRAM_INDEXER "$PAGESDIR" "$OUTFILE"

  # check for output fil
  if [ ! -f "$OUTFILE" ]; then
      echo "Failed to produce $OUTFILE"
      continue
  fi

  # run indextest to read file and copy
  COPYFILE="$INDEXDIR/$dir-copy.index"
  rm -f "$COPYFILE"
  echo "indextest $OUTFILE -> $COPYFILE"
  $PROGRAM_TESTER "$OUTFILE" "$COPYFILE"

  # compare (I had issues using indexcmp so I check if i need to use diff)
  echo "Compare $OUTFILE vs $COPYFILE"
  if [ -x "$INDEXCMP" ]; then
      $INDEXCMP "$OUTFILE" "$COPYFILE"
      ret=$?
      if [ $ret -eq 0 ]; then
          echo "$OUTFILE and $COPYFILE match (indexcmp)."
      else
          echo "Differences found by indexcmp (exit code $ret)."
      fi
  else
      diff -q "$OUTFILE" "$COPYFILE"
      ret=$?
    if [ $ret -eq 0 ]; then
        echo "$OUTFILE and $COPYFILE match (diff)."
    else
        echo "Differences found by diff (exit code $ret)."
    fi
  fi
done


echo
echo "Done"
