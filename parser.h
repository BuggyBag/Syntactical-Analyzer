#include "lexer.h"

// grammar rules for context-free grammar
void parseFunction();
void parseParams();
void parseStmts();
void parseStmt();
void parseAssignment();
void parseCall();
void parseArgs();
void parseExpr();
void parseOperation();
void parseTerm();
void parseOp();
void parseString();
void parseIdentifier();

Token currentToken;
const char *sourcePtr;

// function to report syntax errors
void syntaxError(const char *message) {
    printf("Syntax Error: %s at token '%s'\n", message, currentToken.value);
    exit(1);
}

// function to fetch the next token from the source
Token getNextToken() {
    static char buffer[BUFFER_SIZE];
    int bufferIndex = 0;

    // skip whitespace
    while (isspace(*sourcePtr)) {
        sourcePtr++;
    }

    // read characters into the buffer until whitespace or end of token
    while (*sourcePtr != '\0' && !isspace(*sourcePtr)) {
        buffer[bufferIndex++] = *sourcePtr++;
    }
    buffer[bufferIndex] = '\0';

    return lexicalAnalyzer(
        &keywordsAutomata, &identifiersAutomata, &numbersAutomata,
        &operatorsAutomata, &delimitersAutomata, buffer);
}

// match function to check the token is as expected
void match(TokenType expected, const char *expectedValue) {
    if (currentToken.type == expected &&
        (expectedValue == NULL || strcmp(currentToken.value, expectedValue) == 0)) {
        currentToken = getNextToken();
    } else {
        printf("Syntax error: expected '%s', but got '%s'\n",
               expectedValue ? expectedValue : "token",
               currentToken.value);
        exit(1);
    }
}

Token peekNextToken() {
    static const char *savedSourcePtr = NULL; // save the source pointer state
    static Token savedToken;                 // save the peeked token

    // if the saved state exists, return it
    if (savedSourcePtr) {
        const char *tempSourcePtr = savedSourcePtr; // backup saved pointer
        savedSourcePtr = NULL; // reset the saved state
        return savedToken; // return the saved token
    }

    // save the current state and fetch the next token
    savedSourcePtr = sourcePtr;       // save the current source pointer
    savedToken = getNextToken();      // fetch the next token but don't commit
    sourcePtr = savedSourcePtr;       // restore the source pointer
    savedSourcePtr = NULL;            // reset the saved state for subsequent calls
    return savedToken;
}

// grammar rule implementations
void parseFunction() {
    if (currentToken.type == IDENTIFIER) {
        parseIdentifier();
        match(DELIMITER, "(");
        parseParams();
        match(DELIMITER, ")");
        match(DELIMITER, "{");
        parseStmts();
        match(DELIMITER, "}");
    } else {
        printf("Syntax error: expected function definition.\n");
        exit(1);
    }
}

// function to parse parameters
void parseParams() {
    // if there are paramters in the function
    if (currentToken.type == IDENTIFIER) {
        match(IDENTIFIER, NULL); // match an identifier as a parameter
        
        // Check if there are additional parameters
        while (currentToken.type == DELIMITER && currentToken.value == ",") {
            match(DELIMITER, ",");    // match the "," separating parameters
            match(IDENTIFIER, NULL); // match the next parameter
        }
    }
}

// function to parse statements
void parseStmts() {
    // loop until we reach the closing brace or EOF
    while (currentToken.type != DELIMITER || strcmp(currentToken.value, "}") != 0) {
        if (currentToken.type == EOF) {
            syntaxError("Unexpected EOF while parsing statements.");
        }
        
        parseStmt(); // parse each statement
        match(DELIMITER, ";"); // match the semicolon after the statement, but allow for optional whitespace
    }
}

// function to parse a statement
void parseStmt() {
    if (currentToken.type == IDENTIFIER) {
        Token nextToken = peekNextToken();

        if (nextToken.type == OPERATOR) {
            parseAssignment();
        } else if (nextToken.type == DELIMITER) {
            parseCall();
        } else {
            syntaxError("Invalid statement.");
        }
    } else {
        syntaxError("Statement expected.");
    }
}

// function to parse an assignment (identifier = expr)
void parseAssignment() {
    match(IDENTIFIER, NULL); // match the identifier
    match(OPERATOR, "=");     // match the assignment operator
    parseExpr();     
}

// function to parse a function call (identifier ( args ))
void parseCall() {
    match(IDENTIFIER, NULL);  // match the identifier (function name)
    match(DELIMITER, "(");      // match the opening parenthesis
    parseArgs();        // parse the arguments
    match(DELIMITER, ")");      // match the closing parenthesis
}

// function to parse arguments (expr | expr, args | none)
void parseArgs() {
    if (currentToken.type == IDENTIFIER || currentToken.type == NUMBER || isString(currentToken.value)) {
        parseExpr();      // parse an expression for an argument
        
        // check for additional arguments
        while (currentToken.type == DELIMITER && currentToken.value == ",") {
            match(DELIMITER, ",");
            parseExpr(); // parse next argument
        }
    }
}

// function to parse an expression
void parseExpr() {
    if (currentToken.type == IDENTIFIER) {
        Token nextToken = peekNextToken();
        if (nextToken.type == OPERATOR) {
            parseOperation();
        } else {
            match(IDENTIFIER, NULL);
        }
    } else if (currentToken.type == NUMBER || isString(currentToken.value)) {
        match(currentToken.type, NULL);
    } else {
        syntaxError("Invalid expression.");
    }
}

// function to parse operations
void parseOperation() {
    parseTerm(); // parse the first term
    while (currentToken.type == OPERATOR) { // check for operator
        parseOp();   // parse the operator
        parseTerm(); // parse the next term
    }
}

// function to parse a term
void parseTerm() {
    if (currentToken.type == IDENTIFIER || currentToken.type == NUMBER) {
        match(currentToken.type, NULL);
    } else {
        syntaxError("Invalid term.");
    }
}

// function to parse a string or char
void parseString() {
    if (isString(currentToken.value)) {
        match(currentToken.type, NULL); // match the string token
    } else {
        syntaxError("String expected.");
    }
}

// function to parse an identifier (start of an identifier letter)
void parseIdentifier() {
    if (currentToken.type == IDENTIFIER) {
        match(IDENTIFIER, NULL); // match the identifier
    } else {
        syntaxError("Identifier expected.");
    }
}

// function to parse an operator
void parseOp() {
    if (currentToken.type == OPERATOR) {
        match(OPERATOR, NULL); // match the operator token
    } else {
        syntaxError("Operator expected.");
    }
}
