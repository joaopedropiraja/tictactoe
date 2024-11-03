#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <chrono>
#include<cstdlib>


class Semaphore {
private:
  std::mutex m;
  std::condition_variable cv;
  int count;

public:
  Semaphore(int c): count(c) {}
  void wait() {
    std::unique_lock<std::mutex> lock(this->m);
    while(this->count <= 0) {
      this->cv.wait(lock);
    }
    this->count--;
  };
  void signal() {
    std::unique_lock<std::mutex> lock(this->m);
    this->count++;
    this->cv.notify_one();
  };
};

class Barrier {
private:
  std::mutex m;
  std::condition_variable cv;
  int desiredCount;
  int currCount;

public:
  Barrier(int c): desiredCount(c), currCount(0) {}
  
  void wait() {
    std::unique_lock<std::mutex> lock(this->m);
    this->currCount++;
    while (this->currCount < this->desiredCount) {
      this->cv.wait(lock);
    }

    this->cv.notify_all();
  };
};


enum Strategy {
  SEQUENTIAL = 1,
  RANDOM,
};

int getRandomInt(int min, int max) {
	srand((unsigned) time(NULL));

  return min + (rand() % max);
}

// Classe TicTacToe
class TicTacToe {
private:
  std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
  std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
  std::condition_variable turn_cv; // Variável de condição para alternância de turnos
  char current_player; // Jogador atual ('X' ou 'O')
  bool game_over; // Estado do jogo
  char winner; // Vencedor do jogo

public:
  TicTacToe() {
    this->winner = ' ';
    this->current_player = 'X';
    this->game_over = false;

    for (int i = 0; i < this->board.size(); ++i) {
      for (int j = 0; j < this->board[0].size(); ++j) {
        board[i][j] = ' ';
      }
    }
  }

  void display_board() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    system("clear");

    for (int i = 0; i < this->board.size(); ++i) {
      for (int j = 0; j < this->board[0].size(); ++j) {
        std::cout << ' ' << board[i][j] << ' ';
        if (j != this->board[0].size() - 1) {
          std::cout << '|';
        }
      }

      if (i != this->board.size() - 1) {
        std::cout << std::endl << "===========" << std::endl;
      }
    }

    std::cout << std::endl << std::endl;
    std::cout << "Current player: " << this->current_player << std::endl << std::endl;
  }

  bool make_move(char player, int row, int col) {
    std::unique_lock<std::mutex> lock(this->board_mutex);
    while (this->current_player != player) {
      this->turn_cv.wait(lock);
    }
    // Verificar com o professor se faz sentido essa linha ou se uma maneira melhor
    if (this->game_over) return true;
    if (this->board[row][col] != ' ') return false;

    this->display_board();
    this->board[row][col] = player;

    if (this->check_win(player)) {
      this->winner = player;
      this->game_over = true;
      this->display_board();
    } else if (this->check_draw()) {
      this->winner = 'D';
      this->game_over = true;
      this->display_board();
    }

    this->current_player =  this->current_player == 'X' ? 'O' : 'X';
    this->turn_cv.notify_one();
    return true;
  }

  bool check_win(char player) {
    for (int i = 0; i < this->board.size(); ++i) {
      bool rowWins = true;
      bool colWins = true;
      for (int j = 0; j < this->board[0].size(); ++j) {
        if (this->board[i][j] != player) {
          rowWins = false;
        }

        if (this->board[j][i] != player) {
          colWins = false;
        }
      }

      if (rowWins || colWins) return true;
    }

    bool diagonal1 = true;
    bool diagonal2 = true;
    for (int i = 0, j = this->board.size() - 1; i < this->board.size(); ++i, --j) {
      if (this->board[i][i] != player) {
        diagonal1 = false;
      }

      if (this->board[i][j] != player) {
        diagonal2 = false;
      }
    }

    return diagonal1 || diagonal2;
  }

  bool check_draw() {
    for (std::array<char, 3> row : this->board) {
      for (char value : row) {
        if (value == ' ') return false;
      }
    }

    return true;
  }

  bool is_game_over() {
    return this->game_over;
  }

  char get_winner() {
    return this->winner;
  }
};

// Classe Player
class Player {
private:
  TicTacToe& game; // Referência para o jogo
  char symbol; // Símbolo do jogador ('X' ou 'O')
  Strategy strategy; // Estratégia do jogador

  int row;
  int col;

public:
  Player(TicTacToe& g, char s, Strategy strat) 
    : game(g), symbol(s), strategy(strat), row(-1), col(-1) {}

  void play() {
    while(!this->game.is_game_over()) {
      if (this->strategy == Strategy::SEQUENTIAL) this->play_sequential();
      else this->play_random();
    }
  }

private:
  void play_sequential() {
    do {
      this->col = (this->col + 1) % 3;
      if (this->col == 0) this->row = (this->row + 1) % 3;
    } while(!this->game.make_move(this->symbol, this->row, this->col));
  }

  void play_random() {
    do {
      this->row = getRandomInt(0, 3);
      this->col = getRandomInt(0, 3);
    } while(!this->game.make_move(this->symbol, this->row, this->col));
  }
};

// Função principal
int main() {
  // Inicializar o jogo e os jogadores
  TicTacToe game;
  Player player1(game, 'X', Strategy::SEQUENTIAL);
  Player player2(game, 'O', Strategy::SEQUENTIAL);
  
  // Criar as threads para os jogadores
  std::thread t1([&]() {
    player1.play();
  });
  std::thread t2([&]() {
    player2.play();
  });

  // Aguardar o término das threads
  t1.join();
  t2.join();

  // Exibir o resultado final do jogo
  if (game.get_winner() == 'D') {
    std::cout << "Draw!" << std::endl;
  } else {
    std::cout << "Player " << game.get_winner() << " wins!" << std::endl;
  }

  return 0;
}
