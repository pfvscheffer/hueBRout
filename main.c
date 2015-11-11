#include <ncurses.h>

/* OBS: as unidades do jogo são consideradas em distância Chebyshev */

/* Representa as raquetes do jogo */
typedef struct str_raquete {
  int posicao; // A posição do elemento mais à esquerda da raquete
  int tamanho; // O tamanho total da raquete
} RAQUETE;

/* Representa a bola */
typedef struct str_bola {
  int x, y;    // A posição horizontal e vertical da bola
  int periodo; // Período que a bola demora para se mexer 1 unidade, em microssegundos
  int direção; // Direção do movimento
} BOLA;

typedef struct str_powerup {
  int x, y; // Posição horizontal e vertical do powerup
  int periodo; // Período que o powerup demora pra cair 1 unidade, ou 0 para parado, em microssegundos
  int tipo; // Tipo de powerup
} POWERUP;

int main(int agrc, char *argv[]) {
  WINDOW *
  
  return 0;
}
