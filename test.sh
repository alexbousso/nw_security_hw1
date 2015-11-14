#! /bin/bash

TEST_DIR="Tests"

if [[ ! -d "$TEST_DIR" ]]; then
	mkdir $TEST_DIR
fi

touch ${TEST_DIR}/test.c
cat test > ${TEST_DIR}/test.c

gcc -o protector *.c -g3
cp protector test.c

cd ${TEST_DIR}
protector test.c

gcc -fno-stack-protector -O0 -z execstack protected_test.c -o protected_test
