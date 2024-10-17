#pragma once

#define MAXROWS 20
#define WIDTH 5
#define HEIGHT 6

struct st {
   char board[MAXROWS][WIDTH];
};
typedef struct st state;

bool initialise(state *s, const char *data);
bool matches(state *s);
bool dropblocks(state *s);
bool tostring(const state *s, char *str);
void test();