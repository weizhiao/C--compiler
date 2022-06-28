#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"type.h"
#define HASHTABLESIZE 101

typedef struct HashNode_* HashNode;

struct HashNode_
{
	char* name;
	Type type;
	HashNode next;
};

HashNode varhashtable[HASHTABLESIZE];
HashNode structhashtable[HASHTABLESIZE];
HashNode deffundechashtable[HASHTABLESIZE];
HashNode declarefundechashtable[HASHTABLESIZE];

void InitHashTable();
int InsertVar(char* name, Type type);
int InsertStruct(char* name, Type type);
int InsertDefFunc(char* name, Type type);
int InsertDeclareFunc(char* name, Type type);
int IsExist(char* name,HashNode* hashtable);
Type FindStruct(char* name);
Type FindVar(char* name);
Type FindDefFunc(char* name);
Type FindDeclareFunc(char* name);

#endif // !_HASHTABLE_H_
