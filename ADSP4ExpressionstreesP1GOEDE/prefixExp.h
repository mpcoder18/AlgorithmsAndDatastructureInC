

#ifndef PREFIXEXP_H
#define PREFIXEXP_H



typedef struct ExpTreeNode *ExpTree;
  
typedef struct ExpTreeNode {
  TokenType tt;
  Token t;
  ExpTree left;
  ExpTree right;
} ExpTreeNode;

ExpTree newExpTreeNode(TokenType tt, Token t, ExpTree tL, ExpTree tR);
int valueIdentifier(List *lp, char **sp);
int isNumerical(ExpTree tr);
double valueExpTree(ExpTree tr);
int treePrefixExpression(List *lp, ExpTree *tp);
void printExpTreeInfix(ExpTree tr);
void prefExpTrees();
void freeExpTree(ExpTree tr);
int isOperator(char c);
int isNumerical(ExpTree tr);
int valueOperator(List *lp, char *cp);

#endif
