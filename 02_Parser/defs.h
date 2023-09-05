#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Structure and enum definitions

// Token types
enum {
  T_EOF, T_PLUS, T_MINUS, T_STAR, T_SLASH, T_INTLIT
};

// Token structure
struct token {
  int token;
  int intvalue;
};

// AST node types
enum {
  A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT
};

// Abstract Syntax Tree structure
struct ASTnode {
  int op;				        // 操作符
  struct ASTnode *left;	// 左右子树
  struct ASTnode *right;
  int intvalue;				  // 整数值
};
