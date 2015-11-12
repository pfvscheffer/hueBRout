#include <ncurses.h>
#include <string.h>

/* OBS: as unidades do jogo são consideradas em distância Chebyshev */

/* Representa as raquetes do jogo */

#define TIMEOUT_JOGO 500 // Milissegundos para leitura de caractere
#define JOGO_LARG 60
#define JOGO_ALT 40
#define DEFAULT_LARG_RAQUETE 15

typedef struct {
  int x, y;
} POSICAO;

typedef struct {
  POSICAO pos; // A posição do elemento mais à esquerda da raquete
  int tamanho; // O tamanho total da raquete
} RAQUETE;

/* Representa a bola */
typedef struct {
  int x, y;    // A posição horizontal e vertical da bola
  int periodo; // Período que a bola demora para se mexer 1 unidade, em microssegundos
  int direcao; // Direção do movimento
} BOLA;

typedef struct {
  int x, y; // Posição horizontal e vertical do powerup
  int periodo; // Período que o powerup demora pra cair 1 unidade, ou 0 para parado, em microssegundos
  int tipo; // Tipo de powerup
} POWERUP;

void imprime_no_meio(WINDOW *win, char *texto);
WINDOW *criar_raquete(RAQUETE raquete);
WINDOW *mover_raquete(WINDOW *w_raquete, RAQUETE *raquete, int direcao);

int main(int agrc, char *argv[]) {
  // Não será usada a janela principal para manter o controle dos tamanhos
  int finaliza = 0;
  int tecla;
  WINDOW *w_splash;
  WINDOW *w_jogo;
  WINDOW *w_raquete;
  RAQUETE raquete;
  
  //WINDOW *vida;
  //WINDOW *escore;

  initscr();
  keypad(stdscr, TRUE); // Precisaremos do F1
  noecho();
  curs_set(FALSE);

  //newwin(altura, largura, y, x);
  //box(win, v, h);
  w_splash = newwin(40, 80, 0, 0);
  box(w_splash, 0, 0);
  wrefresh(w_splash);
  imprime_no_meio(w_splash, "Pressione qualquer tecla...");

  wgetch(w_splash);

  // Cira janela do jogo
  w_jogo = newwin(JOGO_ALT, JOGO_LARG, 0, 0);
  keypad(w_jogo, TRUE); // Precisaremos do F1

  box(w_jogo, 0, 0);

  imprime_no_meio(w_jogo, "Pressione F1 para sair!");
  
  wrefresh(w_jogo);
  wtimeout(w_jogo, TIMEOUT_JOGO);

  // Cria raquete
  raquete.tamanho = DEFAULT_LARG_RAQUETE;
  raquete.pos.x = (JOGO_LARG - raquete.tamanho) / 2;
  raquete.pos.y = JOGO_ALT - 2;
  w_raquete = criar_raquete(raquete);  

  mvwprintw(w_jogo, 1, 2, "F1: %d", KEY_F(1));
  mvwprintw(w_jogo, 2, 2, "KEY_LEFT: %d", KEY_LEFT);
  mvwprintw(w_jogo, 3, 2, "KEY_RIGHT: %d", KEY_RIGHT);
  mvwprintw(w_jogo, 4, 2, "Tecla: ");
  
  while (finaliza == 0) {
    tecla = wgetch(w_jogo);
    mvwprintw(w_jogo, 4, 7, "%d", tecla);
    switch(tecla) {
    case KEY_F(1):
      finaliza = 1;
      break;
    case KEY_LEFT:
      mover_raquete(w_raquete, &raquete, -1);
      break;
    case KEY_RIGHT:
      mover_raquete(w_raquete, &raquete, 1);
      break;
    default:
      break;
    }
  }

  endwin();
  
  return 0;
}

void imprime_no_meio(WINDOW *win, char *texto) {
  int length, x, y;
  int max_x, max_y;

  float temp;

  // obs: getmaxyx(win,y,x)	(y = getmaxy(win), x = getmaxx(win))
  getyx(win, y, x);
  getmaxyx(win, max_y, max_x);

  length = strlen(texto);
  temp = (max_x - x - length) / 2;
  x += (int)temp;
  mvwprintw(win, (max_y - y)/2, x, "%s", texto);
  refresh();
}

WINDOW *criar_raquete(RAQUETE raquete) {
  WINDOW *nova_raquete;
  int i;
  int altura = 1;
  int largura = raquete.tamanho;
  POSICAO inicio = raquete.pos;

  nova_raquete = newwin(altura, largura, inicio.y, inicio.x);
  wmove(nova_raquete, 0, 0);
  for (i = 0; i < raquete.tamanho; i++) {
    waddch(nova_raquete, ACS_DIAMOND);
  }
  
  wrefresh(nova_raquete);
  
  return nova_raquete;
}

WINDOW *mover_raquete(WINDOW *w_raquete, RAQUETE *raquete, int direcao) {
  int i;

  wmove(w_raquete, 0, 0);  
  for (i = 0; i < raquete->tamanho; i++) {
    wprintw(w_raquete, "%c", ' ');
  }

  wrefresh(w_raquete);
  
  delwin(w_raquete);
  raquete->pos.x += direcao;
  return criar_raquete(*raquete);
}
