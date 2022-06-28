#ifndef _SYNTAXTREE_H_
#define _SYNTAXTREE_H_

#include<stdlib.h>
#include<stdio.h>
#include"wordtype.h"

#define EMPTY 0
#define SYNTAX 1
#define TYPEINT 2
#define TYPEFLOAT 3
#define TYPESTRING 4
#define OTHER_TYPE 5
#define OTHER 6

struct node {
	int type;
	int line;
	char* name;
	union {
		int intvalue;
		float floatvalue;
		char* stringvalue;
	}lexical_value;
	struct node* rbro_node;
	struct node* lson_node;
};

struct node* boot;
struct node* CreateNode(int type, char* name, struct WordType* word);
struct node* InitTree(char* name, int line, struct node* lnode);
void BuildTrees(struct node** Nodes, int count);
void SetLine(struct node* Node, int line);
void BuildlTree(struct node* Node, struct node* ltree);
void BuildrTree(struct node* Node, struct node* rtree);
void PrintTree(struct node* Node, int spaceNum);
int GetLine(struct node* Node);
char* GetIDName(struct node* Node);
int GetIntValue(struct node* Node);
float GetFloatValue(struct node* Node);

#endif // !_SYNTAXTREE_H_