#include <time.h>

#include "player.h"
#include "msgassert.h"

struct player_s {
  TicTacToe *game;
  Strategy strategy;
  char symbol;
  int row;
  int col;
};

static void playSequential(Player *player);
static void playRandom(Player *player);
static int getRandomInt(int min, int max);

Player *pl_create(TicTacToe *game, char symbol, Strategy strategy) {
  Player *player = (Player*)malloc(sizeof(Player));
  msgassert(player != NULL, "Failed to allocate memory for the player: { symbol: %c, strategy: %d }", symbol, strategy);

  player->game = game;
  player->strategy = strategy;
  player->symbol = symbol;
  player->row = -1;
  player->col = -1;

  return player;
};
void pl_delete(Player *player) {
  if (!player) return;

  free(player);
};

void *pl_play(void *player) {
  Player *p = player;
  while(!ttt_isGameOver(p->game)) {
    if (p->strategy == SEQUENTIAL) playSequential(p);
    else playRandom(p);
  }  
};

static int getRandomInt(int min, int max) {
	srand((unsigned) time(NULL));

  return min + (rand() % max);
}

static void playSequential(Player *player) {
  do {
    player->col = (player->col + 1) % 3;
    if (player->col == 0) player->row = (player->row + 1) % 3;
  } while(!ttt_makeMove(player->game, player->symbol, player->row, player->col));
};
static void playRandom(Player *player) {
  do {
    player->col = getRandomInt(0, ttt_getBoardSize(player->game));
    player->row = getRandomInt(0, ttt_getBoardSize(player->game));
  } while(!ttt_makeMove(player->game, player->symbol, player->row, player->col));
};