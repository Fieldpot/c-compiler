#include "9cc.h"

int consume(int ty){
  if(tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

Node *assign(){
  Node *node = add();
  while(consume('='))
    node = new_node('=', node, assign());
  return node;
}

Node *unary(){
  if(consume('+'))
    return term();
  if(consume('-'))
    return new_node('-', new_node_num(0), term());
  return term();
}

Node *stmt(){
  Node *node;

  if (consume(TK_RETURN)){
    node = malloc(sizeof(Node));
    node->ty = ND_RETURN;
    node->lhs=assign();
  }else{
    node = assign();
  }
  if(!consume(';'))
    error("';'ではないトークンです: %s", tokens[pos].input);
  return node;
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

int is_alnum(char c){
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}

void program(){
  int i = 0;
  while (tokens[pos].ty != TK_EOF)
    code[i++] = stmt();
  code[i] = NULL;
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

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])){
      tokens[i].ty = TK_RETURN;
      tokens[i].input = p;
      i++;
      p += 6;
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
