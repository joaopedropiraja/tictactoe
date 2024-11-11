#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "ticTacToe.h"
#include "player.h"
#include "msgassert.h"

int main(int argc, char *argv[]) {
  size_t boardSize = 2;
  TicTacToe *game = ttt_create(boardSize);
  Player *player1 = pl_create(game, X, RANDOM);
  Player *player2 = pl_create(game, O, RANDOM);

  pthread_t t1;
  pthread_create(&t1, NULL, pl_play, (void*)player1);
  pthread_t t2;
  pthread_create(&t2, NULL, pl_play, (void*)player2);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  ttt_displayResult(game);

  ttt_delete(game);
  pl_delete(player1);
  pl_delete(player2);

  return EXIT_SUCCESS;
}
