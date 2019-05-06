//#include "9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型を表す値
enum {
  ND_NUM = 256, // 整数トークン
};

// トークンの型を表す値
enum {
  TK_NUM = 256, // 整数トークン
  TK_IDENT,
  TK_EOF,
};

typedef struct{
  int ty;       // トークンの型
  int val;      // tyがTK_NUMの場合、その数値
  char *input;  // トークン文字列(エラーメッセージ用)
} Token;

typedef struct Node{
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
} Node;

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

// 関数プロトタイプを書いておく
Node *mul();
Node *term();
Vector *new_vector();
void vec_push(Vector * vec, void *elem);
void error(char *fmt, ... );


Token tokens[100];
Node *code[100];
int pos = 0;

Node *assign(){
  Node *node = add();
  while(consume('='))
    node = new_node('=', node, assign());
  return node;
}

Node *stmt(){
  Node *node = assign();
  if(!consume(';'))
    error("';'ではないトークンです: %s", tokens[pos].input);
  return node;
}

void program(){
  int i = 0;
  while (tokens[pos].ty != TK_EOF)
    code[i++] = stmt();
  code[i] = NULL;
}

int expect(int line, int expected, int actual){
  if(expected == actual)
    return 0;
  fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
  exit(1);
}

void runtest(){
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for(int i = 0; i < 100; i++)
    vec_push(vec, (void *)i);

  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0, (int)vec->data[0]);
  expect(__LINE__, 50, (int)vec->data[50]);
  expect(__LINE__, 99, (int)vec->data[99]);

  printf("OK\n");

}

Vector *new_vector(){
  Vector *vec = malloc(sizeof(Vector));
  vec->data = malloc(sizeof(void *) * 16);
  vec->capacity = 16;
  vec->len = 0;
  return vec;
}

void vec_push(Vector * vec, void *elem){
  if(vec->capacity == vec->len){
    vec->capacity *= 2;
    vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
  }
  vec->data[vec->len++] = elem;
}

Node *new_node(int ty, Node *lhs, Node *rhs){
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val){
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

int consume(int ty){
  if(tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

Node *add(){
  Node *node = mul();

  for(;;){
    if (consume('+'))
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

Node *mul(){
  Node *node = term();

  for(;;){
    if (consume('*'))
      node = new_node('*', node, term());
    else if (consume('/'))
      node = new_node('/', node, term());
    else
      return node;
  }
}

Node *term(){
  if (consume('(')){
    Node *node = add();
    if (!consume(')'))
      error("開きカッコに対応する閉じカッコがありません: %s",tokens[pos].input);
    return node;
  }

  if (tokens[pos].ty == ND_NUM)
    return new_node_num(tokens[pos++].val);

  error("数値でも開きカッコでもないトークンです: %s", tokens[pos].input);
}

void gen_lval(Node *node){
  if(node->ty != ND_IDENT)
    error("代入の左辺値が変数ではありません");
  int offset = ('z' - node->name + 1) * 8;
  printf("  mov rax, rdp\n");
  printf("  sub rax, %d\n", offset);
  printf("  push rax\n");
}

void gen(Node * node){
  if (node->ty == ND_NUM){
    printf("  push %d\n", node->val);
    return;
  }

  if(node->ty == ND_IDENT){
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if(node->ty == '='){
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->ty){
  case '+':
    printf("  add rax, rdi\n");
    break;
  case '-':
    printf("  sub rax, rdi\n");
    break;
  case '*':
    printf("  mul rdi\n");
    break;
  case '/':
    printf("  mov rdx, 0\n");
    printf("  div rdi\n");
    break;

  }
    printf("  push rax\n");
    
}


void error(char *fmt, ... ){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void tokenize(char *p){
  int i = 0;
  while(*p){
    if (isspace(*p)){
      p++;
      continue;
    }

    if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if('a' <= *p && *p <= 'z'){
      tokens[i].ty = TK_IDENT;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)){
      tokens[i].ty = ND_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    error("トークナイズできません: %s", p);
    exit(1);
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

int main(int argc, char **argv){
  if(argc != 2){
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  if(strcmp(argv[1], "-test")==0){
    runtest();
    return 0;
  }

  tokenize(argv[1]);
  program();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  //プロローグ
  //変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  for(int i = 0; code[i]; i++){
    gen(code[i]);

    // 式の評価結果としてスタックに１つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf("  pop rax\n");
  }

  // それをRAXにロードして関数からの返り値とする
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
