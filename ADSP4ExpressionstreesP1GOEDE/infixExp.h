#ifndef INFIXEXP_H
#define INFIXEXP_H

void infExpTrees();
int treeExpression(List *lp, ExpTree *tp);
int treeTerm(List *lp, ExpTree *tp);
int treeFactor(List *lp, ExpTree *tp);
int checkOperator(List *lp, char *cp);
void simplify (ExpTree *tp);
void differentiate (ExpTree *tp);
ExpTree copyExpTree(ExpTree *tp);

#endif