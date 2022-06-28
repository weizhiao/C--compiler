#include"type.h"
#define BASICSIZE 4

Type ReturnFunDecReturnType(Type fundec) {
	return fundec->fundec.ReturnType;
}

int ReturnTypeKind(Type type) {
	return type->kind;
}

int IsTypeHasLeftValue(Type type) {
	if (type->hasleftvalue == HASLEFTVALUE) {
		return 1;
	}
	else {
		return 0;
	}
}

Type InitFunDec() {
	Type temp = malloc(sizeof(struct Type_));
	temp->kind = FUNDEC;
	temp->hasleftvalue = 0;
	temp->fundec.ReturnType = NULL;
	temp->fundec.Params = NULL;
	return temp;
}

Type VisitArray(Type Array) {
	Type temp;
	if (Array->array.list->next == NULL) {
		temp = Array->array.arraytype;
	}
	else {
		temp = InitArray();
		SetArrayType(temp, Array->array.arraytype);
		temp->array.list = Array->array.list->next;
	}
	return temp;
}

int ComputeTypeSize(Type type) {
	int sizenum = 1;
	ArrayList temp1;
	FieldList temp2;
	switch (type->kind) {
	case BASIC:
		sizenum = BASICSIZE;
		break;
	case ARRAY:
		temp1 = type->array.list;
		while (temp1 != NULL) {
			sizenum = sizenum * temp1->size;
			temp1 = temp1->next;
		}
		sizenum = sizenum * ComputeTypeSize(type->array.arraytype);
		break;
	case STRUCTURE:
		temp2 = type->structure.list;
		sizenum = 0;
		while (temp2 != NULL) {
			sizenum += ComputeTypeSize(temp2->type);
			temp2 = temp2->next;
		}
		break;
	}
	return sizenum;
}

Type CreateBasic(char* basic,int hasleftvalue) {
	Type temp = malloc(sizeof(struct Type_));
	temp->kind = BASIC;
	temp->hasleftvalue=hasleftvalue;
	temp->basic = basic;
	return temp;
}

FieldList CreateField(char* name, Type type) {
	FieldList temp = malloc(sizeof(struct FieldList_));
	temp->name = name;
	temp->type = type;
	temp->next = NULL;
	return temp;
}

ArrayList CreateArrayElem(int size) {
	ArrayList temp = malloc(sizeof(struct ArrayList_));
	temp->size = size;
	temp->next = NULL;
	return temp;
}

Type InitArray() {
	Type temp = malloc(sizeof(struct Type_));
	temp->hasleftvalue = 1;
	temp->kind = ARRAY;
	temp->array.arraytype = NULL;
	temp->array.list = NULL;
	return temp;
}

Type InitStructure() {
	Type temp = malloc(sizeof(struct Type_));
	temp->hasleftvalue = 1;
	temp->kind = STRUCTURE;
	temp->structure.list = NULL;
	temp->structure.name = NULL;
	return temp;
}

void AppendStructureField(Type structure, FieldList field) {
	FieldList current = structure->structure.list;
	if (current == NULL) {
		structure->structure.list = field;
	}
	else {
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = field;
	}
}

void AppendArrayElem(Type array, ArrayList elem) {
	ArrayList current = array->array.list;
	if (current == NULL) {
		array->array.list = elem;
	}
	else {
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = elem;
	}
}

void SetArrayType(Type array, Type type) {
	array->array.arraytype = type;
}

void SetStructureName(Type structure, char* name) {
	structure->structure.name = name;
}

int IsTypeEqual(Type type1, Type type2) {
	if (type1->kind == type2->kind) {
		if (type1->kind == BASIC) {
			if (strcmp(type1->basic, type2->basic) == 0) {
				return 1;
			}
		}
		if (type1->kind == ARRAY) {
			return IsTypeEqual(type1->array.arraytype, type2->array.arraytype);
		}
		if (type1->kind == STRUCTURE) {
			FieldList temp1 = type1->structure.list;
			FieldList temp2 = type2->structure.list;
			while (temp1!=NULL&&temp2!=NULL)
			{
				if (IsTypeEqual(temp1->type,temp2->type) == 0) {
					return 0;
				}
				temp1 = temp1->next;
				temp2 = temp2->next;
			}
			if (temp1 == NULL && temp2 == NULL) {
				return 1;
			}
		}
		if (type1->kind == FUNDEC) {
			if (IsTypeEqual(type1->fundec.ReturnType, type2->fundec.ReturnType) == 0) {
				return 0;
			}
			Param temp1 = type1->fundec.Params;
			Param temp2 = type2->fundec.Params;
			while (temp1 != NULL && temp2 != NULL)
			{
				if (IsTypeEqual(temp1->param, temp2->param) == 0) {
					return 0;
				}
				temp1 = temp1->next;
				temp2 = temp2->next;
			}
			if (temp1 == NULL && temp2 == NULL) {
				return 1;
			}
		}
	}
	return 0;
}

Param CreateParam(Type type) {
	Param temp = malloc(sizeof(struct Param_));
	temp->next = NULL;
	temp->param = type;
	return temp;
}

void AppendFunDecParam(Type fundec, Param param) {
	Param temp = fundec->fundec.Params;
	if (temp == NULL) {
		fundec->fundec.Params = param;
	}
	else {
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = param;
	}
}

void SetFunDecReturn(Type fundec, Type ReturnType) {
	fundec->fundec.ReturnType = ReturnType;
}

Type FindStructFieldType(Type structure, char* name){
	FieldList temp = structure->structure.list;
	while (temp != NULL) {
		if (strcmp(name, temp->name) == 0) {
			return temp->type;
		}
		temp = temp->next;
	}
	return NULL;
}

void CreateParamList(Type FuncArgs, Param param) {
	param->next = FuncArgs->fundec.Params;
	FuncArgs->fundec.Params = param;
}

int ComputeFieldAddr(Type structure, char* fieldname) {
	int addr = 0;
	FieldList temp = structure->structure.list;
	while (1) {
		if (strcmp(temp->name, fieldname) == 0) {
			break;
		}
		addr += ComputeTypeSize(temp->type);
		temp = temp->next;
	}
	return addr;
}