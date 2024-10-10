#include "syntaxtree.h"

struct node *boot = NULL;

int GetIntValue(struct node *Node) { return Node->lexical_value.intvalue; }

float GetFloatValue(struct node *Node) {
  return Node->lexical_value.floatvalue;
}

char *GetIDName(struct node *Node) { return Node->lexical_value.stringvalue; }

int GetLine(struct node *Node) { return Node->line; }

void BuildTrees(struct node **Nodes, int count) {
  for (int i = 0; i < count - 1; i++) {
    BuildrTree(Nodes[i], Nodes[i + 1]);
  }
}

struct node *InitTree(char *name, int line, struct node *lnode) {
  struct node *temp = CreateNode(SYNTAX, name, NULL);
  SetLine(temp, line);
  BuildlTree(temp, lnode);
  return temp;
}

void SetLine(struct node *Node, int line) { Node->line = line; }

void BuildlTree(struct node *Node, struct node *ltree) {
  Node->lson_node = ltree;
}

void BuildrTree(struct node *Node, struct node *rtree) {
  Node->rbro_node = rtree;
}

struct node *CreateNode(int type, char *name, struct WordType *word) {
  struct node *newnode = malloc(sizeof(struct node));
  newnode->type = type;
  newnode->name = name;
  newnode->rbro_node = NULL;
  newnode->lson_node = NULL;
  switch (type) {
  case TYPEINT:
    newnode->lexical_value.intvalue = word->value.intvalue;
    break;
  case TYPEFLOAT:
    newnode->lexical_value.floatvalue = word->value.floatvalue;
    break;
  case TYPESTRING:
  case OTHER_TYPE:
    newnode->lexical_value.stringvalue = word->value.stringvalue;
    break;
  }
  return newnode;
}

void PrintTree(struct node *Node, int spaceNum) {
  if (Node->type != EMPTY) {
    for (int i = 0; i < spaceNum; i++) {
      printf(" ");
    }
  }
  switch (Node->type) {
  case SYNTAX:
    if (Node->lson_node->type != EMPTY) {
      printf("%s (%d)\n", Node->name, Node->line);
    }
    break;
  case TYPESTRING:
    printf("%s: %s\n", Node->name, Node->lexical_value.stringvalue);
    break;
  case TYPEINT:
    printf("%s: %d\n", Node->name, Node->lexical_value.intvalue);
    break;
  case TYPEFLOAT:
    printf("%s: %f\n", Node->name, Node->lexical_value.floatvalue);
    break;
  case OTHER_TYPE:
    printf("%s: %s\n", Node->name, Node->lexical_value.stringvalue);
    break;
  case OTHER:
    printf("%s\n", Node->name);
    break;
  }
  if (Node->lson_node != NULL) {
    PrintTree(Node->lson_node, spaceNum + 2);
  }
  if (Node->rbro_node != NULL) {
    PrintTree(Node->rbro_node, spaceNum);
  }
}