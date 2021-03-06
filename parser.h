#ifndef PARSER_H_
#define PARSER_H_

/**
 * Adds the line
 *    int canary = <canary>;\n
 * after every function declaration.
 * Returns -1 if there is an error, otherwise returns 0.
 */
int CreateProtectedFile(const char *filename);

#endif /* PARSER_H_ */
