#ifndef _STMTLIST_H_
#define _STMTLIST_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct StmtList_ *StmtList;

struct StmtList_;

StmtList CreateStmtList();
void AddStmt(char *str, StmtList stmtlist);
void ClearList(StmtList stmtlist);
void MergeList(StmtList stmtlist1, StmtList stmtlist2);
void WriteList(StmtList stmtlist, char *filename);

#endif // !_STMTLIST_H_
