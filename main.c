#include <ncurses.h>
#include <string.h>

/* OBS: as unidades do jogo são consideradas em distância Chebyshev */

/* Representa as raquetes do jogo */

#define TIMEOUT_JOGO 500 // Milissegundos para leitura de caractere
#define JOGO_LARG 60
#define JOGO_ALT 40
#define DEFAULT_LARG_RAQUETE 15
#define COR_RAQUETE 1
#define COR_BOLA 2
#define COR_BLOCO_NORMAL 3
#define COR_INQUEBRAVEL 4

typedef struct posicao_str {
  int x, y;
} POSICAO;

typedef struct raquete_str {
  POSICAO pos; // A posição do elemento mais à esquerda da raquete
  int tamanho; // O tamanho da raquete
} RAQUETE;

/* Representa a bola */
typedef struct bola_str {
  POSICAO pos;    // A posição horizontal e vertical da bola
  int periodo; // Período que a bola demora para se mexer 1 unidade, em microssegundos
  POSICAO direcao; // Vetor de direção do movimento
} BOLA;

typedef struct powerup_str {
  int x, y; // Posição horizontal e vertical do powerup
  int periodo; // Período que o powerup demora pra cair 1 unidade, ou 0 para parado, em microssegundos
  int tipo; // Tipo de powerup
} POWERUP;

void iniciar_jogo(RAQUETE *raquete, WINDOW *w_raquete, WINDOW *w_raquete2, BOLA *bola, WINDOW *w_bola);
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
  WINDOW *w_raquete2; // A janela da raquete superior
  WINDOW *w_bola;
  BOLA bola;
  RAQUETE raquete;

  initscr();
  if(has_colors() == FALSE) {
    endwin();
    printf("Terminal nao suporta cores.\n");
    return 1;
  }
  start_color();

  init_pair(COR_RAQUETE, COLOR_WHITE, COLOR_GREEN);
  init_pair(COR_BOLA, COLOR_WHITE, COLOR_BLACK);
  init_pair(COR_BLOCO_NORMAL, COLOR_WHITE, COLOR_BLUE);
  init_pair(COR_INQUEBRAVEL, COLOR_WHITE, COLOR_YELLOW);

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

  // Cria janela do jogo
  w_jogo = newwin(JOGO_ALT, JOGO_LARG, 0, 0);
  keypad(w_jogo, TRUE); // Precisaremos do F1

  box(w_jogo, 0, 0);

  imprime_no_meio(w_jogo, "Pressione F1 para sair!");
  
  wrefresh(w_jogo);
  wtimeout(w_jogo, TIMEOUT_JOGO);

  /* Warning: as variaveis de janela serao inicializadas */
  iniciar_jogo(&raquete, w_raquete, w_raquete2, &bola, w_bola);
  
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

  /* Cria uma nova janela pra a raquete, move o cursor para o inicio e inicializa cores da raquete */
  nova_raquete = newwin(altura, largura, inicio.y, inicio.x);
  wmove(nova_raquete, 0, 0);
  wattron(nova_raquete, COLOR_PAIR(COR_RAQUETE));

  /* Preenche a raquete com caracteres de diamante */
  for (i = 0; i < raquete.tamanho; i++) {
    waddch(nova_raquete, ACS_DIAMOND);
  }
  
  wrefresh(nova_raquete);
  
  return nova_raquete;
}

WINDOW *mover_raquete(WINDOW *w_raquete, RAQUETE *raquete, int direcao) {
  int i;

  /* Move cursor para início da raquete, altera cores para padrão e preenche raquete com ' ' */
  wmove(w_raquete, 0, 0);
  wattron(w_raquete, COLOR_PAIR(COR_BOLA));
  for (i = 0; i < raquete->tamanho; i++) {
    wprintw(w_raquete, "%c", ' ');
  }
  wrefresh(w_raquete);

  /* Libera memoria da janela da raquete antiga, altera a posicao da raquete e retorna uma nova raquete */
  delwin(w_raquete);
  raquete->pos.x += direcao;
  return criar_raquete(*raquete);
}

WINDOW *criar_bola(BOLA bola) {
  WINDOW *nova_bola;
  POSICAO inicio = bola.pos;

  /* Cria uma nova janela pra a raquete, move o cursor para o inicio e inicializa cores da raquete */
  nova_bola = newwin(1, 1, inicio.y, inicio.x);
  wmove(nova_bola, 0, 0);
  wattron(nova_bola, COLOR_PAIR(COR_BOLA));
  waddch(nova_bola, 'o');
  wrefresh(nova_bola);
  
  return nova_bola;
}

void iniciar_jogo(RAQUETE *raquete, WINDOW *w_raquete1, WINDOW *w_raquete2, BOLA *bola, WINDOW *w_bola) {
  RAQUETE raquete2;
  
  // Cria raquetes
  raquete->tamanho = DEFAULT_LARG_RAQUETE;
  raquete->pos.x = (JOGO_LARG - raquete->tamanho) / 2;
  raquete->pos.y = JOGO_ALT - 2;

  raquete2 = *raquete;
  raquete2.pos.y = 1;
  
  w_raquete1 = criar_raquete(*raquete);
  w_raquete2 = criar_raquete(raquete2);

  // Cria bola
  bola->pos.x = (raquete->pos.x + raquete->tamanho) / 2;
  bola->pos.y = (raquete->pos.y - 1);
  bola->direcao.x = 1;
  bola->direcao.y = -1;

  w_bola = criar_bola(*bola);
}

WINDOW *mover_bola(WINDOW *w_bola, BOLA *bola) {
  /* Move cursor para posicao da bola, altera cores para padrão e apaga bola */
  wmove(w_bola, 0, 0);
  wattron(w_bola, COLOR_PAIR(COR_BOLA));
  waddch(w_bola, ' ');

  wrefresh(w_bola);

  /* Libera memoria da janela da bola  antiga, altera a posicao da bola e retorna uma nova bola */
  delwin(w_bola);

  /* Colisao da bola deve ser verificada antes desse metodo */
  bola->pos.x += bola->direcao.x;
  bola->pos.y += bola->direcao.y;
  return criar_bola(*bola);
}
/*
void verificar_colisao(BOLA *bola, RAQUETE *raquete, MAPA *mapa) {
  
}*/

