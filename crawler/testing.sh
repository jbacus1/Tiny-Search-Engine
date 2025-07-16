#!/bin/bash
# testing.sh - test script for Crawler
#
# Call crawler several times with a variety of arguments:
#   1. Argument tests.
#   2. Valgrind test
#   3. Tests on the three CS50 websites:
#        - letters at depths 0,1,2,10
#        - toscrape at depths 0,1 (short for sake of time)
#        - wikipedia at depths 0,1 (short for sake of time)
#
# Usage:
#   bash -v testing.sh
#     (can put output in testing.sh)
#
# Author: Jacob Bacus
# Date: Feburary 2025

PROGRAM=./crawler
DATADIR=../data

# 1. Argument tests

echo
echo "-----ARGUMENT TESTS-----"
echo "1. No args:"
$PROGRAM

echo
echo "2. Fewer than 3 args:"
$PROGRAM http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $DATADIR/letters-err

echo
echo "3. Non-cs50 seedURL:"
$PROGRAM https://en.wikipedia.org/wiki/Algorithm $DATADIR/test-err 1

echo
echo "4. SeedURL not valid:"
$PROGRAM http://thiswillnot.normalize/ $DATADIR/test-err 1

echo
echo "5. pageDirectory does not exist:"
$PROGRAM http://cs50tse.cs.dartmouth.edu/tse/letters/index.html /this/does/not/exist 1

echo
echo "6. maxDepth is -1:"
$PROGRAM http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $DATADIR/test-err -1

echo
echo "7. maxDepth is 99:"
$PROGRAM http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $DATADIR/test-err 99


# 2. Valgrind test on a moderate site

echo
echo "-----VALGRIND TEST (toscrape at depth=1)-----"
VALGRIND_LOG=valgrind.out
# Create (or clean) data directory
if [ ! -d "$DATADIR/toscrape-val" ]; then
  mkdir -p "$DATADIR/toscrape-val"
else
  rm -rf "$DATADIR/toscrape-val/*"
fi

valgrind --leak-check=full --show-leak-kinds=all --log-file=$VALGRIND_LOG \
  $PROGRAM http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html $DATADIR/toscrape-val 1

echo "Valgrind report in $VALGRIND_LOG"
echo "Check $DATADIR/toscrape-val directory for fetched pages."


# 3. Crawler Tests

# Run on letters, toscrape, wikipedia
# at multiple depths.  (Will take a while)

echo
echo "-----CRAWLER TESTS-----"

# letters
echo "*letters site*"
for depth in 0 1 2 10
do
  dir="$DATADIR/letters-$depth"
  echo
  echo "Creating directory $dir"
  mkdir -p "$dir"
  rm -rf "$dir/*"
  
  echo "Running: $PROGRAM http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $dir $depth"
  $PROGRAM http://cs50tse.cs.dartmouth.edu/tse/letters/index.html $dir $depth
done

# toscrape
echo
echo "*toscrape site*"
for depth in 0 1
do
  dir="$DATADIR/toscrape-$depth"
  echo
  echo "Creating directory $dir"
  mkdir -p "$dir"
  rm -rf "$dir/*"

  echo "Running: $PROGRAM http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html $dir $depth"
  $PROGRAM http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html $dir $depth
done

# wikipedia
echo
echo "*wikipedia site*"
for depth in 0 1
do
  dir="$DATADIR/wikipedia-$depth"
  echo
  echo "Creating directory $dir"
  mkdir -p "$dir"
  rm -rf "$dir/*"

  echo "Running: $PROGRAM http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html $dir $depth"
  $PROGRAM http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html $dir $depth
done

echo
echo "TESTING COMPLETE!"
echo "Check the directories in $DATADIR to see fetched pages."
