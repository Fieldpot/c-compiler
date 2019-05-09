#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型を表す値
enum {
  ND_NUM = 256, // 整数トークン
  ND_IDENT,
  ND_RETURN,
};

// トークンの型を表す値
enum {
  TK_NUM = 256, // 整数トークン
  TK_IDENT,
  TK_RETURN,
  TK_EOF,
};

typedef struct{
  int ty;       // トークンの型
  int val;      // tyがTK_NUMの場合、その数値
  char *name;
  char *input;  // トークン文字列(エラーメッセージ用)
} Token;

typedef struct Node{
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
  char name;
} Node;

typedef struct{
  void **data;
  int capacity;
  int len;
} Vector;

typedef struct{
  Vector *keys;
  Vector *vals;
} Map;

Node *mul();
Node *term();
Node *add();
Vector *new_vector();
Node *new_node(int ty, Node *lhs, Node *rhs);
void vec_push(Vector * vec, void *elem);
void error(char *fmt, ... );

extern int pos;
extern Token tokens[100];
extern Node *code[100];
