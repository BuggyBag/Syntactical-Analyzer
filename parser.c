#include "parser.h"

// main parser function
void parse(const char *source) {
    sourcePtr = source; // initialize source pointer
    currentToken = getNextToken();
    parseFunction();
    if (!EOF) {
        printf("Syntax error: unexpected tokens after end of input.\n");
    } else {
        printf("Parsing completed successfully.\n");
    }
}

// main function
int main(int argc, char *argv[]) {
    // open the source code file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }

    // get file size and read contents
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char *sourceCode = (char *)malloc(fileSize + 1);
    fread(sourceCode, 1, fileSize, file);
    sourceCode[fileSize] = '\0';
    fclose(file);

    // start lexical analyzer and parsing
    initLexicalAnalyzer();
    printf("Starting parser...\n");
    parse(sourceCode);

    free(sourceCode);
    return 0;
}