#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "test_utils.h"
#include <stdbool.h>

#define BUFFER_SIZE 256

#define NUM_OF_TYPES 6
const char *types[NUM_OF_TYPES] = {
    "short", "long", "char", "int", "float", "double"
}

/**************************************************************************************************
 * Static function declarations
 *************************************************************************************************/

/**
 * 
 */
bool isVariableDeclaration(const char *line);

/**
 * 
 */
char *getLastWord(const char *line, const char *delimiters);

/**
 * If the filename is /home/user/prog.c it returns /home/user/protected_prog.c
 */
char *getOutputFilename(const char *filename);

/**
 * 
 */
bool isFunctionImplementation(const char *line);


/**************************************************************************************************
 * Implemetation
 *************************************************************************************************/

int AddCanary(const char *filename, const int canary) {
    if (!filename) {
        return -1;
    }

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

    bool isLastVariableDeclaration = false;
    bool inFunction = false;

    // Holds the difference between open and closed curly brackets
    int curlyBrackets = 0;

    while (fgets(line, sizeof(line), input)) {
        if (isLastVariableDeclaration == true) {
            fprintf(output, "int canary = %d;\n", canary);
        }

        fprintf(output, line);

        if (isFunctionImplementation(line)) {
            fprintf(output, "int canary = %d;\n", canary);
            inFunction = true;
            brackets++;
            isLastVariableDeclaration = false;
        }

        if (strcmp(strtok(line, " \t"), "}") == 0) {
            curlyBrackets--;
        }

        if (curlyBrackets == 0) {
            inFunction = false;
        }

        ASSERT(curlyBrackets >= 0);
    }

    fclose(input);
    fclose(output);
}

static bool isVariableDeclaration(const char *line) {
    char *delimiters = " \t";
    char *word = strtok(line, delimiters);
    
    ASSERT(word);
    for (int i = 0; i < NUM_OF_TYPES; i++) {
        if (strcmp(types[i], word) == 0) {
            return true;
        }
    }

    return false;
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

static char *getOutputFilename(const char *filename) {
    char *outputFilename = getLastWord(filename, "/");
    // char *outputPath = strtok(filename, outputFilename);
    // ASSERT(outputPath);
    outputFilename = strcat("protected_", outputFilename);
    // outputFilename = strcat(outputPath, outputFilename);
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
