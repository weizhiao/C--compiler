#include "hashtable.h"

HashNode varhashtable[HASHTABLESIZE];
HashNode structhashtable[HASHTABLESIZE];
HashNode deffundechashtable[HASHTABLESIZE];
HashNode declarefundechashtable[HASHTABLESIZE];

static unsigned int BKDRHash(char *str) {
  unsigned int seed = 131313;
  unsigned int hash = 0;
  while (*str) {
    hash = hash * seed + (*str++);
  }
  return hash % HASHTABLESIZE;
}

static void Insert(char *name, Type type, HashNode *hashtable) {
  unsigned int hashvalue = BKDRHash(name);
  HashNode node = malloc(sizeof(struct HashNode_));
  node->next = hashtable[hashvalue];
  hashtable[hashvalue] = node;
  node->name = name;
  node->type = type;
}

static Type Find(char *name, HashNode *hashtable) {
  unsigned int hashvalue = BKDRHash(name);
  HashNode current_node = hashtable[hashvalue];
  while (current_node != NULL) {
    if (strcmp(current_node->name, name) == 0) {
      return current_node->type;
    }
    current_node = current_node->next;
  }
  return NULL;
}

void InitHashTable() {
  for (int i = 0; i < HASHTABLESIZE; i++) {
    varhashtable[i] = NULL;
    structhashtable[i] = NULL;
    deffundechashtable[i] = NULL;
    declarefundechashtable[i] = NULL;
  }
}

int IsExist(char *name, HashNode *hashtable) {
  unsigned int hashvalue = BKDRHash(name);
  HashNode current_node = hashtable[hashvalue];
  while (current_node != NULL) {
    if (strcmp(current_node->name, name) == 0) {
      return 1;
    }
    current_node = current_node->next;
  }
  return 0;
}

int InsertVar(char *name, Type type) {
  if (IsExist(name, varhashtable)) {
    return 0;
  }
  if (IsExist(name, structhashtable)) {
    return 0;
  }
  Insert(name, type, varhashtable);
  return 1;
}

int InsertStruct(char *name, Type type) {
  if (IsExist(name, varhashtable)) {
    return 0;
  }
  if (IsExist(name, structhashtable)) {
    return 0;
  }
  Insert(name, type, structhashtable);
  return 1;
}

Type FindStruct(char *name) { return Find(name, structhashtable); }

Type FindVar(char *name) { return Find(name, varhashtable); }

Type FindDefFunc(char *name) { return Find(name, deffundechashtable); }

Type FindDeclareFunc(char *name) { return Find(name, declarefundechashtable); }

int InsertDeclareFunc(char *name, Type type) {
  Type temp = FindDeclareFunc(name);
  if (temp != NULL) {
    if (IsTypeEqual(temp, type) == 1) {
      return 1;
    }
    return 0;
  }
  Insert(name, type, declarefundechashtable);
  return 1;
}

int InsertDefFunc(char *name, Type type) {
  if (IsExist(name, deffundechashtable)) {
    return 0;
  }
  Insert(name, type, deffundechashtable);
  return 1;
}