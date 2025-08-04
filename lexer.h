#include "automata.h"

// function to check if a string is a keyword
int isKeyword(const char *str) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

// function to check if a string is an operator
int isOperator(const char *str) {
    for (int i = 0; operators[i] != NULL; i++) {
        if (strcmp(str, operators[i]) == 0) {
            return true;
        }
    }
    return false;
}

// function to check if a string is a delimiter
int isDelimiter(const char *str) {
    for (int i = 0; delimiters[i] != NULL; i++) {
        if (strcmp(str, delimiters[i]) == 0) {
            return true;
        }
    }
    return false;
}

// function to check if a token is a string
int isString(const char *str) {
    size_t length = strlen(str);

    // a string must be at least 2 characters long (for enclosing quotes)
    if (length < 2) {
        return false;
    }

    // check if the first and last characters are matching quotes (' or ")
    if ((str[0] == '"' && str[length - 1] == '"') ||
        (str[0] == '\'' && str[length - 1] == '\'')) {
        return true;
    }

    return false; // not a valid string
}

void printToken(TokenType type, char* value){
    switch(type){
        case KEYWORD:
            printf("KEYWORD: %s\n", value);
            break;
        case IDENTIFIER:
            printf("IDENTIFIER: %s\n", value);
            break;
        case NUMBER:
            printf("NUMBER: %s\n", value);
            break;
        case OPERATOR:
            printf("OPERATOR: %s\n", value);
            break;
        case DELIMITER:
            printf("DELIMITER: %s\n", value);
            break;
        case UNKNOWN:
            printf("UNKNOWN: %s\n", value);
            break;
    }
}

// function to create a token
Token createToken(TokenType type, const char *value) {
    Token token;
    token.type = type;
    strcpy(token.value, value);
    printToken(token.type, token.value);
    return token;
}

// initialize the DFA with transition matrix
void initDFA(DFA *dfa, int nStates, int nSymbols, int transitions[MAX_STATES][MAX_ALPHABET], int final[], char symbols[]) {
    dfa->num_states = nStates;
    dfa->num_symbols = nSymbols;
    
    // copy transitions and symbols to DFA
    for (int i = 0; i < dfa->num_states; i++) {
        for (int j = 0; j < dfa->num_symbols; j++) {
            dfa->transition[i][j] = transitions[i][j];
        }
    }

    // add the symbols to the DFA
    for (int j = 0; j < dfa->num_symbols; j++) {
        dfa->symbols[j] = symbols[j];
    }

    // set final states
    for (int i = 0; i < dfa->num_states; i++) {
        dfa->final[i] = final[i];
    }
}

// traverse the automata using states and delta function being the matrix
int traverseDFA(DFA *automata, const char *input){
    // if traverse is completed and current state is a final state, return true
    // else if there is no transition or traverse was not completed return -1

    int currentState = 0;
    int symbolIndex;
    int i = 0;

    while (input[i] != '\0') {
        symbolIndex = -1;
        // search index of current symbol in DFA's alphabet
        for (int j = 0; j < automata->num_symbols; j++) {
            if (input[i] == automata->symbols[j]) {
                symbolIndex = j;
                break;
            }
        }

        if (symbolIndex == -1) {
            return false; // symbol not found
        }

        // go to next state
        currentState = automata->transition[currentState][symbolIndex];
        if (currentState == -1) {
            return false; // invalid transition
        }

        i++;
    }

    // check if the final state is accepted
    int isFinalState = false;
    i = 0;

    while (automata->final[i] != -1) {  // loop until -1 is encountered
        isFinalState = (currentState == automata->final[i]) ? 1 : isFinalState;
        i++;
    }

    return isFinalState ? true : false;
}

// DFA declaration
Token lexicalAnalyzer(
    DFA *keywordsAutomata, DFA *indentifiersAutomata, 
    DFA *numbersAutomata, DFA *operatorsAutomata, 
    DFA *delimitersAutomata, const char *buffer){

    if (traverseDFA(keywordsAutomata, buffer)){
        return createToken(KEYWORD, buffer);
    } else if (traverseDFA(indentifiersAutomata, buffer)){
        return createToken(IDENTIFIER, buffer);
    } else if (traverseDFA(numbersAutomata, buffer)){
        return createToken(NUMBER, buffer);
    } else if (traverseDFA(operatorsAutomata, buffer)){
        return createToken(OPERATOR, buffer);
    } else if (traverseDFA(delimitersAutomata, buffer)){
        return createToken(DELIMITER, buffer);
    } else {
        return createToken(UNKNOWN, buffer);
    }
}

// Lexical analyzer logic
void initLexicalAnalyzer() {
    initDFA(&keywordsAutomata, 23, 13, keywordsDFA, finalStatesKeywords, symbolsKeywords);
    initDFA(&identifiersAutomata, 2, 36, identifiersDFA, finalStatesIdentifiers, symbolsIdentifiers);
    initDFA(&numbersAutomata, 3, 11, numbersDFA, finalStatesNumbers, symbolsNumbers);
    initDFA(&operatorsAutomata, 5, 10, operatorsDFA, finalStatesOperators, symbolsOperators);
    initDFA(&delimitersAutomata, 2, 6, delimitersDFA, finalStatesDelimiters, symbolsDelimiters);
}