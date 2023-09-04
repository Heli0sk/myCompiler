#include "defs.h"
#include "data.h"
#include "decl.h"


// Get the next character from the input file.
static int next(void) {
    int c;
    if (Putback){
        c = Putback;
        Putback = 0;
        return c;
    }
    c = fgetc(Infile);
    if ('\n' == c) Line++;  // line count ++
    return c;
}

// Put back an unwanted character
static void set_putback(int c) {
    Putback = c;
}

// Skip past input that we don't need to deal with, 
// i.e. whitespace, newlines. Return the first
// character we do need to deal with.
static int skip(void) {
    int c = next();
    while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c) {
        c = next();
    }
    return c;
}

// Scan and return an integer literal
// value from the input file.
static int scanint(int c) {
    int val = 0;
    val = val*10 + c - 48;
    c = next();
    while (isdigit(c)){
       val = val*10 + c - 48;
       c = next();
    }
    set_putback(c);
    return val;
}

// Scan and return the next token found in the input.
// Return 1 if token valid, 0 if no tokens left.
int scan(struct token *t) {
    int c = skip();
    switch (c)
    {
      case '+':
        t->token = T_PLUS;
        break;
      case '-':
        t->token = T_MINUS;
        break;
      case '*':
        t->token = T_STAR;
        break;
      case '/':
        t->token = T_SLASH;
        break;
      case EOF: // End of File
        return 0;
      default:  // digit or unrecongnised char
        if (isdigit(c)){
          t->intvalue = scanint(c);
          t->token = T_INTLIT;
          break;
        }
        printf("Unrecognised character %c on line %d\n", c, Line);
        exit(1);
    }
    return 1;
}
