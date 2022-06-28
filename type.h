#ifndef _TYPE_H_
#define _TYPE_H_

#include<stdlib.h>
#include<string.h>

#define NOLEFTVALUE 0
#define HASLEFTVALUE 1
#define BASIC 1
#define ARRAY 2
#define STRUCTURE 3
#define FUNDEC 4

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct ArrayList_* ArrayList;
typedef struct Param_* Param;

struct FieldList_
{
	char* name;
	Type type;
	FieldList next;
};

struct ArrayList_
{
	int size;
	ArrayList next;
};

struct Param_ {
	Type param;
	Param next;
};

struct Type_ 
{
	int hasleftvalue;
	int kind;
	union 
	{
		char* basic;
		struct { Type arraytype; ArrayList list; } array;
		struct { char* name; FieldList list; } structure;
		struct { Type ReturnType; Param Params; }fundec;
	};
};

Type standtypeint;
Type standtypefloat;

Type CreateBasic(char* basic,int hasleftvalue);
FieldList CreateField(char* name, Type type);
ArrayList CreateArrayElem(int size);
Param CreateParam(Type type);
Type VisitArray(Type Array);
Type InitStructure();
Type InitArray();
Type InitFunDec();
Type FindStructFieldType(Type structure, char* name);
void AppendStructureField(Type structure, FieldList field);
void AppendArrayElem(Type array, ArrayList elem);
void AppendFunDecParam(Type fundec, Param param);
void CreateParamList(Type FuncArgs,Param param);
void SetArrayType(Type array,Type type);
void SetStructureName(Type structure, char* name);
void SetFunDecReturn(Type fundec,Type ReturnType);
int IsTypeEqual(Type type1,Type type2);
int ReturnTypeKind(Type type);
Type ReturnFunDecReturnType(Type fundec);
int IsTypeHasLeftValue(Type type);
int ComputeTypeSize(Type type);
int ComputeFieldAddr(Type structure,char* fieldname);

#endif // !_TYPE_H_
