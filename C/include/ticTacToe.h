#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H

#include <stdlib.h>
#include <stdbool.h>

typedef enum symbol_enum {
  EMPTY = ' ',
  X = 'X',
  O = 'O',
  D = 'D'
} Symbol;

typedef struct ticTacToe_s TicTacToe;

TicTacToe *ttt_create(size_t boardSize);
void ttt_delete(TicTacToe *game);

bool ttt_makeMove(TicTacToe *game, char player, int row, int col);

bool ttt_checkWin(TicTacToe *game, char player);
bool ttt_checkDraw(TicTacToe *game);

size_t ttt_getBoardSize(TicTacToe *game);
bool ttt_isGameOver(TicTacToe *game);

void ttt_displayBoard(TicTacToe *game);
void ttt_displayResult(TicTacToe *game);

#endif // TIC_TAC_TOE_H