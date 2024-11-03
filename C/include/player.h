#ifndef PLAYER_H
#define PLAYER_H

#include "ticTacToe.h"

typedef struct player_s Player;

typedef enum strategy_enum {
  SEQUENTIAL = 1,
  RANDOM
} Strategy;

Player *pl_create(TicTacToe *game, char symbol, Strategy strategy);
void pl_delete(Player *player);

void *pl_play(void *player);

#endif // PLAYER_H