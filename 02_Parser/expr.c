// 表达式解析

#include "defs.h"
#include "data.h"
#include "decl.h"

// 将扫描到的token转换为AST节点操作值
int arithop(int tok) {
  switch (tok) {
    case T_PLUS:
      return (A_ADD);
    case T_MINUS:
      return (A_SUBTRACT);
    case T_STAR:
      return (A_MULTIPLY);
    case T_SLASH:
      return (A_DIVIDE);
    default:
      fprintf(stderr, "unknown token %d in arithop() on line %d\n", tok, Line);
      exit(1);
  }
}

/*
    检查下一个标记是否是整数文字
    对于INTLIT token，创建一个叶子AST节点并扫描下一个token
    对于任何其他令牌类型，报语法错误
*/
static struct ASTnode *primary(void) {
  struct ASTnode *n;
  // 这里的Token是一个全局变量(data.h)，保存着扫描到的最新的值
  switch (Token.token) {
    case T_INTLIT:
      n = mkastleaf(A_INTLIT, Token.intvalue);
      scan(&Token);
      return (n);
    default:
      fprintf(stderr, "syntax error on line %d\n", Line);
      exit(1);
  }
}

// 解析输入表达式生成AST的方法
struct ASTnode *binexpr(void) {
    struct ASTnode *n, *left, *right;
    int nodetype;

    // 获取左子树的整数，同时获取下一个token
    left = primary();

    // 如果所有token都扫描完毕，返回左子树
    if (Token.token == T_EOF)
        return (left);

    // 将token转换为AST节点
    nodetype = arithop(Token.token);

    // 扫描下一个token
    scan(&Token);

    // 递归获取右子树
    right = binexpr();

    // 合并左右子树
    n = mkastnode(nodetype, left, right, 0);
    return (n);
}

