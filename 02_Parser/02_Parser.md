# 02_Parser

# 02-解析器

解析器的主要功能分为两个部分：

- 识别输入的语法元素生成AST（Abstract Syntax Trees）并确保输入符合语法规则
- 解析AST并计算表达式的值

在开始代码编写之前，请先了解本节最重要的的两个知识点。

**抽象语法树（AST）：[https://blog.csdn.net/lockhou/article/details/109700312](https://blog.csdn.net/lockhou/article/details/109700312)**

**巴科斯范式（BNF）：部分语法**

```c
::=     //翻译成人话就是：“定义为”
<A>     //A为必选项
“A”     //A是一个术语，不用翻译
'A'     //A是一个术语，不用翻译
[A]     //A是可选项
{A}     //A是重复项，可出现任意次数，包括0次
A*      //A是重复项，可出现任意次数，包括0次
A+      //A可出现1次或多次
(A B)   //A和B被组合在一起
A|B     //A、B是并列选项，只能选一个
```

我们需要识别的元素包含四个基本的数学运算符`+`，`-`,`*`,`/`与十进制整数共五个语法元素。那么首先让我们为我们的解析器将识别的语言定义一个语法。我么这里采用BNF描述：

```bnf
expression: number
          | expression '*' expression
          | expression '/' expression
          | expression '+' expression
          | expression '-' expression
          ;

number:  T_INTLIT
         ;
```

我们都知道BNF定义的语法是递归定义的，那么我们也需要一个递归函数去解析输入的表达式。在我们现有的语法元素可以构成的表达式中第一个语法元素始终为数字，否则就是语法错误。其后可能是一个运算符，或者只有一个数字。那么我们可以用如下伪代码表示我们的递归解析函数：

```c
function expression() {
  Scan and check the first token is a number. Error if it's not
  Get the next token
  If we have reached the end of the input, return, i.e. base case

  Otherwise, call expression()
}
```

让我们来模拟一次此函数的运行，输入为`2 + 3 - 5 T_EOF`其中`T_EOF` 是反映输入结束的标记。

```c
expression0:
  Scan in the 2, it's a number
  Get next token, +, which isn't T_EOF
  Call expression()

    expression1:
      Scan in the 3, it's a number
      Get next token, -, which isn't T_EOF
      Call expression()

        expression2:
          Scan in the 5, it's a number
          Get next token, T_EOF, so return from expression2

      return from expression1
  return from expression0
```

这就是一个简单的解析器，解析器的工作是识别输入，对任何语法错误发出警告

## 抽象语法树 Abstract Syntax Trees，AST

为了进行语义分析，我们需要代码来解释识别的输入，或者将其转换为另一种格式，例如汇编代码。在这一部分，我们将为输入构建一个解释器。但要实现这一目标，我们首先要将输入转换为抽象语法树。

抽象语法树的介绍：[Level Up One's Parsing Game With ASTs](https://medium.com/basecs/leveling-up-ones-parsing-game-with-asts-d7a6fc2400ff)

本章节抽象语法树的节点结构定义如下：

```c
// defs.h
// AST node types
enum {
  A_ADD, A_SUBTRACT, A_MULTIPLY, A_DIVIDE, A_INTLIT
};

// Abstract Syntax Tree structure
struct ASTnode {
  int op;                               // "Operation" to be performed on this tree
  struct ASTnode *left;                 // Left and right child trees
  struct ASTnode *right;
  int intvalue;                         // For A_INTLIT, the integer value
};
```

节点元素`op`表示该节点的类型，当`op`的值为`A_ADD`、`A_SUBTRACT`等运算符时，该节点具有左右两颗子树，我们将使用`op`代表的运算符对左右两棵子树的值做计算；当`op`的值为`A_INTLIT`时，代表该节点是整数值，是叶节点，节点元素`intvalue`存储着该整数的值。

### 构建AST节点和树

`tree.c` 中的代码具有构建 AST 的功能。函数`mkastnode()`生成一个节点并返回指向节点的指针：

```c
// tree.c
// Build and return a generic AST node
struct ASTnode *mkastnode(int op, struct ASTnode *left,
                          struct ASTnode *right, int intvalue) {
  struct ASTnode *n;

  // Malloc a new ASTnode
  n = (struct ASTnode *) malloc(sizeof(struct ASTnode));
  if (n == NULL) {
    fprintf(stderr, "Unable to malloc in mkastnode()\n");
    exit(1);
  }
  // Copy in the field values and return it
  n->op = op;
  n->left = left;
  n->right = right;
  n->intvalue = intvalue;
  return (n);
}
```

我们对其进一步封装出两个常用的函数，分别用来创建左子树与叶节点：

```c
// Make an AST leaf node
struct ASTnode *mkastleaf(int op, int intvalue) {
  return (mkastnode(op, NULL, NULL, intvalue));
}

// Make a unary AST node: only one child
struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue) {
  return (mkastnode(op, left, NULL, intvalue));
}
```

我们将使用 AST 来存储我们识别的每个表达式，以便稍后我们可以递归遍历它来计算表达式的最终值。 我们确实想处理数学运算符的优先级。 这是一个例子。 考虑表达式 `2*3+4*5`。现在，乘法比加法具有更高的优先级。 因此，我们希望将乘法操作数绑定在一起并在进行加法之前执行这些操作。我们要生成的 AST 树如右侧所示：在遍历树时，我们将首先执行`2*3`，然后执行`4*5`，最后将它们传递到根节点执行加操作

```c
          +
         / \
        /   \
       /     \
      *       *
     / \     / \
    2   3   4   5
```

## 朴素表达式解析器

在开始解析语法树之前，我们需要一个将扫描到的`token`转换为`AST`节点操作值的函数，如下：

```c
// expr.c
// Convert a token into an AST operation.
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
      fprintf(stderr, "unknown token in arithop() on line %d\n", Line);
      exit(1);
  }
}
```

我们需要一个函数来检查下一个标记是否是整数文字，并构建一个 AST 节点来保存文字值。如下

```c
// Parse a primary factor and return an
// AST node representing it.
static struct ASTnode *primary(void) {
  struct ASTnode *n;

  // For an INTLIT token, make a leaf AST node for it
  // and scan in the next token. Otherwise, a syntax error
  // for any other token type.
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
```

这里的`Token`是一个全局变量，保存着扫描到的最新的值。

那么我们现在可以写解析输入表达式生成`AST`的方法

```c
// Return an AST tree whose root is a binary operator
struct ASTnode *binexpr(void) {
  struct ASTnode *n, *left, *right;
  int nodetype;

  // Get the integer literal on the left.
  // Fetch the next token at the same time.
  left = primary();

  // If no tokens left, return just the left node
  if (Token.token == T_EOF)
    return (left);

  // Convert the token into a node type
  nodetype = arithop(Token.token);

  // Get the next token in
  scan(&Token);

  // Recursively get the right-hand tree
  right = binexpr();

  // Now build a tree with both sub-trees
  n = mkastnode(nodetype, left, right, 0);
  return (n);
}
```

这只是一个子简单的解析器，他的解析结果没有实现优先级的调整

解析结果如下：

```c
	 *
    / \
   2   +
      / \
     3   *
        / \
       4   5
```

正确的树状结构应该是这样的：

```c
          +
         / \
        /   \
       /     \
      *       *
     / \     / \
    2   3   4   5
```

我们将在下一节实现生成一个正确的AST。

那么接下来我们来试着写代码递归的解释这颗AST。我们以正确的语法树为例，伪代码：

```
interpretTree:
  First, interpret the left-hand sub-tree and get its value
  Then, interpret the right-hand sub-tree and get its value
  Perform the operation in the node at the root of our tree
  on the two sub-tree values, and return this value
```

调用过程可以用如下过程表示：

```
interpretTree0(tree with +):
  Call interpretTree1(left tree with *):
     Call interpretTree2(tree with 2):
       No maths operation, just return 2
     Call interpretTree3(tree with 3):
       No maths operation, just return 3
     Perform 2 * 3, return 6

  Call interpretTree1(right tree with *):
     Call interpretTree2(tree with 4):
       No maths operation, just return 4
     Call interpretTree3(tree with 5):
       No maths operation, just return 5
     Perform 4 * 5, return 20
  Perform 6 + 20, return 26
```

这是在`interp.c` 中并依据上述伪代码写的功能：

```c
// Given an AST, interpret the
// operators in it and return
// a final value.
int interpretAST(struct ASTnode *n) {
  int leftval, rightval;

  // Get the left and right sub-tree values
  if (n->left)
    leftval = interpretAST(n->left);
  if (n->right)
    rightval = interpretAST(n->right);

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
```