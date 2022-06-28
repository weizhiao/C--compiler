#include"translate.h"
#include"stmtlist.h"

#define MAXLENGTH 10
#define STRBUFFERLENGTH 20

typedef struct HashVarTable_* HashVarTable;
typedef struct Addr_* Addr;

struct HashVarTable_ {
	char* varname;
	char* v;
	int isptr;
	HashVarTable next;
};

struct Addr_ {
	char* addr;
	Type type;
};

static StmtList stmtlist;
static StmtList stmtbuffer;
static int CountTemp = 1;
static int CountLabel = 1;
static int CountVar = 1;
static int IsInitStmtList = 0;
static HashVarTable vartable[HASHTABLESIZE];

static Addr CreateAddr(char* addr, Type type);
static unsigned int BKDRHash(char* str);
static void SetIsPtr(char* varname);
static char* ExpUesStar(char* addr);
static int IsPtr(char* varname);
static void InsertV(char* v, char* varname);
static HashVarTable FindV(char* varname);
static void InitStmtList();
static void GenStmt(char** str);
static void GenTempStmt(char** str);
static void ClearBuffer();
static void FlushStmtBuffer();
static char* NewTemp();
static char* NewLabel();
static char* NewVar(char* varname);
static Addr TranSlateExp(struct node* Node);
static void TranSlateStmt(struct node* Node);
static void TranSlateDec(struct node* Node);
static void TranSlateDef(struct node* Node);
static void TranSlateCompSt(struct node* Node);
static Addr TranSlateVarDec(struct node* Node,int isparam);
static void TranSlateVarList(struct node* Node);
static void TranSlateFunc(struct node* Node);
static void TranSlateProgram(struct node* Node);

static char* ExpUesStar(char* addr) {
	char* tempstr = malloc(STRBUFFERLENGTH);
	char* addr1;
	if (addr[0] == '*') {
		addr1 = NewTemp();
		sprintf(tempstr, "%s := %s", addr1, addr);
		GenStmt(&tempstr);
		return addr1;
	}
	else {
		return addr;
	}
}

static Addr CreateAddr(char* addr,Type type) {
	Addr temp = malloc(sizeof(struct Addr_));
	temp->addr = addr;
	temp->type = type;
	return temp;
}

static void ClearBuffer() {
	ClearList(stmtbuffer);
}

static void FlushStmtBuffer() {
	MergeList(stmtlist, stmtbuffer);
	stmtbuffer = CreateStmtList();
}

static unsigned int BKDRHash(char* str)
{
	unsigned int seed = 131313;
	unsigned int hash = 0;
	while (*str)
	{
		hash = hash * seed + (*str++);
	}
	return hash % HASHTABLESIZE;
}

static void InsertV(char* v,char* varname) {
	unsigned int hashvalue = BKDRHash(varname);
	HashVarTable node = malloc(sizeof(struct HashVarTable_));
	node->next = vartable[hashvalue];
	vartable[hashvalue] = node;
	node->v = v;
	node->isptr = 0;
	node->varname = varname;
}

static void SetIsPtr(char* varname) {
	HashVarTable temp = FindV(varname);
	temp->isptr = 1;
}

static int IsPtr(char* varname) {
	HashVarTable temp = FindV(varname);
	return temp->isptr;
}

static HashVarTable FindV(char* varname) {
	unsigned int hashvalue = BKDRHash(varname);
	HashVarTable current_node = vartable[hashvalue];
	while (current_node != NULL) {
		if (strcmp(current_node->varname, varname) == 0) {
			return current_node;
		}
		current_node = current_node->next;
	}
	return NULL;
}

static void InitStmtList() {
	stmtlist = CreateStmtList();
	stmtbuffer = CreateStmtList();
}

static void GenStmt(char** str) {
	AddStmt(*str, stmtlist);
	*str = malloc(STRBUFFERLENGTH);
}

static void GenTempStmt(char** str) {
	AddStmt(*str, stmtbuffer);
	*str = malloc(STRBUFFERLENGTH);
}

static char* NewTemp() {
	char* temp = malloc(MAXLENGTH * sizeof(char));
	char str[10];
	sprintf(str, "%d", CountTemp);
	strcat(temp, "t");
	strcat(temp, str);
	CountTemp++;
	return temp;
}

static char* NewLabel() {
	char* temp = malloc(MAXLENGTH * sizeof(char));
	char str[10];
	sprintf(str, "%d", CountLabel);
	strcat(temp, "label");
	strcat(temp, str);
	CountLabel++;
	return temp;
}

static char* NewVar(char* varname) {
	HashVarTable temp1 = FindV(varname);
	char* temp;
	if (temp1 == NULL) {
		temp = malloc(MAXLENGTH * sizeof(char));
		char str[10];
		sprintf(str, "%d", CountVar);
		strcat(temp, "v");
		strcat(temp, str);
		InsertV(temp, varname);
		CountVar++;
	}
	else {
		temp = temp1->v;
	}
	return temp;
}

static Addr TranSlateExp(struct node* Node) {
	struct node* current = Node->lson_node;
	char* tempidname;
	int typesize;
	Addr SonTree[10];
	Type temptype;
	int index = 0;
	char str[30]="";
	char* addr = NULL;
	char* addr1 = NULL;
	char* addr2 = NULL;
	char* tempstr = malloc(STRBUFFERLENGTH);
	while (current != NULL) {
		strcat(str, current->name);
		SonTree[index] = TranSlateExp(current);
		current = current->rbro_node;
		index++;
	}
	if (strcmp(str, "Exp") == 0) {
		sprintf(tempstr, "ARG %s", SonTree[0]->addr);
		GenTempStmt(&tempstr);
		return SonTree[0];
	}
	if (strcmp(str, "ID") == 0) { 
		tempidname = GetIDName(Node->lson_node);
		addr = NewVar(tempidname);
		temptype = FindVar(tempidname);
		if ((temptype->kind == ARRAY|| temptype->kind == STRUCTURE)&&IsPtr(tempidname)==0) {
			sprintf(tempstr, "&%s", addr);
			return CreateAddr(tempstr, FindVar(GetIDName(Node->lson_node)));
		}
		return CreateAddr(addr, FindVar(GetIDName(Node->lson_node)));
	}
	if (strcmp(str, "INT") == 0) {
		sprintf(tempstr, "#%d", GetIntValue(Node->lson_node));
		return CreateAddr(tempstr, standtypeint);
	}
	if (strcmp(str, "FLOAT") == 0) {
		sprintf(tempstr, "#%f", GetFloatValue(Node->lson_node));
		return CreateAddr(tempstr, standtypefloat);
	}
	if (strcmp(str, "MINUSExp") == 0) {
		addr1 = ExpUesStar(SonTree[1]->addr);
		addr = NewTemp();
		sprintf(tempstr, "%s := #0 - %s", addr, addr1);
		GenStmt(&tempstr);
		return CreateAddr(addr, SonTree[1]->type);
	}
	if (strcmp(str, "ExpPLUSExp") == 0) {
		addr1 = ExpUesStar(SonTree[0]->addr);
		addr2 = ExpUesStar(SonTree[2]->addr);
		addr = NewTemp();
		sprintf(tempstr, "%s := %s + %s", addr, addr1, addr2);
		GenStmt(&tempstr);
		return CreateAddr(addr, SonTree[0]->type);
	}
	if (strcmp(str, "ExpDIVExp") == 0) {
		addr1 = ExpUesStar(SonTree[0]->addr);
		addr2 = ExpUesStar(SonTree[2]->addr);
		addr = NewTemp();
		sprintf(tempstr, "%s := %s / %s", addr, addr1, addr2);
		GenStmt(&tempstr);
		return CreateAddr(addr, SonTree[0]->type);
	}
	if (strcmp(str, "ExpMINUSExp") == 0) {
		addr1 = ExpUesStar(SonTree[0]->addr);
		addr2 = ExpUesStar(SonTree[2]->addr);
		addr = NewTemp();
		sprintf(tempstr, "%s := %s - %s", addr, addr1, addr2);
		GenStmt(&tempstr);
		return CreateAddr(addr, SonTree[0]->type);
	}
	if (strcmp(str, "ExpSTARExp") == 0) {
		addr1 = ExpUesStar(SonTree[0]->addr);
		addr2 = ExpUesStar(SonTree[2]->addr);
		addr = NewTemp();
		sprintf(tempstr, "%s := %s * %s", addr, addr1, addr2);
		GenStmt(&tempstr);
		return CreateAddr(addr, SonTree[0]->type);
	}
	if (strcmp(str, "ExpRELOPExp") == 0) {
		addr1 = ExpUesStar(SonTree[0]->addr);
		addr2 = ExpUesStar(SonTree[2]->addr);
		sprintf(tempstr, "%s %s %s", addr1, Node->lson_node->rbro_node->lexical_value.stringvalue, addr2);
		return CreateAddr(tempstr, standtypeint);
	}
	if (strcmp(str, "ExpASSIGNOPExp") == 0) {
		addr2 = ExpUesStar(SonTree[2]->addr);
		sprintf(tempstr, "%s := %s", SonTree[0]->addr, addr2);
		GenStmt(&tempstr);
		return SonTree[0];
	}
	if (strcmp(str, "LPExpRP") == 0) {
		return SonTree[1];
	}
	if (strcmp(str, "ExpDOTID") == 0) {
		addr = NewTemp();
		tempidname = GetIDName(Node->lson_node->rbro_node->rbro_node);
		typesize = ComputeFieldAddr(SonTree[0]->type, tempidname);
		sprintf(tempstr, "%s := %s + #%d", addr, SonTree[0]->addr, typesize);
		GenStmt(&tempstr);
		sprintf(tempstr, "*%s", addr);
		return CreateAddr(tempstr, FindStructFieldType(SonTree[0]->type, tempidname));
	}
	if (strcmp(str, "IDLPRP") == 0) {
		tempidname = GetIDName(Node->lson_node);
		addr = NewTemp();
		if (strcmp(tempidname, "read") == 0) {
			sprintf(tempstr, "READ %s", addr);
			GenStmt(&tempstr);
		}
		else {
			sprintf(tempstr, "%s := CALL %s", addr, tempidname);
			GenStmt(&tempstr);
		}
		return CreateAddr(addr, ReturnFunDecReturnType(FindDefFunc(tempidname)));
	}
	if (strcmp(str, "ExpCOMMAArgs") == 0) {
		sprintf(tempstr, "ARG %s", SonTree[0]->addr);
		GenTempStmt(&tempstr);
		return SonTree[0];
	}
	if (strcmp(str, "IDLPArgsRP") == 0) {
		tempidname = GetIDName(Node->lson_node);
		addr = NewTemp();
		if (strcmp(tempidname, "write") == 0) {
			ClearBuffer();
			addr1 = ExpUesStar(SonTree[2]->addr);
			sprintf(tempstr, "WRITE %s", addr1);
			GenStmt(&tempstr);
		}
		else {
			FlushStmtBuffer();
			sprintf(tempstr, "%s := CALL %s", addr, tempidname);
			GenStmt(&tempstr);
		}
		return CreateAddr(addr, ReturnFunDecReturnType(FindDefFunc(tempidname)));
	}
	if (strcmp(str, "ExpLBExpRB") == 0) {
		char* addr1;
		addr1 = NewTemp();
		temptype = VisitArray(SonTree[0]->type);
		typesize = ComputeTypeSize(temptype);
		sprintf(tempstr, "%s := %s * #%d", addr1, SonTree[2]->addr, typesize);
		GenStmt(&tempstr);
		addr = NewTemp();
		sprintf(tempstr, "%s := %s + %s", addr, SonTree[0]->addr, addr1);
		GenStmt(&tempstr);
		if (temptype->kind !=ARRAY) {
			sprintf(tempstr, "*%s", addr);
			return CreateAddr(tempstr, temptype);
		}
		else {
			return CreateAddr(addr, temptype);
		}
	}
	return NULL;
}

static void TranSlateStmt(struct node* Node) {
	Addr tempaddr = NULL;
	struct node* tempstmt = NULL;
	int IsVisitLson = 1;
	int IsVisitRson = 1;
	char* tempstr = malloc(STRBUFFERLENGTH);
	char* templabel1 = NULL;
	char* templabel2 = NULL;
	char* templabel3 = NULL;
	if (strcmp(Node->name, "CompSt") == 0) {
		IsVisitLson = 0;
		TranSlateCompSt(Node->lson_node);
	}
	if (strcmp(Node->name, "Exp") == 0) {
		IsVisitLson = 0;
		TranSlateExp(Node);
	}
	if (strcmp(Node->name, "RETURN") == 0) {
		IsVisitRson = 0;
		tempaddr = TranSlateExp(Node->rbro_node);
		sprintf(tempstr, "RETURN %s", tempaddr->addr);
		GenStmt(&tempstr);
	}
	if (strcmp(Node->name, "IF") == 0) {
		IsVisitRson = 0;
		templabel1 = NewLabel();
		templabel2 = NewLabel();
		tempaddr = TranSlateExp(Node->rbro_node->rbro_node);
		sprintf(tempstr, "IF %s GOTO %s", tempaddr->addr, templabel1);
		GenStmt(&tempstr);
		sprintf(tempstr, "GOTO %s", templabel2);
		GenStmt(&tempstr);
		sprintf(tempstr, "LABEL %s :", templabel1);
		GenStmt(&tempstr);
		tempstmt = Node->rbro_node->rbro_node->rbro_node->rbro_node;
		TranSlateStmt(tempstmt->lson_node);
		if (tempstmt->rbro_node != NULL) {
			templabel3 = NewLabel();
			sprintf(tempstr, "GOTO %s", templabel3);
			GenStmt(&tempstr);
		}
		sprintf(tempstr, "LABEL %s :", templabel2);
		GenStmt(&tempstr);
		if (tempstmt->rbro_node != NULL) {
			tempstmt = tempstmt->rbro_node->rbro_node;
			TranSlateStmt(tempstmt->lson_node);
			sprintf(tempstr, "LABEL %s :", templabel3);
			GenStmt(&tempstr);
		}
	}
	if (strcmp(Node->name, "WHILE") == 0) {
		IsVisitRson = 0;
		templabel1 = NewLabel();
		templabel2 = NewLabel();
		templabel3 = NewLabel();
		tempaddr = TranSlateExp(Node->rbro_node->rbro_node);
		sprintf(tempstr, "LABEL %s :", templabel3);
		GenStmt(&tempstr);
		sprintf(tempstr, "IF %s GOTO %s", tempaddr->addr, templabel1);
		GenStmt(&tempstr);
		sprintf(tempstr, "GOTO %s", templabel2);
		GenStmt(&tempstr);
		sprintf(tempstr, "LABEL %s :", templabel1);
		GenStmt(&tempstr);
		tempstmt = Node->rbro_node->rbro_node->rbro_node->rbro_node;
		TranSlateStmt(tempstmt);
		sprintf(tempstr, "GOTO %s", templabel3);
		GenStmt(&tempstr);
		sprintf(tempstr, "LABEL %s :", templabel2);
		GenStmt(&tempstr);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		TranSlateStmt(Node->lson_node);
	}
	if (IsVisitRson == 1 && Node->rbro_node != NULL) {
		TranSlateStmt(Node->rbro_node);
	}
}

static Addr TranSlateVarDec(struct node* Node,int isparam) {
	char* tempstr = malloc(STRBUFFERLENGTH);
	Addr SonTree[10];
	Type temptype;
	int index = 0;
	char str[20] = "";
	char* addr = NULL;
	char* tempidname = NULL;
	struct node* current = Node->lson_node;
	while (current != NULL) {
		strcat(str, current->name);
		SonTree[index] = TranSlateVarDec(current,isparam);
		current = current->rbro_node;
		index++;
	}
	if (strcmp(str, "ID") == 0) {
		tempidname = GetIDName(Node->lson_node);
		temptype = FindVar(tempidname);
		addr = NewVar(tempidname);
		if (isparam == 1) {
			SetIsPtr(tempidname);
			sprintf(tempstr, "PARAM %s", addr);
			GenStmt(&tempstr);
		}
		else {
			if (ReturnTypeKind(temptype) == ARRAY || ReturnTypeKind(temptype) == STRUCTURE) {
				int typesize;
				typesize = ComputeTypeSize(temptype);
				sprintf(tempstr, "DEC %s %d", addr, typesize);
				GenStmt(&tempstr);
			}
		}
		return CreateAddr(addr, temptype);
	}
	if(strcmp(str,"VarDecLBINTRB") == 0) {
		return SonTree[0];
	}
	return NULL;
}

static void TranSlateVarList(struct node* Node) {
	int IsVisitLson = 1;
	int IsVisitRson = 1;
	if (strcmp(Node->name, "VarDec") == 0) {
		IsVisitLson = 0;
		TranSlateVarDec(Node, 1);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		TranSlateVarList(Node->lson_node);
	}
	if (IsVisitRson == 1 && Node->rbro_node != NULL) {
		TranSlateVarList(Node->rbro_node);
	}
}

static void TranSlateCompSt(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "Dec") == 0) {
		IsVisitLson = 0;
		TranSlateDec(Node->lson_node);
	}
	if (strcmp(Node->name, "Stmt") == 0) {
		IsVisitLson = 0;
		TranSlateStmt(Node->lson_node);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		TranSlateCompSt(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		TranSlateCompSt(Node->rbro_node);
	}
}

static void TranSlateFunc(struct node* Node) {
	char* tempstr = malloc(40);
	int IsVisitLson = 1;
	if (strcmp(Node->name, "FunDec") == 0) {
		IsVisitLson = 0;
		sprintf(tempstr, "FUNCTION %s :", GetIDName(Node->lson_node));
		GenStmt(&tempstr);
		if (strcmp(Node->lson_node->rbro_node->rbro_node->name,"VarList")==0) {
			TranSlateVarList(Node->lson_node->rbro_node->rbro_node->lson_node);
		}
	}
	if (strcmp(Node->name, "CompSt") == 0) {
		IsVisitLson = 0;
		TranSlateCompSt(Node->lson_node);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		TranSlateFunc(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		TranSlateFunc(Node->rbro_node);
	}
}

static void TranSlateDec(struct node* Node) {
	char* tempstr = malloc(STRBUFFERLENGTH);
	char* addr;
	char* addr1;
	Addr temp;
	Addr temp1;
	temp = TranSlateVarDec(Node, 0);
	addr = temp->addr;
	if (Node->rbro_node != NULL) {
		temp1 = TranSlateExp(Node->rbro_node->rbro_node);
		addr1 = ExpUesStar(temp1->addr);
		sprintf(tempstr, "%s := %s", addr, addr1);
		GenStmt(&tempstr);
	}
}

static void TranSlateDef(struct node* Node) {
	char* tempstr = malloc(STRBUFFERLENGTH);
	char* addr;
	Type temptype;
	if (strcmp(Node->name, "ID") == 0) {
		temptype = FindVar(GetIDName(Node));
		addr = NewVar(GetIDName(Node));
		if (ReturnTypeKind(temptype) == ARRAY || ReturnTypeKind(temptype) == STRUCTURE) {
			int typesize;
			typesize = ComputeTypeSize(temptype);
			sprintf(tempstr, "DEC %s %d", addr, typesize);
			GenStmt(&tempstr);
		}
	}
	if (Node->lson_node != NULL) {
		TranSlateDef(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		TranSlateDef(Node->rbro_node);
	}
}

static void TranSlateProgram(struct node* Node) {
	int IsVisitLson = 1;
	int IsVisitRson = 1;
	if (strcmp(Node->name, "FunDec") == 0) {
		IsVisitLson = 0;
		IsVisitRson = 0;
		TranSlateFunc(Node);
	}
	if (strcmp(Node->name, "ExtDecList") == 0) {
		IsVisitLson = 0;
		TranSlateDef(Node->lson_node);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		TranSlateProgram(Node->lson_node);
	}
	if (IsVisitRson == 1 && Node->rbro_node != NULL) {
		TranSlateProgram(Node->rbro_node);
	}
}

void TranSlate(struct node* Node,char* filename) {
	if (IsInitStmtList == 0) {
		IsInitStmtList = 1;
		InitStmtList();
	}
	TranSlateProgram(Node);
	WriteList(stmtlist, filename);
}
