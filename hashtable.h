#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASHTABLESIZE 101

typedef struct HashNode_ *HashNode;

struct HashNode_ {
  char *name;
  Type type;
  HashNode next;
};

void InitHashTable();
int InsertVar(char *name, Type type);
int InsertStruct(char *name, Type type);
int InsertDefFunc(char *name, Type type);
int InsertDeclareFunc(char *name, Type type);
int IsExist(char *name, HashNode *hashtable);
Type FindStruct(char *name);
Type FindVar(char *name);
Type FindDefFunc(char *name);
Type FindDeclareFunc(char *name);

#endif // !_HASHTABLE_H_
