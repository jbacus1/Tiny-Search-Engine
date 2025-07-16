#!/bin/bash
# testing.sh - TSE Querier testing script
#
# The program does the following:
#
# 1. Argument tests
# 2. Query tests on letters-10 directory (since it's a small, predicatable example),
#    using three query files: testquery1, testquery2, testquery3
#    each fed into the querier, with output sent to testing.out
#    (files are made in file and deleted afterward)
# 3. Valgrind test
#
# Usage:
#   make test   (outputs to testing.out)
#   !Assumes letters-10 and letter-10.index exists!
#
# Author: Jacob Bacus
# February 2025

PROGRAM=./querier
PAGEDIR=../data/letters-10
INDEXFILE=../data/indexes/letters-10.index
OUTFILE=testing.out

# clean up before we start
rm -f testquery1 testquery2 testquery3 $OUTFILE

echo "----- Argument Tests -----" | tee -a $OUTFILE

echo
echo "1) No arguments" | tee -a $OUTFILE
$PROGRAM >> $OUTFILE 2>&1

echo
echo "2) Only one arg" | tee -a $OUTFILE
$PROGRAM $PAGEDIR >> $OUTFILE 2>&1

echo
echo "3) Bad directory" | tee -a $OUTFILE
$PROGRAM not-a-dir $INDEXFILE >> $OUTFILE 2>&1

echo
echo "4) Bad index file" | tee -a $OUTFILE
$PROGRAM $PAGEDIR not-a-index >> $OUTFILE 2>&1


echo "------- Querier Tests -------" >> $OUTFILE

# Create 3 files to run as inputs on querier
# (based on the contents of letters-10

cat > testquery1 <<EOF
# testquery1 - Basic queries
the
the for
for home and page
algorithm
or
for or algorithm
graph for page
EOF

cat > testquery2 <<EOF
# testquery2 - Some 'and'/'or' combos, plus errors
the home
the or home
and
or
huffman computational
fast and first
EOF

cat > testquery3 <<EOF
# testquery3 - Mixed queries
for page or the
the graph or traversal
fast fourier or transform
home and depth
this or coding
search first
EOF

# Run the querier with query files
echo "Running testquery1:" >> $OUTFILE
$PROGRAM $PAGEDIR $INDEXFILE < testquery1 >> $OUTFILE 2>&1

echo "" >> $OUTFILE
echo "Running testquery2:" >> $OUTFILE
$PROGRAM $PAGEDIR $INDEXFILE < testquery2 >> $OUTFILE 2>&1

echo "" >> $OUTFILE
echo "Running testquery3:" >> $OUTFILE
$PROGRAM $PAGEDIR $INDEXFILE < testquery3 >> $OUTFILE 2>&1



# Simple valgrind test using testquery1
echo "" >> $OUTFILE
echo "------- Valgrind Test ------" >> $OUTFILE
valgrind --leak-check=full --show-leak-kinds=all $PROGRAM $PAGEDIR $INDEXFILE < testquery1 >> $OUTFILE 2>&1


# Cleanup
rm -f testquery1 testquery2 testquery3

echo "" >> $OUTFILE
echo "Done" >> $OUTFILE
