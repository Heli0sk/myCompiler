#include "defs.h"
#include "data.h"
#include "decl.h"

// 抽象语法树相关函数

// 创建一个AST节点并返回指向节点的指针
struct ASTnode *mkastnode(int op, struct ASTnode *left, struct ASTnode *right, int intvalue)
{
    struct ASTnode *n;
    n = (struct ASTnode *)malloc(sizeof(struct ASTnode));
    if (n == NULL){
        fprintf(stderr, "Unable to malloc a ASTnode in mkastnode()\n");
        exit(1);
    }
    n->op = op;
    n->left = left;
    n->right = right;
    n->intvalue = intvalue;
    
    return n;
}

// 创建叶子结点
struct ASTnode *mkastleaf(int op, int intvalue) {
    return (mkastnode(op, NULL, NULL, intvalue));
}

// 创建一个一元AST节点：只有一个子节点
struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue) {
    return (mkastnode(op, left, NULL, intvalue));
}