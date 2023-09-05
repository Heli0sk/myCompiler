// 所有编译器文件的函数原型

int scan(struct token *t);
struct ASTnode *mkastnode(int op, struct ASTnode *left,
			  struct ASTnode *right, int intvalue);
struct ASTnode *mkastleaf(int op, int intvalue);
struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue);
struct ASTnode *binexpr(void);
int interpretAST(struct ASTnode *n);
