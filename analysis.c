#include"analysis.h"

#define COMPSTUSE 0
#define STRUCTSPECIFIERUSE 1
#define DEFFUNC 2
#define DECLAREFUNC 3

typedef struct SpecifierStack_* SpecifierStack;
typedef struct TreeNodeStack_* TreeNodeStack;
typedef struct MultiTree_* MultiTree;
typedef struct DeclareFuncList_* DeclareFuncList;

struct SpecifierStack_ {
	Type specifier;
	SpecifierStack next;
};

struct DeclareFuncList_ {
	char* funcname;
	int line;
	DeclareFuncList next;
};

extern int iserror;
static SpecifierStack specifierstack;
static DeclareFuncList declarefunclist;
static int IsArray = 0;
static int hasAssign = 0;
static int IsInitStandType = 0;
static int IsInitDeclareFuncList = 0;
static int IsInitStack = 0;
static int IsInitHashTable = 0;
static int IsAddFunc = 0;
static Type Specifier;
static Type Array;
static Type Func;
static Type AssignopExp;
static char* currentvarname;
static char* currentfuncname;

static void AddWriteAndRead();
static void InitDeclareFuncList();
static void AddDeclareFunc(char* funcname,int line);
static void IsExistNoDefFunc();
static void InitStack();
static void InitStandType();
static Type Top();
static Type UnderTop();
static void Push(Type specifier);
static void Pop();
static void PrintError(int ErrorType, int line, char* str);
static void AnalysisVarDec(struct node* Node);
static void AnalysisDec(struct node* Node);
static void AnalysisDecList(struct node* Node, int kind);
static void AnalysisDef(struct node* Node, int kind);
static void AnalysisDefList(struct node* Node, int kind);
static void AnalysisStructSpecifier(struct node* Node);
static void AnalysisSpecifier(struct node* Node);
static void AnalysisExtDecList(struct node* Node);
static void AnalysisParamDec(struct node* Node);
static void AnalysisVarList(struct node* Node);
static void AnalysisFunDec(struct node* Node, int kind);
static void AnalysisCompSt(struct node* Node);
static void AnalysisExtDef(struct node* Node);
static void AnalysisExtDefList(struct node* Node);
static Type AnalysisExp(struct node* Node);
static void AnalysisStmt(struct node* Node);
static void AnalysisStmtList(struct node* Node);
static void AnalysisProgram(struct node* Node);
void SemanticAnalysis(struct node* Node);

static void AddWriteAndRead() {
	Type temp = InitFunDec();
	Type IntType = CreateBasic("int", NOLEFTVALUE);
	SetFunDecReturn(temp, IntType);
	InsertDefFunc("read", temp);
	temp = InitFunDec();
	AppendFunDecParam(temp, CreateParam(IntType));
	SetFunDecReturn(temp, IntType);
	InsertDefFunc("write", temp);
}

static void InitDeclareFuncList() {
	declarefunclist = malloc(sizeof(struct DeclareFuncList_));
	declarefunclist->next = NULL;
}

static void AddDeclareFunc(char* funcname,int line) {
	DeclareFuncList temp = malloc(sizeof(struct DeclareFuncList_));
	temp->funcname = funcname;
	temp->line = line;
	temp->next = declarefunclist->next;
	declarefunclist->next = temp;
}

static void IsExistNoDefFunc() {
	DeclareFuncList temp = declarefunclist->next;
	while (temp != NULL) {
		if (FindDefFunc(temp->funcname) == NULL) {
			PrintError(18, temp->line, "Undefined function");
		}
		temp = temp->next;
	}
}

static void InitStandType() {
	standtypeint = CreateBasic("int", NOLEFTVALUE);
	standtypefloat = CreateBasic("float", NOLEFTVALUE);
}

static void InitStack() {
	specifierstack = malloc(sizeof(struct SpecifierStack_));
	specifierstack->next = NULL;
	specifierstack->specifier = NULL;
}

static Type Top() {
	return specifierstack->specifier;
}

static Type UnderTop() {
	return specifierstack->next->specifier;
}

static void Push(Type specifier) {
	SpecifierStack temp = malloc(sizeof(struct SpecifierStack_));
	temp->next = specifierstack;
	temp->specifier = specifier;
	specifierstack = temp;
}

static void Pop() {
	SpecifierStack temp = specifierstack;
	specifierstack = specifierstack->next;
	free(temp);
}

static void PrintError(int ErrorType,int line,char*str) {
	iserror = 1;
	printf("Error type %d at Line %d: %s\n", ErrorType, line, str);
}

static void AnalysisVarDec(struct node* Node) {
	if (strcmp(Node->name, "ID") == 0) {
		currentvarname = GetIDName(Node);
	}
	if (strcmp(Node->name, "INT") == 0) {
		if (IsArray == 0) {
			IsArray = 1;
			Array = InitArray();
		}
		ArrayList elem = CreateArrayElem(GetIntValue(Node));
		AppendArrayElem(Array, elem);
	}
	if (Node->lson_node != NULL) {
		AnalysisVarDec(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisVarDec(Node->rbro_node);
	}
}

static void AnalysisDec(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "VarDec") == 0) {
		IsVisitLson = 0;
		AnalysisVarDec(Node->lson_node);
	}
	if (strcmp(Node->name, "Exp") == 0) {
		IsVisitLson = 0;
		AssignopExp = AnalysisExp(Node);
		hasAssign = 1;
	}
	if (IsVisitLson==1&&Node->lson_node != NULL) {
		AnalysisDec(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisDec(Node->rbro_node);
	}
}

static void AnalysisDecList(struct node* Node,int kind) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "Dec") == 0) {
		IsVisitLson = 0;
		AnalysisDec(Node->lson_node);
		if (IsArray == 1) {
			SetArrayType(Array, Top());
			if (hasAssign == 1) {
				hasAssign = 0;
				if (IsTypeEqual(AssignopExp, Array) == 0) {
					PrintError(7, Node->line, "Arithmetic operation type mismatch");
				}
			}
			if (kind == STRUCTSPECIFIERUSE) {
				if (FindStructFieldType(UnderTop(), currentvarname) != NULL) {
					PrintError(15, Node->line, "Redefined field name");
				}
				else {
					AppendStructureField(UnderTop(), CreateField(currentvarname, Array));
				}
			}
			else {
				if (InsertVar(currentvarname, Array) == 0) {
					PrintError(3, Node->line, "Redefined Var");
				}
			}
			IsArray = 0;
		}
		else {
			if (hasAssign == 1) {
				hasAssign = 0;
				if (IsTypeEqual(AssignopExp, Top()) == 0) {
					PrintError(7, Node->line, "Arithmetic operation type mismatch");
				}
			}
			if (kind == STRUCTSPECIFIERUSE) {
				if (FindStructFieldType(UnderTop(), currentvarname) !=NULL ) {
					PrintError(15, Node->line, "Redefined field name");
				}
				else {
					AppendStructureField(UnderTop(), CreateField(currentvarname, Top()));
				}
			}
			else {
				if (InsertVar(currentvarname, Top()) == 0) {
					PrintError(3, Node->line, "Redefined Var");
				}
			}
		}
	}
	if (IsVisitLson==1&&Node->lson_node != NULL) {
		AnalysisDecList(Node->lson_node,kind);
	}
	if (Node->rbro_node != NULL) {
		AnalysisDecList(Node->rbro_node,kind);
	}
}

static void AnalysisDef(struct node* Node,int kind) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "Specifier") == 0) {
		IsVisitLson = 0;
		AnalysisSpecifier(Node->lson_node);
	}
	if (strcmp(Node->name, "DecList") == 0) {
		IsVisitLson = 0;
		AnalysisDecList(Node->lson_node,kind);
	}
	if (IsVisitLson==1&&Node->lson_node != NULL) {
		AnalysisDef(Node->lson_node,kind);
	}
	if (Node->rbro_node != NULL) {
		AnalysisDef(Node->rbro_node,kind);
	}
}

static void AnalysisDefList(struct node* Node,int kind) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "Def") == 0) {
		IsVisitLson = 0;
		AnalysisDef(Node->lson_node,kind);
		Pop();
	}
	if (IsVisitLson==1&&Node->lson_node != NULL) {
		AnalysisDefList(Node->lson_node,kind);
	}
	if (Node->rbro_node != NULL) {
		AnalysisDefList(Node->rbro_node,kind);
	}
}

static void AnalysisStructSpecifier(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "STRUCT") == 0) {
		Specifier = InitStructure();
	}
	if (strcmp(Node->name, "OptTag") == 0) {
		if (strcmp(Node->lson_node->name, "ID") == 0) {
			SetStructureName(Specifier, GetIDName(Node->lson_node));
		}
		Push(Specifier);
	}
	if (strcmp(Node->name, "Tag") == 0) {
		if (IsExist(GetIDName(Node->lson_node), structhashtable)==0) {
			PrintError(17, Node->line, "Undefined struct ");
		}
		else {
			Specifier = FindStruct(GetIDName(Node->lson_node));
		}
		Push(Specifier);
	}
	if (strcmp(Node->name, "DefList") == 0) {
		IsVisitLson = 0;
		AnalysisDefList(Node->lson_node,STRUCTSPECIFIERUSE);
		if (InsertStruct(Top()->structure.name, Top()) == 0) {
			PrintError(16, Node->line, "Duplicate struct name");
		}
	}
	if (IsVisitLson == 1 &&Node->lson_node != NULL) {
		AnalysisStructSpecifier(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisStructSpecifier(Node->rbro_node);
	}
}

static void AnalysisSpecifier(struct node* Node) {
	if (strcmp(Node->name, "TYPE") == 0) {
		Specifier = CreateBasic(Node->lexical_value.stringvalue,HASLEFTVALUE);
		Push(Specifier);
	}
	else {
		AnalysisStructSpecifier(Node->lson_node);
	}
}

static void AnalysisExtDecList(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "VarDec") == 0) {
		IsVisitLson = 0;
		AnalysisVarDec(Node->lson_node);
		if (IsArray == 1) {
			SetArrayType(Array, Top());
			if (InsertVar(currentvarname, Array) == 0) {
				PrintError(3, Node->line, "Redefined Var");
			}
			IsArray = 0;
		}
		else {
			if (InsertVar(currentvarname, Top())==0) {
				PrintError(3, Node->line, "Redefined Var");
			}
		}
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisExtDecList(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisExtDecList(Node->rbro_node);
	}
}

static void AnalysisParamDec(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "Specifier") == 0) {
		IsVisitLson = 0;
		AnalysisSpecifier(Node->lson_node);
	}
	if (strcmp(Node->name, "VarDec") == 0) {
		IsVisitLson = 0;
		AnalysisVarDec(Node->lson_node);
		if (IsArray == 1) {
			SetArrayType(Array, Top());
			AppendFunDecParam(Func, CreateParam(Array));
			if (InsertVar(currentvarname, Array) == 0) {
				PrintError(3, Node->line, "Redefined Var");
			}
			IsArray = 0;
		}
		else {
			AppendFunDecParam(Func, CreateParam(Top()));
			if (InsertVar(currentvarname, Top()) == 0) {
				PrintError(3, Node->line, "Redefined Var");
			}
		}
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisParamDec(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisParamDec(Node->rbro_node);
	}
}

static void AnalysisVarList(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "ParamDec") == 0) {
		IsVisitLson = 0;
		AnalysisParamDec(Node->lson_node);
		Pop();
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisVarList(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisVarList(Node->rbro_node);
	}
}

static void AnalysisFunDec(struct node* Node,int kind) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "ID") == 0) {
		currentfuncname = Node->lexical_value.stringvalue;
	}
	if (strcmp(Node->name, "VarList") == 0) {
		IsVisitLson = 0;
		AnalysisVarList(Node->lson_node);
	}
	if (strcmp(Node->name, "RP") == 0) {
		if (kind == DEFFUNC) {
			Type temp1 = FindDeclareFunc(currentfuncname);
			if (temp1 != NULL) {
				if (IsTypeEqual(temp1, Func) == 0) {
					PrintError(19, Node->line, "Inconsistent declaration of function");
				}
				else {
					if (InsertDefFunc(currentfuncname, Func) == 0) {
						PrintError(4, Node->line, "Redefined function");
					}
				}
			}
			else {
				if (InsertDefFunc(currentfuncname, Func) == 0) {
					PrintError(4, Node->line, "Redefined function");
				}
			}
		}
		else {
			if (InsertDeclareFunc(currentfuncname, Func) == 0) {
				PrintError(19, Node->line, "Inconsistent declaration of function");
			}
			else {
				AddDeclareFunc(currentfuncname, Node->line);
			}
		}
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisFunDec(Node->lson_node, kind);
	}
	if (Node->rbro_node != NULL) {
		AnalysisFunDec(Node->rbro_node, kind);
	}
}

static void AnalysisCompSt(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "DefList") == 0) {
		IsVisitLson = 0;
		AnalysisDefList(Node->lson_node,COMPSTUSE);
	}
	if (strcmp(Node->name, "Stmt") == 0) {
		IsVisitLson = 0;
		AnalysisStmt(Node->lson_node);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisCompSt(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisCompSt(Node->rbro_node);
	}
}

static void AnalysisExtDef(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "Specifier") == 0) {
		IsVisitLson = 0;
		AnalysisSpecifier(Node->lson_node);
	}
	if (strcmp(Node->name, "ExtDecList") == 0) {
		IsVisitLson = 0;
		AnalysisExtDecList(Node->lson_node);
	}
	if (strcmp(Node->name, "FunDec") == 0) {
		IsVisitLson = 0;
		Func = InitFunDec();
		SetFunDecReturn(Func, Top());
		if (strcmp(Node->rbro_node->name, "SEMI") == 0) {
			AnalysisFunDec(Node->lson_node,DECLAREFUNC);
		}
		else {
			AnalysisFunDec(Node->lson_node,DEFFUNC);
		}
	}
	if (strcmp(Node->name, "CompSt") == 0) {
		IsVisitLson = 0;
		AnalysisCompSt(Node->lson_node);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisExtDef(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisExtDef(Node->rbro_node);
	}
}

static void AnalysisExtDefList(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "ExtDef") == 0) {
		IsVisitLson = 0;
		AnalysisExtDef(Node->lson_node);
		Pop();
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisExtDefList(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisExtDefList(Node->rbro_node);
	}
}

static Type AnalysisExp(struct node* Node) {
	char str[30] = "";
	int index = 0;
	Type SonTree[10];
	struct node* current = Node->lson_node;
	if (current == NULL) {
		if (strcmp(Node->name, "ID") == 0) {
			Type temp;
			temp = FindVar(Node->lexical_value.stringvalue);
			if (temp == NULL) {
				temp = FindDefFunc(Node->lexical_value.stringvalue);
			}
			return temp;
		}
		else if (strcmp(Node->name, "INT") == 0) {
			return CreateBasic("int", NOLEFTVALUE);
		}
		else if (strcmp(Node->name, "FLOAT") == 0) {
			return CreateBasic("float", NOLEFTVALUE);
		}
		else {
			return CreateBasic(Node->name,NOLEFTVALUE);
		}
	}
	while (current != NULL) {
		strcat(str, current->name);
		SonTree[index] = AnalysisExp(current);
		current = current->rbro_node;
		index++;
	}
	if (strcmp(str, "INT") == 0) {
		return SonTree[0];
	}
	if (strcmp(str, "FLOAT") == 0) {
		return SonTree[0];
	}
	if (strcmp(str, "ID") == 0) {
		if (SonTree[0] == NULL) {
			PrintError(1, Node->line, "Undefined Var");
			return CreateBasic("int", HASLEFTVALUE);
		}
		return SonTree[0];
	}
	if (strcmp(str, "Exp") == 0) {
		Type FuncArgs = InitFunDec();
		CreateParamList(FuncArgs, CreateParam(SonTree[0]));
		return FuncArgs;
	}
	if (strcmp(str, "MINUSExp") == 0) {
		return SonTree[1];
	}
	if (strcmp(str, "NOTExp") == 0) {
		if (IsTypeEqual(SonTree[1], standtypeint)==0) {
			PrintError(7, Node->line, "Logical operations can only use int");
		}
		return CreateBasic("int",NOLEFTVALUE);
	}
	if (strcmp(str, "ExpANDExp") == 0|| strcmp(str, "ExpORExp") == 0) {
		if (IsTypeEqual(SonTree[0], standtypeint) == 0 || IsTypeEqual(SonTree[2], standtypeint) == 0) {
			PrintError(7, Node->line, "Logical operations can only use int");
		}
		return CreateBasic("int",NOLEFTVALUE);
	}
	if (strcmp(str, "ExpPLUSExp") == 0 || strcmp(str, "ExpMINUSExp") == 0 || strcmp(str, "ExpSTARExp") == 0 || strcmp(str, "ExpDIVExp") == 0) {
		if (ReturnTypeKind(SonTree[0])==BASIC&& ReturnTypeKind(SonTree[2]) == BASIC) {
			if (IsTypeEqual(SonTree[0],SonTree[2])==0) {
				PrintError(7, Node->line, "Arithmetic operation type mismatch");
			}
			return SonTree[0];
		}
		else {
			PrintError(7, Node->line, "Only int and float can participate in arithmetic operations");
			return CreateBasic("int",NOLEFTVALUE);
		}
	}
	if (strcmp(str, "ExpRELOPExp") == 0) {
		if (IsTypeEqual(SonTree[0], standtypeint) == 0 || IsTypeEqual(SonTree[2], standtypeint) == 0) {
			PrintError(7, Node->line, "Only int can participate in RELOP operations");
		}
		return CreateBasic("int", NOLEFTVALUE);
	}
	if (strcmp(str, "LPExpRP") == 0) {
		return SonTree[1];
	}
	if (strcmp(str, "ExpASSIGNOPExp") == 0) {
		if (IsTypeEqual(SonTree[0],SonTree[2])==0) {
			PrintError(5, Node->line, "'=' mismatched");
		}
		if (IsTypeHasLeftValue(SonTree[0])==0) {
			PrintError(6, Node->line, "var has not left value");
		}
		return SonTree[0];
	}
	if (strcmp(str, "IDLPRP") == 0) {
		if (SonTree[0] == NULL) {
			if (FindDeclareFunc(Node->lson_node->lexical_value.stringvalue) == NULL) {
				PrintError(2, Node->line, "Undefined function");
			}
			else {
				PrintError(18, Node->line, "Undefined function");
			}
			return CreateBasic("int", NOLEFTVALUE);
		}
		if (ReturnTypeKind(SonTree[0]) != FUNDEC) {
			PrintError(11, Node->line, "Object is not function");
			return CreateBasic("int", NOLEFTVALUE);
		}
		return ReturnFunDecReturnType(SonTree[0]);
	}
	if (strcmp(str, "ExpDOTID") == 0) {
		if (ReturnTypeKind(SonTree[0]) != STRUCTURE) {
			PrintError(13, Node->line,"Illegal use of \".\"");
		}
		else {
			char* fieldname = Node->lson_node->rbro_node->rbro_node->lexical_value.stringvalue;
			Type temp = FindStructFieldType(SonTree[0], fieldname);
			if (temp==NULL) {
				PrintError(14, Node->line, "Non-existent field");
			}
			else {
				return temp;
			}
		}
		return CreateBasic("int", HASLEFTVALUE);
	}
	if (strcmp(str, "ExpCOMMAArgs") == 0) {
		CreateParamList(SonTree[2], CreateParam(SonTree[0]));
		return SonTree[2];
	}
	if (strcmp(str, "IDLPArgsRP") == 0) {
		if (SonTree[0] == NULL) {
			if (FindDeclareFunc(Node->lson_node->lexical_value.stringvalue) == NULL) {
				PrintError(2, Node->line, "Undefined function");
			}
			else {
				PrintError(18, Node->line, "Undefined function");
			}
			return CreateBasic("int", NOLEFTVALUE);
		}
		if (ReturnTypeKind(SonTree[0]) != FUNDEC) {
			PrintError(11, Node->line, "Object is not function");
			return CreateBasic("int", NOLEFTVALUE);
		}
		SetFunDecReturn(SonTree[2], ReturnFunDecReturnType(SonTree[0]));
		if (IsTypeEqual(SonTree[0], SonTree[2]) == 0) {
			PrintError(9, Node->line, "Function is not applicable for arguments");
		}
		return ReturnFunDecReturnType(SonTree[0]);
	}
	if (strcmp(str, "ExpLBExpRB") == 0) {
		if (IsTypeEqual(SonTree[2],standtypeint)==0) {
			PrintError(12, Node->line, "Number is not an integer");
		}
		if (ReturnTypeKind(SonTree[0]) == ARRAY) {
			return VisitArray(SonTree[0]);
		}
		else {
			PrintError(10, Node->line, "Var is not an array");
		}
		return SonTree[0];
	}
}

static void AnalysisStmt(struct node* Node) {
	int IsVisitLson = 1;
	int IsVisitRson = 1;
	if (strcmp(Node->name, "Exp") == 0) {
		IsVisitLson = 0;
		AnalysisExp(Node);
	}
	if (strcmp(Node->name, "CompSt") == 0) {
		IsVisitLson = 0;
		AnalysisCompSt(Node->lson_node);
	}
	if (strcmp(Node->name, "RETURN") == 0) {
		IsVisitRson = 0;
		Type temp = AnalysisExp(Node->rbro_node);
		if (IsTypeEqual(temp, Func->fundec.ReturnType) == 0) {
			PrintError(8, Node->rbro_node->line, "Type mismatched for return");
		}
	}
	if (strcmp(Node->name, "LP") == 0) {
		IsVisitRson = 0;
		Type temp = AnalysisExp(Node->rbro_node);
		if (IsTypeEqual(temp, standtypeint) == 0) {
			PrintError(7, Node->rbro_node->line, "Only int can use in IF or WHILE");
		}
		struct node* tempnode = Node->rbro_node->rbro_node->rbro_node;
		AnalysisStmt(tempnode->lson_node);
		if (tempnode->rbro_node != NULL) {
			AnalysisStmt(tempnode->rbro_node->rbro_node->lson_node);
		}
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisStmt(Node->lson_node);
	}
	if (IsVisitRson==1&&Node->rbro_node != NULL) {
		AnalysisStmt(Node->rbro_node);
	}
}

static void AnalysisStmtList(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "Stmt") == 0) {
		IsVisitLson = 0;
		AnalysisStmt(Node->lson_node);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisStmtList(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisStmtList(Node->rbro_node);
	}
}

static void AnalysisProgram(struct node* Node) {
	int IsVisitLson = 1;
	if (strcmp(Node->name, "ExtDefList") == 0) {
		IsVisitLson = 0;
		AnalysisExtDefList(Node->lson_node);
	}
	if (IsVisitLson == 1 && Node->lson_node != NULL) {
		AnalysisProgram(Node->lson_node);
	}
	if (Node->rbro_node != NULL) {
		AnalysisProgram(Node->rbro_node);
	}
}

void SemanticAnalysis(struct node* Node) {
	if (IsInitStandType == 0) {
		IsInitStandType = 1;
		InitStandType();
	}
	if (IsInitStack == 0) {
		IsInitStack = 1;
		InitStack();
	}
	if (IsInitHashTable == 0) {
		IsInitHashTable = 1;
		InitHashTable();
	}
	if (IsInitDeclareFuncList == 0) {
		IsInitDeclareFuncList = 1;
		InitDeclareFuncList();
	}
	if (IsAddFunc == 0) {
		IsAddFunc = 1;
		AddWriteAndRead();
	}
	AnalysisProgram(Node->lson_node);
	IsExistNoDefFunc();
}