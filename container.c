#include "9cc.h"

// プロトタイプ宣言
Map *new_map();
void *map_get(Map *map, char *key);

// この辺はテストコード
int expect(int line, int expected, int actual){
  if(expected == actual)
    return 0;
  fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
  exit(1);
}

void test_vector(){
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for(int i = 0; i < 100; i++)
    vec_push(vec, (void *)i);

  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0, (int)vec->data[0]);
  expect(__LINE__, 50, (int)vec->data[50]);
  expect(__LINE__, 99, (int)vec->data[99]);
  printf("test_vector_ok!!\n");
}

void test_map(){
  Map *map = new_map();
  expect(__LINE__, 0, (int)map_get(map, "foo"));

  map_put(map, "foo", (void *)2);
  expect(__LINE__, 2, (int)map_get(map, "foo"));

  map_put(map, "bar", (void *)4);
  expect(__LINE__, 4, (int)map_get(map, "bar"));

  map_put(map, "foo", (void *)6);
  expect(__LINE__, 6, (int)map_get(map, "foo"));

  printf("test_map_ok!!\n");
}

void runtest(){
  test_vector();
  test_map();
}

// Vector系のコード
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

// この辺はMap系のコード
Map *new_map(){
  Map *map = malloc(sizeof(Map));
  map->keys = new_vector();
  map->vals = new_vector();
  return map;
}

void map_put(Map *map, char *key, void *val){
  vec_push(map->keys, key);
  vec_push(map->vals, val);
}

void *map_get(Map *map, char *key){
  for(int i = map->keys->len -1; i >= 0; i--)
    if(strcmp(map->keys->data[i], key) == 0)
      return map->vals->data[i];
  return NULL;
}

// この辺はノード系のコード
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
