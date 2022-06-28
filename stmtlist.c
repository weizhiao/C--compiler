#include"stmtlist.h"

struct StmtList_ {
	char* stmt;
	StmtList next;
};

StmtList CreateStmtList() {
	StmtList temp = malloc(sizeof(struct StmtList_));
	temp->next = NULL;
	temp->stmt = NULL;
	return temp;
}

void AddStmt(char* str, StmtList stmtlist) {
	StmtList temp = malloc(sizeof(struct StmtList_));
	temp->stmt = str;
	temp->next = stmtlist->next;
	stmtlist->next = temp;
}

void ClearList(StmtList stmtlist) {
	StmtList current = stmtlist->next;
	StmtList temp;
	while (current != NULL) {
		temp = current->next;
		free(current);
		current = temp;
	}
	stmtlist->next = NULL;
}

void MergeList(StmtList stmtlist1, StmtList stmtlist2) {
	StmtList temp = stmtlist2->next;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	temp->next = stmtlist1->next;
	stmtlist1->next = stmtlist2->next;
}

void WriteList(StmtList stmtlist, char* filename) {
	FILE* fp;
	fp = fopen(filename, "w");
	StmtList Reverse = CreateStmtList();
	StmtList temp = stmtlist->next;
	while (temp != NULL) {
		AddStmt(temp->stmt, Reverse);
		temp = temp->next;
	}
	temp = Reverse->next;
	while (temp != NULL) {
		printf("%s\n", temp->stmt);
		fprintf(fp, "%s\n", temp->stmt);
		temp = temp->next;
	}
}