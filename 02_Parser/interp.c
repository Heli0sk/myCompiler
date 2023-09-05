// AST 解释器

#include "defs.h"
#include "data.h"
#include "decl.h"

// AST 操作符列表
static char *ASTop[] = { "+", "-", "*", "/" };

// 给定一个AST，解释其中的运算符并返回最终值
int interpretAST(struct ASTnode *n) {
  int leftval, rightval;

  // 递归获取左右子树的值
  if (n->left)
    leftval = interpretAST(n->left);
  if (n->right)
    rightval = interpretAST(n->right);

  if (n->op == A_INTLIT)
    printf("int %d\n", n->intvalue);
  else
    printf("%d %s %d\n", leftval, ASTop[n->op], rightval);

  switch (n->op) {
    case A_ADD:
      return (leftval + rightval);
    case A_SUBTRACT:
      return (leftval - rightval);
    case A_MULTIPLY:
      return (leftval * rightval);
    case A_DIVIDE:
      return (leftval / rightval);
    case A_INTLIT:
      return (n->intvalue);
    default:
      fprintf(stderr, "Unknown AST operator %d\n", n->op);
      exit(1);
  }
}
