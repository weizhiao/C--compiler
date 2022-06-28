%option noyywrap
%option yylineno
%{
	#include <stdlib.h>
	#include "syntax.tab.h"	
	#include "wordtype.h"
	int yycolumn = 1;
    	#define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;

	static void yyerror(char* s);
	extern int iserror;
%}

digit [0-9]
letter [_a-zA-Z]
character [_a-zA-Z0-9]
eight [0-7]
sixteen [0-9a-fA-F]
eight_digit 0{eight}*
sixteen_digit 0(x|X){sixteen}+
float_exp (([0-9]+".")|("."[0-9]+)|([0-9]+"."[0-9]+))(e|E)("-"|"+")?[0-9]+

%%
";" {return SEMI;}
"," {return COMMA;}
"=" {return ASSIGNOP;}
">"|"<"|">="|"<="|"=="|"!=" {yylval.word = CreateWord(OTHER_TYPE,yytext);return RELOP;}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return STAR;}
"/" {return DIV;}
"&&" {return AND;}
"||" {return OR;}
"." {return DOT;}
"!" {return NOT;}
int|float {yylval.word = CreateWord(OTHER_TYPE,yytext);return TYPE;}
"(" {return LP;}
")" {return RP;}
"[" {return LB;}
"]" {return RB;}
"{" {return LC;}
"}" {return RC;}
struct {return STRUCT;}
return {return RETURN;}
if {return IF;}
else {return ELSE;}
while {return WHILE;}
"/*"[^"*/"]*"*/" {}
"//".*\n {}
{float_exp} {yylval.word = CreateWord(TYPEFLOAT,yytext);return FLOAT;}
{sixteen_digit} {yylval.word = CreateWord(SIXTEEN,yytext);return INT;}
{eight_digit} {yylval.word = CreateWord(EIGHT,yytext);return INT;}
[1-9]{digit}*/[^"."] {yylval.word = CreateWord(TYPEINT,yytext);return INT;}
({digit}+)"."({digit}+) {yylval.word = CreateWord(TYPEFLOAT,yytext);return FLOAT;}
{letter}{character}* {yylval.word = CreateWord(TYPESTRING,yytext);return ID;}
[ \t] {;}
\n {}
. {iserror=1;yyerror("");}
%%
void yyerror(char* s)
{
    fprintf(stderr, "Error type A at Line %d: %s\n",yylineno,s);
}