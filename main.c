#include <stdio.h>

extern int yyparse(void);
int iserror = 0;
extern struct node *boot;
extern void SemanticAnalysis(struct node *Node);
extern void TranSlate(struct node *Node, char *filename);
extern void yyrestart(FILE *input_file);

int main(int argc, char **argv) {
  if (argc <= 1)
    return 1;
  FILE *f = fopen(argv[1], "r");
  if (!f) {
    perror(argv[1]);
    return 1;
  }
  yyrestart(f);
  yyparse();
  if (iserror == 0) {
    SemanticAnalysis(boot);
  }
  if (iserror == 0) {
    if (argc == 3) {
      TranSlate(boot, argv[2]);
    } else {
      TranSlate(boot, "outdefault.ir");
    }
  }
  return 0;
}