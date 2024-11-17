#include <stdio.h>  /* printf */
#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */
#include "scanner.h"
#include "recognizeExp.h"
#include "evalExp.h"
#include "prefixExp.h"
#include "infixExp.h"
#include "string.h" /* strcmp */


int checkOperator(List *lp, char *cp) {
  if (*lp != NULL && (*lp)->tt == Symbol && isOperator(((*lp)->t).symbol)) {
    *cp = ((*lp)->t).symbol;
    return 1;
  }
  return 0;
}


int treeExpression(List *lp, ExpTree *tp) {
  char c;
  ExpTree tpLeft, tpRight;
  Token t;
  // we use the left node as a temporary storage for the expression tree we create
  if (!treeTerm(lp, &tpLeft)) {
    return 0;
  }
  //iterates through as many + and - as necessary, adding previously generated nodes as left children
  while (checkOperator(lp, &c) && (c == '+' || c == '-')) {
    valueOperator(lp, &c); // gets the operator which is either + or 0
    t.symbol = c;
    if (!treeTerm(lp, &tpRight)) { //get the next term as the right child
      return 0;
    }
    // temporary node is the left one, the new is the right one
    tpLeft = newExpTreeNode(Symbol, t, tpLeft, tpRight);
  }
  // add what is in the temporary variable
  *tp = tpLeft;
  return 1;
}

// creates a tree structure for a factor
int treeFactor(List *lp, ExpTree *tp) {
  double w;
  Token t;
  char *s;
  if (valueNumber(lp, &w)) { //set up a number
    t.number = (int) w;
    *tp = newExpTreeNode(Number, t, NULL, NULL);
    return 1;
  }
  if (valueIdentifier(lp, &s)) { // set up an identifier
    t.identifier = s;
    *tp = newExpTreeNode(Identifier, t, NULL, NULL);
    return 1;
  }
  return acceptCharacter(lp, '(') && treeExpression(lp, tp) &&
         acceptCharacter(lp, ')'); // account for possible expressions in parentheses
}

// creates tree structure for a term
int treeTerm(List *lp, ExpTree *tp) {
  char c;
  ExpTree tpLeft, tpRight;
  Token t;
  // we use the left node as a temporary storage for the expression tree we create
  if (!treeFactor(lp, &tpLeft)) {
    return 0;
  }
  //iterates through as many + and - as necessary, adding previously generated nodes as left children
  while (checkOperator(lp, &c) && (c == '*' || c == '/')) {
    valueOperator(lp, &c);
    t.symbol = c;
    if (!treeFactor(lp, &tpRight)) { //get the next term as the right child
      return 0;
    }
    // temporary node is the left one, the new is the right one
    tpLeft = newExpTreeNode(Symbol, t, tpLeft, tpRight);
  }
  // add what is in the temporary variable
  *tp = tpLeft;
  return 1;
}

// creates and returns a copy of an expression tree
ExpTree copyExpTree(ExpTree *tp) {
  if ((*tp) == NULL) {
    return NULL;
  }
  return newExpTreeNode((*tp)->tt, (*tp)->t, copyExpTree(&(*tp)->left), copyExpTree(&(*tp)->right));
}

// simplifies the tree according to the rules given
void simplify(ExpTree *tp) {
  if ((*tp) == NULL) {
    return;
  }
  if ((*tp)->tt == Number) { // numbers are leaves, if a leave is reached, stop the recursion
    return;
  }
  if ((*tp)->tt == Symbol) {
    // recursively simplify the lower nodes
    simplify(&(*tp)->left);
    simplify(&(*tp)->right);
    // if left child is a number, get its value
    int lVal = 0, leftNumber = 0;
    if (((*tp)->left)->tt == Number) {
      leftNumber = 1; // says that left child is a number
      lVal = (((*tp)->left)->t).number;
    }
    // if left child is a number, get its value
    int rVal = 0, rightNumber = 0;
    if (((*tp)->right)->tt == Number) {
      rightNumber = 1; // says that right child is a number
      rVal = (((*tp)->right)->t).number;
    }
    Token t;
    ExpTree tempTree;
    switch (((*tp)->t).symbol) {
      // the structures of if statements in the cases are similar to each other so only the first cases are commented

      case '*': 
        if ((!lVal && leftNumber) || (!rVal && rightNumber)) { // E*0 or 0*E
          freeExpTree(*tp);
          t.number = 0;
          *tp = newExpTreeNode(Number, t, NULL, NULL); // simplify to a leaf of value 0
          return;
        }
        if (leftNumber == 1 && lVal == 1) { // 1*E
          tempTree = *tp; // temporarily save the node of the tree
          *tp = copyExpTree(&(*tp)->right); // set the new node as a copy of its right child
          freeExpTree(tempTree); // free the memory from the abandoned node
          return;
        }
        if (rightNumber == 1 && rVal == 1) { // E*1
          tempTree = *tp;
          *tp = copyExpTree(&(*tp)->left);
          freeExpTree(tempTree);
          return;
        }
        break;

      case '/': // potential simplifications for /
        if (rightNumber == 1 && rVal == 1) { // E/1
          tempTree = *tp;
          *tp = copyExpTree(&(*tp)->left);
          freeExpTree(tempTree);
          return;
        }
        break;

      case '+': // potential simplifications for +
        if (leftNumber == 1 && lVal == 0) { // 0+E
          tempTree = *tp;
          *tp = copyExpTree(&(*tp)->right);
          freeExpTree(tempTree);
          return;
        }
        if (rightNumber == 1 && rVal == 0) { // E+0
          tempTree = *tp;
          *tp = copyExpTree(&(*tp)->left);
          freeExpTree(tempTree);
          return;
        }
        break;

      case '-': // potential simplifications for -
        if (rightNumber == 1 && rVal == 0) { // E-0
          tempTree = *tp;
          *tp = copyExpTree(&(*tp)->left);
          freeExpTree(tempTree);
          return;
        }
        break;
    }
  }
}

// differentiates the tree
void differentiate(ExpTree *tp) {
  if ((*tp) == NULL) {
    return;
  }
  if ((*tp)->tt == Number) { // derivative of a number
    (*tp)->t.number = 0;
    return;
  }
  if ((*tp)->tt == Identifier) { // derivative of a variable
    (*tp)->tt = Number;
    if (strcmp(((*tp)->t).identifier, "x") == 0) { // dx/dx = 1
      (*tp)->t.number = 1;
      return;
    } // anything other variable y: dy/dx = 0
    (*tp)->t.number = 0;
    return;
  }
  if ((*tp)->tt == Symbol) { // derivative of an expression
    Token t;
    ExpTree difRight, difLeft, saveRight, saveLeft;
    switch (((*tp)->t).symbol) {

      case '*': // (left * right)' = difLeft * saveRight + saveLeft * difRight
        // prepare all components
        difRight = copyExpTree(&(*tp)->right);
        difLeft = copyExpTree(&(*tp)->left);
        saveRight = copyExpTree(&(*tp)->right);
        saveLeft = copyExpTree(&(*tp)->left);
        differentiate(&difLeft);
        differentiate(&difRight);
        ((*tp)->t).symbol = '+'; // change the symbol of this node
        t.symbol = '*'; // the symbol for new node which will be created
        // free old nodes and append new values
        freeExpTree((*tp)->left);
        freeExpTree((*tp)->right);
        (*tp)->left = newExpTreeNode(Symbol, t, difLeft, saveRight);
        (*tp)->right = newExpTreeNode(Symbol, t, saveLeft, difRight);
        break;

      case '/': /* (left * right)' = newLeft / newRight =
                    (difLeft * saveRight - saveLeft * difRight) / (divisor1 * divisor2) */
        t.symbol = '*';
        ExpTree divisor1, divisor2, newRight, newLeft, newLeftLeft, newLeftRight;
        // right part of the differentiation
        divisor1 = copyExpTree(&(*tp)->right);
        divisor2 = copyExpTree(&(*tp)->right);
        newRight = newExpTreeNode(Symbol, t, divisor1, divisor2);
        // prepare all components of the left side (same as multiplication)
        difRight = copyExpTree(&(*tp)->right);
        difLeft = copyExpTree(&(*tp)->left);
        saveRight = copyExpTree(&(*tp)->right);
        saveLeft = copyExpTree(&(*tp)->left);
        differentiate(&difLeft);
        differentiate(&difRight);
        t.symbol = '*';
        newLeftLeft = newExpTreeNode(Symbol, t, difLeft, saveRight);
        newLeftRight = newExpTreeNode(Symbol, t, saveLeft, difRight);
        // connect the parts with a '-'
        t.symbol = '-';
        newLeft = newExpTreeNode(Symbol, t, newLeftLeft, newLeftRight);
        // put it all back together
        freeExpTree((*tp)->left);
        freeExpTree((*tp)->right);
        (*tp)->left = newLeft;
        (*tp)->right = newRight;
        break;

      case '+': // same as '-', so we let it fall through
      case '-':
        differentiate(&(*tp)->left);
        differentiate(&(*tp)->right);
        break;
    }
  }
}

/* The function infExpTrees performs a dialogue with the user and tries
 * to recognize the input as an infix expression. When it is a numerical infix
 * expression its value is computed and printed.
 */
void infExpTrees() {
  char *ar;
  List tl, tl1;
  ExpTree t = NULL;
  printf("give an expression: ");
  ar = readInput();
  while (ar[0] != '!') {
    tl = tokenList(ar);
    printList(tl);
    tl1 = tl;
    if (treeExpression(&tl1, &t) && tl1 == NULL) {
      /* there should be no tokens left */
      printf("in infix notation: ");
      printExpTreeInfix(t);
      printf("\n");
      if (isNumerical(t)) {
        printf("the value is %g\n", valueExpTree(t));
      } else {
        printf("this is not a numerical expression\n");
        printf("simplified: ");
        simplify(&t);
        printExpTreeInfix(t);
        printf("\n");
        printf("derivative to x: ");
        differentiate(&t);
        simplify(&t);
        printExpTreeInfix(t);
        printf("\n");
      }
    } else {
      printf("this is not an expression\n");
    }
    freeExpTree(t);
    t = NULL;
    freeTokenList(tl);
    free(ar);
    printf("\ngive an expression: ");
    ar = readInput();
  }
  free(ar);
  printf("good bye\n");
}
