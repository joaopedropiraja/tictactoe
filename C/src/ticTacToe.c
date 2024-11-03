#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "ticTacToe.h"
#include "msgassert.h"

#define DELAY_PRINT 1

struct ticTacToe_s {
  pthread_mutex_t boardMutex;
  pthread_cond_t turnCv;

  size_t boardSize;
  char **board;
  char currentPlayer;
  bool gameOver;
  char winner;
};

TicTacToe *ttt_create(size_t boardSize) {
  TicTacToe *game = (TicTacToe*)malloc(sizeof(TicTacToe));
  msgassert(game != NULL, "Memory allocation failed for tic tac toe game: { boardSize: %zu }\n", boardSize);

  pthread_mutex_init(&game->boardMutex, NULL);
  pthread_cond_init(&game->turnCv, NULL);

  game->currentPlayer = X;
  game->gameOver = false;
  game->winner = EMPTY;

  game->boardSize = boardSize;
  game->board = (char **)malloc(boardSize * sizeof(char *));
  msgassert(game->board != NULL, "Memory allocation failed for tic tac toe game: { boardSize: %zu }\n", boardSize);

  for (size_t i = 0; i < boardSize; i++) {
    game->board[i] = (char *)malloc(boardSize * sizeof(char));
    msgassert(game != NULL, "Memory allocation failed for tic tac toe game: { boardSize: %zu }\n", boardSize);
    memset(game->board[i], EMPTY, boardSize);
  }

  return game;
};

void ttt_delete(TicTacToe *game) {
  if (!game) return;

  pthread_mutex_destroy(&game->boardMutex);
  pthread_cond_destroy(&game->turnCv);

  for (size_t i = 0; i < game->boardSize; i++) {
    free(game->board[i]);
  }
  free(game->board);
};

size_t ttt_getBoardSize(TicTacToe *game) {
  return game->boardSize;
};

bool ttt_makeMove(TicTacToe *game, char player, int row, int col) {
  pthread_mutex_lock(&game->boardMutex);
  while (game->currentPlayer != player) {
    pthread_cond_wait(&game->turnCv, &game->boardMutex);
  }

  if (game->gameOver) {
    pthread_mutex_unlock(&game->boardMutex);
    return true;
  }
  if (game->board[row][col] != EMPTY) {
    pthread_mutex_unlock(&game->boardMutex);
    return false;
  }

  ttt_displayBoard(game);
  game->board[row][col] = player;
  sleep(DELAY_PRINT);

  if (ttt_checkWin(game, player)) {
    game->winner = player;
    game->gameOver = true;
    ttt_displayBoard(game);
  } else if (ttt_checkDraw(game)) {
    game->winner = 'D';
    game->gameOver = true;
    ttt_displayBoard(game);
  }

  game->currentPlayer = game->currentPlayer == X ? O : X;
  pthread_mutex_unlock(&game->boardMutex);
  pthread_cond_signal(&game->turnCv);

  return true;
};

bool ttt_checkWin(TicTacToe *game, char player) {
  for (size_t i = 0; i < game->boardSize; i++) {
    bool rowWins = true;
    bool colWins = true;
    for (size_t j = 0; j < game->boardSize; j++) {
      if (game->board[i][j] != player) {
        rowWins = false;
      }

      if (game->board[j][i] != player) {
        colWins = false;
      }
    }

    if (rowWins || colWins) return true;
  }

  bool diagonal1 = true;
  bool diagonal2 = true;
  for (size_t i = 0, j = game->boardSize - 1; i < game->boardSize; i++, j--) {
    if (game->board[i][i] != player) {
      diagonal1 = false;
    }

    if (game->board[i][j] != player) {
      diagonal2 = false;
    }
  }

  return diagonal1 || diagonal2;  
};
bool ttt_checkDraw(TicTacToe *game) {
  for (size_t i = 0; i < game->boardSize; i++) {
    for (size_t j = 0; j < game->boardSize; j++) {
      if (game->board[i][j] == EMPTY) return false;
    }
  }

  return true;
};

bool ttt_isGameOver(TicTacToe *game) {
  return game->gameOver;
};

void ttt_displayBoard(TicTacToe *game) {
  system("clear");

  for (size_t i = 0; i < game->boardSize; i++) {
    for (size_t j = 0; j < game->boardSize; j++) {
      printf(" %c ", game->board[i][j]);

      if (j != (game->boardSize - 1)) {
        printf("|");
      }
    }

    if (i != (game->boardSize - 1)) {
      printf("\n===========\n");
    }
  }

  printf("\n\n");
  printf("Current player: %c\n\n", game->currentPlayer);
};
void ttt_displayResult(TicTacToe *game) {
  if (game->winner == 'D') {
    printf("Draw!\n");
  } else {
    printf("Player %c wins!\n", game->winner);
  }
};