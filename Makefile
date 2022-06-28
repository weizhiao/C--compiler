all: calc


calc: lexical.l syntax.y
	bison -d syntax.y
	flex lexical.l
	gcc syntax.tab.c lex.yy.c main.c syntaxtree.c analysis.c hashtable.c type.c wordtype.c translate.c stmtlist.c -o parser

testall:
	./parser 2.1
	./parser 2.2
	./parser 2.3
	./parser 2.4
	./parser 2.5
	./parser 2.6
	./parser 2.7
	./parser 2.8
	./parser 2.9
	./parser 2.10
	./parser 2.11
	./parser 2.12
	./parser 2.13
	./parser 2.14
	./parser 2.15
	./parser 2.16
	./parser 2.17
testnecessary:
	./parser 2.1
	./parser 2.3
	./parser 2.5
	./parser 2.6
	./parser 2.7
	./parser 2.10
	./parser 2.12
testselect:
	./parser 2.2
	./parser 2.4
	./parser 2.8
	./parser 2.9
	./parser 2.11
	./parser 2.13
	./parser 2.14
	./parser 2.15
	./parser 2.16
	./parser 2.17
