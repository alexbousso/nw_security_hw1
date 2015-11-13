#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "test_utils.h"


#define BUFFER_SIZE 256;

int AddCanary(const char *filename, const int canary) {
    if (!filename) {
        return -1;
    }

    // Steps:
    //    - Find a function
    //    - Add the canary at the begining and after all variables have been declared

    FILE *input;
    FILE *output;
    char line[BUFFER_SIZE];

    input = fopen(filename, "r");
    if (!input) {
        ERROR("AddCanary - Cannot open the file %s\n", filename);
        return -1;
    }

    output = fopen(getOutputFilename(filename), "w");
    if (!output) {
        ERROR("AddCanary - Cannot open the file %s\n", getOutputFilename(filename));
        fclose(input);
        return -1;
    }

    while (fgets(line, sizeof(line), input)) {
        fprintf(output, line);

        if (isFunctionImplementation(line)) {
            fprintf(output, "int canary = %d;\n", canary);
        }
    }

    fclose(input);
}

static char *getLastWord(const char *line, const char *delimiters) {
    ASSERT(line && delimiters);

    char *word = strtok(line, delimiters);
    char *lastWord;
    while (word) {
        lastWord = word;
        word = strtok(NULL, delimiters);
    }
    return lastWord;
}

// If the filename is /home/user/prog.c it returns /home/user/protected_prog.c
static char *getOutputFilename(const char *filename) {
    char *outputFilename = getLastWord(filename, "/");
    char *outputPath = strtok(filename, outputFilename);
    ASSERT(outputPath);
    outputFilename = strcat("protected_", outputFilename);
    outputFilename = strcat(outputPath, outputFilename);
    return outputFilename;
}

static bool isFunctionImplementation(const char *line) {
    if (!line) {
        ERROR("isFunctionImplementation - Received null pointer");
        return false;
    }

    const char *delimiters = " \t,()"
    word = strtok(line, delimiters);
    if (strcmp(word, "void") == 0) {
        char *lastWord = getLastWord(line, delimiters);

        // Checking that it is a function implementation and not a declaration, that means it ends
        // with { and not with ;
        if (strcmp(lastWord, "{")) {
            return true;
        }
        ASSERT(strcmp(lastWord, ";") == 0);
    }

    return false;
}
