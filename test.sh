#! /bin/bash

# Setting option to exit the script if a command exits with a non-zero status
set -e

TEST_DIR="Tests"

if [[ ! -d "$TEST_DIR" ]]; then
	mkdir $TEST_DIR
fi

echo "Creating ${TEST_DIR}/test.c"
touch ${TEST_DIR}/test.c
cat test > ${TEST_DIR}/test.c

echo "Compiling code"
gcc -o protector *.c -g3

echo "Copying protector to ${TEST_DIR}/"
cp protector ${TEST_DIR}/

cd ${TEST_DIR}

echo "Executing protector test.c"
./protector test.c

echo "Compiling protected_test.c"
gcc -fno-stack-protector -O0 -z execstack protected_test.c -o protected_test -g3
