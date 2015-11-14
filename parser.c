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
static void appendUrandom(FILE *output);

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

    appendUrandom(output);
    fprintf(output, "\n");

    addCanary(input, output);

    fclose(input);
    fclose(output);
    return 0;
}

static void appendUrandom(FILE *output) {
#define APPEND(__str) fprintf(output, "%s\n", (__str))
    APPEND("#include <stdio.h>");
    APPEND("#include <stdlib.h>");
    APPEND("");
    APPEND("int urandom() {");
    APPEND("#ifdef __unix__");
    APPEND("\tint var;");
    APPEND("\tFILE *fd = fopen(\"/dev/urandom\", \"r\");");
    APPEND("\tfread(&var, sizeof(int), 1, fd);");
    APPEND("\tfclose(fd);");
    APPEND("\treturn var;");
    APPEND("#else");
    APPEND("\treturn 4;");
    APPEND("#endif");
    APPEND("}");
#undef APPEND
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

        if (isVariableDeclaration(line) || isFunctionImplementation(line)) {
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
    char *lastWord = malloc(sizeof(char) * BUFFER_SIZE);
    while (word) {
        strcpy(lastWord, word);
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
    if (!word) {
        ERROR("isFunctionImplementation - word is null");
        return false;
    }
    if (strcmp(word, "void") == 0) {
        char *lastWord = getLastWord(line, delimiters);

        // Checking that it is a function implementation and not a declaration, that means it ends
        // with { and not with ;
        if (strstr(lastWord, "{")) {
            free(_line);
            free(lastWord);
            return true;
        }
        ASSERT(strstr(lastWord, ";"));
        free(lastWord);
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
