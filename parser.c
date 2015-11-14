#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "test_utils.h"

#define BUFFER_SIZE 256
#define PROTECTED_PREFIX "protected_"
#define URANDOM_FILE "urandom.txt"
#define CANARY_VARIABLE "__stack_canary_protection"

#define NUM_OF_TYPES 6
const char *types[NUM_OF_TYPES] = {
    "short", "long", "char", "int", "float", "double"
};

/**************************************************************************************************
 * Static function declarations
 *************************************************************************************************/

/**
 * 
 */
static bool isVariableDeclaration(const char *line);

/**
 * 
 */
static char *getLastWord(const char *line, const char *delimiters);

/**
 * If the filename is /home/user/prog.c it returns /home/user/protected_prog.c
 */
static char *getOutputFilename(const char *filename);

/**
 * 
 */
static bool isFunctionImplementation(const char *line);

/**
 * 
 */
static void addCanary(FILE *input, FILE *output);

/**
 * 
 */
static bool isReturnDetected(const char *line);

/**
 * 
 */
static void addCheckCanaryCode(FILE *output);

/**************************************************************************************************
 * Implementation
 *************************************************************************************************/

int CreateProtectedFile(const char *filename) {
    if (!filename) {
        ERROR("CreateProtectedFile - Null argument received");
        exit(1);
    }
    
    char line[BUFFER_SIZE];

    FILE *input = fopen(filename, "r");
    if (!input) {
        ERROR("CreateProtectedFile - Cannot open the file %s\n", filename);
        exit(1);
    }

    char *outputFilename = getOutputFilename(filename);
    FILE *output = fopen(outputFilename, "w");
    if (!output) {
        ERROR("CreateProtectedFile - Cannot open the file %s\n", getOutputFilename(filename));
        free(outputFilename);
        fclose(input);
        exit(1);
    }
    free(outputFilename);
    outputFilename = 0;

    FILE *urandom = fopen(URANDOM_FILE, "r");
    if (!urandom) {
        ERROR("CreateProtectedFile - Cannot open the file %s\n", URANDOM_FILE);
        fclose(input);
        fclose(output);
        exit(1);
    }

    // Appending urandom code
    while (fgets(line, sizeof(line), urandom)) {
        fprintf(output, "%s", line);
    }
    fprintf(output, "\n");

    addCanary(input, output);

    fclose(input);
    fclose(output);
    fclose(urandom);
    return 0;
}

static void addCanary(FILE *input, FILE *output) {
    ASSERT(input && output);

    char line[BUFFER_SIZE];

    bool isLastVariableDeclaration = false;
    bool inFunction = false;

    // Holds the difference between open and closed curly brackets
    int curlyBrackets = 0;

    while (fgets(line, sizeof(line), input)) {
        if (isLastVariableDeclaration == true && !isVariableDeclaration(line)) {
            isLastVariableDeclaration = false;
            fprintf(output, "\tint canary2 = %s;\n", CANARY_VARIABLE);
        }

        if (strstr(line, "}")) {
            curlyBrackets--;
        }

        if (strstr(line, "{")) {
            curlyBrackets++;
        }

        if (isVariableDeclaration(line)) {
            isLastVariableDeclaration = true;
        }

        if (curlyBrackets == 0 && inFunction) {
            addCheckCanaryCode(output);
            inFunction = false;
        }

        if (isReturnDetected(line)) {
            addCheckCanaryCode(output);
        }

        fprintf(output, "%s", line);

        if (isFunctionImplementation(line)) {
            fprintf(output, "\tint %s = urandom();\n", CANARY_VARIABLE);
            fprintf(output, "\tint canary1 = %s;\n", CANARY_VARIABLE);
            inFunction = true;
        }
        
        ASSERT(curlyBrackets >= 0);
    }
}

static bool isVariableDeclaration(const char * line) {
    char *delimiters = " \t";

    char *_line = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    if (!_line) {
        ERROR("isVariableDeclaration - out of memory");
        exit(1);
    }
    strcpy(_line, line);

    char *word = strtok(_line, delimiters);
    int i;

    ASSERT(word);
    for (i = 0; i < NUM_OF_TYPES; i++) {
        if (strcmp(types[i], word) == 0) {
            free(_line);
            return true;
        }
    }

    free(_line);
    return false;
}

static char *getLastWord(const char *line, const char *delimiters) {
    ASSERT(line && delimiters);

    char *_line = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    if (!_line) {
        ERROR("getLastWord - out of memory");
        exit(1);
    }
    strcpy(_line, line);

    char *word = strtok(_line, delimiters);
    char *lastWord;
    while (word) {
        lastWord = word;
        word = strtok(NULL, delimiters);
    }

    free(_line);
    return lastWord;
}

static char *getOutputFilename(const char *filename) {
    char *outputFilename = (char *) malloc(sizeof(char) * 
        (strlen(PROTECTED_PREFIX) + strlen(filename) + 1));
    if (!outputFilename) {
        ERROR("getOutputFilename - out of memory");
        exit(1);
    }
    strcpy(outputFilename, PROTECTED_PREFIX);
    strcat(outputFilename, filename);
    return outputFilename;
}

static bool isFunctionImplementation(const char *line) {
    if (!line) {
        ERROR("isFunctionImplementation - Received null pointer");
        return false;
    }

    char *_line = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    if (!_line) {
        ERROR("isFunctionImplementation - out of memory");
        exit(1);
    }
    strcpy(_line, line);

    const char *delimiters = " \t,()";
    char *word = strtok(_line, delimiters);
    if (strcmp(word, "void") == 0) {
        char *lastWord = getLastWord(line, delimiters);

        // Checking that it is a function implementation and not a declaration, that means it ends
        // with { and not with ;
        if (strstr(lastWord, "{")) {
            free(_line);
            return true;
        }
        ASSERT(strstr(lastWord, ";"));
    }

    free(_line);
    return false;
}

static bool isReturnDetected(const char *line) {
    ASSERT(line);
    if (strstr(line, "return")) {
        return true;
    }
    return false;
}

static void addCheckCanaryCode(FILE *output) {
    ASSERT(output);
    fprintf(output, "\tif (canary1 != %s || canary2 != %s) {\n", CANARY_VARIABLE, CANARY_VARIABLE);
    fprintf(output, "\t\tprintf(\"Alert! Buffer Overflow detected.\");\n");
    fprintf(output, "\t\texit(1);\n");
    fprintf(output, "\t}\n");
}
