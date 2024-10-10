%locations
%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "syntaxtree.h"
	extern struct node *boot;
	extern int yylineno;
	extern int yylex();
	extern int yyrestart();
	extern int iserror;
	static void yyerror(char* s);
	static char ErrorType = 'B';
%}
%union{
	struct WordType* word;
	struct node* type_node;
}

%token <word> INT
%token <word> FLOAT
%token <word> ID
%token <word> TYPE
%token <word> RELOP
%token SEMI COMMA ASSIGNOP PLUS MINUS STAR DIV AND OR DOT NOT LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%nonassoc error
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left ADD SUB
%left STAR DIV
%right NOT UMINUS
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%type <type_node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier
%type <type_node> OptTag Tag VarDec FunDec VarList ParamDec CompSt
%type <type_node> StmtList Stmt DefList Def DecList Dec Exp Args

%%
Program:ExtDefList{
boot = InitTree("Program",@$.first_line,$1);
$$ = boot;
}
|ExtDefList error{iserror=1;}
;
ExtDefList:ExtDefList ExtDef{
$$= InitTree("ExtDefList",@$.first_line,$1);
BuildrTree($1,$2);
}
| ExtDefList error SEMI{ yyerrok;iserror = 1; }
| {
struct node* temp1 = CreateNode(EMPTY, "EMPTY", NULL);
$$ = InitTree("ExtDefList", @$.first_line,temp1);
}
;
ExtDef:Specifier ExtDecList SEMI{
$$ = InitTree("ExtDef",@$.first_line,$1);
struct node* temp1 = CreateNode(OTHER,"SEMI",NULL);
BuildrTree($1,$2);
BuildrTree($2,temp1);
}
| Specifier SEMI{
$$ = InitTree("ExtDef",@$.first_line,$1);
struct node* temp1 = CreateNode(OTHER,"SEMI",NULL);
BuildrTree($1,temp1);
}
| Specifier FunDec CompSt{
$$ = InitTree("ExtDef",@$.first_line,$1);
BuildrTree($1,$2);
BuildrTree($2,$3);
}
| Specifier FunDec SEMI{
struct node* temp1 = CreateNode(OTHER,"SEMI",NULL);
$$ = InitTree("ExtDef",@$.first_line,$1);
BuildrTree($1,$2);
BuildrTree($2,temp1);
}
;
ExtDecList:VarDec{
$$ = InitTree("ExtDecList",@$.first_line,$1);
}
| VarDec COMMA ExtDecList{
$$ = InitTree("ExtDecList",@$.first_line,$1);
struct node* temp1 = CreateNode(OTHER,"COMMA",NULL);
BuildrTree($1,temp1);
BuildrTree(temp1,$3);
}
;
Specifier:TYPE{
struct node* temp1 = CreateNode(OTHER_TYPE,"TYPE",$1);
$$ = InitTree("Specifier", @$.first_line, temp1);
}
| StructSpecifier{
$$ = InitTree("Specifier", @$.first_line, $1);
}
;
StructSpecifier:STRUCT OptTag LC DefList RC{
struct node* temp1 = CreateNode(OTHER,"STRUCT",NULL);
struct node* temp2 = CreateNode(OTHER,"LC",NULL);
struct node* temp3 = CreateNode(OTHER,"RC",NULL);
$$ = InitTree("StructSpecifier", @$.first_line, temp1);
BuildrTree(temp1,$2);
BuildrTree($2,temp2);
BuildrTree(temp2,$4);
BuildrTree($4,temp3);
}
| STRUCT Tag{
struct node* temp1 = CreateNode(OTHER,"STRUCT",NULL);
$$ = InitTree("StructSpecifier", @$.first_line, temp1);
BuildrTree(temp1,$2);
}
;
OptTag:ID{
struct node* temp1 = CreateNode(TYPESTRING,"ID",$1);
SetLine(temp1,@1.first_line);
$$ = InitTree("OptTag", @$.first_line, temp1);
}
| {
struct node* temp1 = CreateNode(EMPTY,  "EMPTY", NULL);
$$ = InitTree("OptTag", @$.first_line, temp1);
}
;
Tag:ID{
struct node* temp1 = CreateNode(TYPESTRING,"ID",$1);
SetLine(temp1,@1.first_line);
$$ = InitTree("Tag", @$.first_line, temp1);
}
;
VarDec:ID{
struct node* temp1 = CreateNode(TYPESTRING,"ID",$1);
SetLine(temp1,@1.first_line);
$$ = InitTree("VarDec", @$.first_line, temp1);
}
| VarDec LB INT RB
{
struct node* temp1 = CreateNode(OTHER,"LB",NULL);
struct node* temp2 = CreateNode(TYPEINT,"INT",$3);
struct node* temp3 = CreateNode(OTHER,"RB",NULL);
$$ = InitTree("VarDec", @$.first_line, $1);
BuildrTree($1,temp1);
BuildrTree(temp1,temp2);
BuildrTree(temp2,temp3);
}
|error RB{yyerror("missing ']'");iserror=1;} 
;
FunDec:ID LP VarList RP{
struct node* temp1 = CreateNode(TYPESTRING,"ID",$1);
SetLine(temp1,@1.first_line);
struct node* temp2 = CreateNode(OTHER,"LP",NULL);
struct node* temp3 = CreateNode(OTHER,"RP",NULL);
SetLine(temp3,@4.first_line);
$$ = InitTree("FunDec", @$.first_line, temp1);
BuildrTree(temp1,temp2);
BuildrTree(temp2,$3);
BuildrTree($3,temp3);
}
| ID LP RP{
struct node* temp1 = CreateNode(TYPESTRING,"ID",$1);
SetLine(temp1,@1.first_line);
struct node* temp2 = CreateNode(OTHER,"LP",NULL);
struct node* temp3 = CreateNode(OTHER,"RP",NULL);
SetLine(temp3,@3.first_line);
$$ = InitTree("FunDec", @$.first_line, temp1);
BuildrTree(temp1,temp2);
BuildrTree(temp2,temp3);
}
;
VarList:ParamDec COMMA VarList{
struct node* temp1 = CreateNode(OTHER,"COMMA",NULL);
$$ = InitTree("VarList", @$.first_line, $1);
BuildrTree($1,temp1);
BuildrTree(temp1,$3);
}
| ParamDec{
$$ = InitTree("VarList", @$.first_line, $1);
}
;
ParamDec:Specifier VarDec{
$$ = InitTree("ParamDec", @$.first_line, $1);
BuildrTree($1,$2);
}
;
CompSt:LC DefList StmtList RC{
struct node* temp1 = CreateNode(OTHER,"LC",NULL);
struct node* temp2 = CreateNode(OTHER,"RC",NULL);
$$ = InitTree("CompSt", @$.first_line, temp1);
BuildrTree(temp1,$2);
BuildrTree($2,$3);
BuildrTree($3,temp2);
}
;
StmtList:StmtList Stmt{
$$ = InitTree("StmtList", @$.first_line, $1);
BuildrTree($1,$2);
}
| {
struct node* temp1 = CreateNode(EMPTY, "EMPTY", NULL);
$$ = InitTree("StmtList", @$.first_line, temp1);
}
;
Stmt:Exp SEMI{
struct node* temp1 = CreateNode(OTHER,"SEMI",NULL);
$$ = InitTree("Stmt", @$.first_line, $1);
BuildrTree($1,temp1);
}
| CompSt{
$$ = InitTree("Stmt", @$.first_line, $1);
}
| RETURN Exp SEMI{
struct node* temp1 = CreateNode(OTHER,"RETURN",NULL);
struct node* temp2 = CreateNode(OTHER,"SEMI",NULL);
$$ = InitTree("Stmt", @$.first_line, temp1);
BuildrTree(temp1,$2);
BuildrTree($2,temp2);
}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{
struct node* temp1 = CreateNode(OTHER,"IF",NULL);
struct node* temp2 = CreateNode(OTHER,"LP",NULL);
struct node* temp3 = CreateNode(OTHER,"RP",NULL);
$$ = InitTree("Stmt", @$.first_line, temp1);
BuildrTree(temp1,temp2);
BuildrTree(temp2,$3);
BuildrTree($3,temp3);
BuildrTree(temp3,$5);
}
| IF LP Exp RP Stmt ELSE Stmt{
struct node* temp1 = CreateNode(OTHER,"IF",NULL);
struct node* temp2 = CreateNode(OTHER,"LP",NULL);
struct node* temp3 = CreateNode(OTHER,"RP",NULL);
struct node* temp4 = CreateNode(OTHER,"ELSE",NULL);
$$ = InitTree("Stmt", @$.first_line, temp1);
BuildrTree(temp1,temp2);
BuildrTree(temp2,$3);
BuildrTree($3,temp3);
BuildrTree(temp3,$5);
BuildrTree($5,temp4);
BuildrTree(temp4,$7);
}
| WHILE LP Exp RP Stmt{
struct node* temp1 = CreateNode(OTHER,"WHILE",NULL);
struct node* temp2 = CreateNode(OTHER,"LP",NULL);
struct node* temp3 = CreateNode(OTHER,"RP",NULL);
$$ = InitTree("Stmt", @$.first_line, temp1);
struct node* Nodes[] = { temp1,temp2,$3,temp3,$5 };
BuildTrees(Nodes, 5);
}
|Exp error {iserror=1;}
|IF LP error RP Stmt %prec LOWER_THAN_ELSE {iserror=1;}
|error SEMI {iserror=1;}
;
DefList:DefList Def{
$$ = InitTree("DefList", @$.first_line, $1);
struct node* Nodes[] = { $1,$2 };
BuildTrees(Nodes, 2);
}
| {
struct node* temp1 = CreateNode(EMPTY, "EMPTY", NULL);
$$ = InitTree("DefList", @$.first_line, temp1);
}
|error {iserror=1;}
;
Def:Specifier DecList SEMI{
struct node* temp1 = CreateNode(OTHER, "SEMI", NULL);
$$ = InitTree("Def", @$.first_line, $1);
struct node* Nodes[] = { $1,$2,temp1 };
BuildTrees(Nodes, 3);
}
|Specifier error SEMI {iserror=1; }
;
DecList:Dec{
$$ = InitTree("DecList", @$.first_line, $1);
}
| Dec COMMA DecList{
struct node* temp1 = CreateNode(OTHER, "COMMA", NULL);
$$ = InitTree("DecList", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
;
Dec:VarDec{
$$ = InitTree("Dec", @$.first_line, $1);
}
| VarDec ASSIGNOP Exp{
struct node* temp1 = CreateNode(OTHER, "ASSIGNOP", NULL);
$$ = InitTree("Dec", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
|error {iserror=1;}
;
Exp:Exp ASSIGNOP Exp{
struct node* temp1 = CreateNode(OTHER, "ASSIGNOP", NULL);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
| Exp AND Exp{
struct node* temp1 = CreateNode(OTHER, "AND", NULL);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
| Exp OR Exp{
struct node* temp1 = CreateNode(OTHER, "OR", NULL);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
| Exp RELOP Exp{
struct node* temp1 = CreateNode(OTHER_TYPE, "RELOP", $2);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
| Exp PLUS Exp{
struct node* temp1 = CreateNode(OTHER, "PLUS", NULL);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
| Exp MINUS Exp{
struct node* temp1 = CreateNode(OTHER, "MINUS", NULL);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
| Exp STAR Exp{
struct node* temp1 = CreateNode(OTHER, "STAR", NULL);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3 };
BuildTrees(Nodes, 3);
}
| LP Exp RP{
struct node* temp1 = CreateNode(OTHER, "LP", NULL);
struct node* temp2 = CreateNode(OTHER, "RP", NULL);
$$ = InitTree("Exp", @$.first_line, temp1);
struct node* Nodes[] = { temp1,$2,temp2 };
BuildTrees(Nodes, 3);
}
| MINUS Exp %prec UMINUS{
struct node* temp1 = CreateNode(OTHER, "MINUS", NULL);
$$ = InitTree("Exp", @$.first_line, temp1);
struct node* Nodes[] = { temp1,$2};
BuildTrees(Nodes, 2);
}
| NOT Exp{
struct node* temp1 = CreateNode(OTHER, "NOT", NULL);
$$ = InitTree("Exp", @$.first_line, temp1);
struct node* Nodes[] = { temp1,$2};
BuildTrees(Nodes, 2);
}
| ID LP Args RP{
struct node* temp1 = CreateNode(TYPESTRING, "ID", $1);
SetLine(temp1,@1.first_line);
struct node* temp2 = CreateNode(OTHER, "LP", NULL);
struct node* temp3 = CreateNode(OTHER, "RP", NULL);
$$ = InitTree("Exp", @$.first_line, temp1);
struct node* Nodes[] = { temp1,temp2,$3,temp3};
BuildTrees(Nodes, 4);
}
| ID LP RP{
struct node* temp1 = CreateNode(TYPESTRING, "ID", $1);
SetLine(temp1,@1.first_line);
struct node* temp2 = CreateNode(OTHER, "LP", NULL);
struct node* temp3 = CreateNode(OTHER, "RP", NULL);
$$ = InitTree("Exp", @$.first_line, temp1);
struct node* Nodes[] = { temp1,temp2,temp3};
BuildTrees(Nodes, 3);
}
| Exp LB Exp RB{
struct node* temp1 = CreateNode(OTHER, "LB", NULL);
struct node* temp2 = CreateNode(OTHER, "RB", NULL);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3,temp2};
BuildTrees(Nodes, 4);
}
| Exp DOT ID{
struct node* temp1 = CreateNode(OTHER, "DOT", NULL);
struct node* temp2 = CreateNode(TYPESTRING, "ID", $3);
SetLine(temp2,@3.first_line);
$$ = InitTree("Exp", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,temp2};
BuildTrees(Nodes, 3);
}
| ID{
struct node* temp1 = CreateNode(TYPESTRING, "ID", $1);
SetLine(temp1,@1.first_line);
$$ = InitTree("Exp", @$.first_line, temp1);
}
| INT{
struct node* temp1 = CreateNode(TYPEINT, "INT", $1);
$$ = InitTree("Exp", @$.first_line, temp1);
}
| FLOAT{
struct node* temp1 = CreateNode(TYPEFLOAT, "FLOAT", $1);
$$ = InitTree("Exp", @$.first_line, temp1);
}
|Exp LB error RB{iserror=1;}
;
Args:Exp COMMA Args{
struct node* temp1 = CreateNode(OTHER, "COMMA", NULL);
$$ = InitTree("Args", @$.first_line, $1);
struct node* Nodes[] = { $1,temp1,$3};
BuildTrees(Nodes, 3);
}
| Exp{
$$ = InitTree("Args", @$.first_line, $1);
}
;
%%

void yyerror(char* s)
{
	fprintf(stderr, "Error type %c at Line %d: %s\n", ErrorType, yylineno, s);
}
