#include "wordtype.h"

static int mypow(int a, int b) {
  int value = 1;
  for (int i = 0; i < b; i++) {
    value = value * a;
  }
  return value;
}

struct WordType *CreateWord(int type, char *s) {
  struct WordType *word = malloc(sizeof(struct WordType));
  switch (type) {
  case TYPEINT:
    word->value.intvalue = atoi(s);
    word->type = TYPEINT;
    break;
  case TYPEFLOAT:
    word->value.floatvalue = strtod(s, NULL);
    word->type = TYPEFLOAT;
    break;
  case TYPESTRING:
    word->value.stringvalue = strdup(s);
    word->type = TYPESTRING;
    break;
  case OTHER_TYPE:
    word->value.stringvalue = strdup(s);
    word->type = OTHER_TYPE;
    break;
  case EIGHT: {
    int value = 0;
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
      int temp = s[i] - '0';
      value += temp * mypow(8, len - i - 1);
    }
    word->value.intvalue = value;
    word->type = TYPEINT;
  } break;
  case SIXTEEN: {
    int value = 0;
    int len = strlen(s);
    for (int i = 2; i < len; i++) {
      int temp;
      if (s[i] <= '9') {
        temp = s[i] - '0';
      } else if (s[i] <= 'F') {
        temp = s[i] - 'A' + 10;
      } else {
        temp = s[i] - 'a' + 10;
      }
      value += temp * mypow(16, len - i - 1);
    }
    word->value.intvalue = value;
    word->type = TYPEINT;
  } break;
  }
  return word;
}
