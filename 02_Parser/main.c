#include "defs.h"
#define extern_
#include "data.h"
#undef extern_
#include "decl.h"
#include <errno.h>

static void usage(char *prog) {
  fprintf(stderr, "Usage: %s infile\n", prog);
  exit(1);
}

// Initialise global variables
static void init()
{
  Line = 1;
  Putback = '\n';
}

void main(int argc, char *argv[]) {
    struct ASTnode *n;

    if (argc != 2)
      usage(argv[0]);

    init();

    if ((Infile = fopen(argv[1], "r")) == NULL) {
      fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
      exit(1);
    }

    scan(&Token);			// 获取第一个token
    // printf("Token: %d, Val: %d \n", Token.token, Token.intvalue);
    n = binexpr();		// AST树根节点
    printf("%d\n", interpretAST(n));	// 最终结果
    exit(0);
}