

#ifndef RECOGNIZEEXP_H
#define RECOGNIZEEXP_H

int acceptNumber(List *lp);
int acceptIdentifier(List *lp);
int acceptCharacter(List *lp, char c);
int acceptExpression(List *lp);
int acceptEquation(List *lp);
void recognizeExpressions();
void recognizeEquation();

#endif
