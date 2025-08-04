#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 100
#define MAX_STATES 25
#define MAX_ALPHABET 40
#define false 0
#define true 1

// define token types
typedef enum {
    KEYWORD, IDENTIFIER, NUMBER, OPERATOR, DELIMITER, UNKNOWN
} TokenType;

// structure to represent a token
typedef struct {
    TokenType type;
    char value[100];
} Token;

// define a type for a DFA
typedef struct {
    char symbols[MAX_ALPHABET];
    int transition[MAX_STATES][MAX_ALPHABET]; // transition matrix
    int final[MAX_STATES];                    // final states
    int num_states;                           // state numbers
    int num_symbols;                          // número de símbolos en el alfabeto
} DFA;

// list of keywords
const char *keywords[] = {"if", "else", "while", "return", "for", "struct", NULL};

// list of operators
const char *operators[] = {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "++", "--", "%", NULL};

// list of delimiters
const char *delimiters[] = {"(", ")", "{", "}", ";", "\"", ",", NULL};

// define transition matrix for keywords 
int keywordsDFA[MAX_STATES][MAX_ALPHABET] = {
        //c  e  f  h  i  l  n  o  r  s  t  u  w
        {-1, 3,15,-1, 1,-1,-1,-1,10,18,-1,-1, 7}, // Estado 0
        {-1,-1, 2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 1
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 2
        {-1,-1,-1,-1,-1, 4,-1,-1,-1,-1,-1,-1,-1}, // Estado 3
        {-1,-1,-1,-1,-1,-1,-1,-1,-1, 5,-1,-1,-1}, // Estado 4
        {-1, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 5
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 6
        {-1,-1,-1, 8,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 7
        {-1,-1,-1,-1, 9,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 8
        {-1,-1,-1,-1,-1, 5,-1,-1,-1,-1,-1,-1,-1}, // Estado 9
        {-1,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 10
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,12,-1,-1}, // Estado 11
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,13,-1}, // Estado 12
        {-1,-1,-1,-1,-1,-1,-1,-1,14,-1,-1,-1,-1}, // Estado 13
        {-1,-1,-1,-1,-1,-1, 6,-1,-1,-1,-1,-1,-1}, // Estado 14
        {-1,-1,-1,-1,-1,-1,-1,16,-1,-1,-1,-1,-1}, // Estado 15
        {-1,-1,-1,-1,-1,-1,-1,-1,17,-1,-1,-1,-1}, // Estado 16
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 17
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,19,-1,-1}, // Estado 18
        {-1,-1,-1,-1,-1,-1,-1,-1,20,-1,-1,-1,-1}, // Estado 19
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,21,-1}, // Estado 20
        {22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 21
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,17,-1,-1}  // Estado 22
    };

int finalStatesKeywords[] = {2, 6, 17}; // final states
char symbolsKeywords[] = {'c', 'e', 'f', 'h', 'i', 'l', 'n', 'o', 'r', 's', 't', 'u', 'w'}; // DFA symbols
DFA keywordsAutomata;
    
// define transition matrix for identifiers 
int identifiersDFA[MAX_STATES][MAX_ALPHABET] = {
        //0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z 
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Estado 0
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  // Estado 1
    };

int finalStatesIdentifiers[] = {1}; // final states
char symbolsIdentifiers[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
                                'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 
                                'u', 'v', 'w', 'x', 'y', 'z'}; // DFA symbols
DFA identifiersAutomata;
        
// define transition matrix for numbers 
int numbersDFA[MAX_STATES][MAX_ALPHABET] = {
        //0 1  2  3  4  5  6  7  8  9  .
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1}, // Estado 0
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  2}, // Estado 1
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, -1}  // Estado 2
    };
    
int finalStatesNumbers[] = {1, 2}; // final states
char symbolsNumbers[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'}; // DFA symbols
DFA numbersAutomata;
    
// define transition matrix for operators 
int operatorsDFA[MAX_STATES][MAX_ALPHABET] = {
        //+  -  *  /  %  &  <  >  =  !  
        { 1, 1, 2, 2, 2, 3, 4, 4, 4, 4}, // Estado 0
        { 2, 2,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 1
        {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // Estado 2
        {-1,-1,-1,-1,-1, 2,-1,-1,-1,-1}, // Estado 3
        {-1,-1,-1,-1,-1,-1,-1,-1, 4,-1}  // Estado 4
    };

int finalStatesOperators[] = {1, 2, 4}; // final states
char symbolsOperators[] = {'+', '-', '*', '/', '%', '&', '<', '>', '=', '!'}; // DFA symbols
DFA operatorsAutomata;
   
// define transition matrix for delimiters 
int delimitersDFA[MAX_STATES][MAX_ALPHABET] = {
        // ( ) {  }  "  ;
        {1, 1, 1, 1, 1, 1}, // Estado 0
        {0, 0, 0, 0, 0, 0}  // Estado 1
    };

int finalStatesDelimiters[] = {1}; // final states
char symbolsDelimiters[] = {'(', ')', '{', '}', '"', ';'}; // DFA symbols
DFA delimitersAutomata;