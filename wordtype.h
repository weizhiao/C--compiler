#ifndef _WORDTYPE_H_
#define _WORDTYPE_H_

#include<stdlib.h>
#include<string.h>

#define TYPEINT 2
#define TYPEFLOAT 3
#define TYPESTRING 4
#define OTHER_TYPE 5
#define EIGHT 6
#define SIXTEEN 7

struct WordType {
	int type;
	union {
		int intvalue;
		float floatvalue;
		char* stringvalue;
	}value;
};

struct WordType* CreateWord(int type, char* s);

#endif // !_WORDTYPE_H_
