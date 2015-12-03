#include "aux.h"

int main(int agrc, char *argv[]) {
  // Não será usada a janela principal para manter o controle dos tamanhos
  int finaliza = 0;
  int tecla;
  WINDOW *w_splash;
  WINDOW *w_jogo;

  WRAQUETE wraq1;
  WRAQUETE wraq2;

  WBOLA wbola;

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
  iniciar_jogo(&wraq1, &wraq2, &wbola);
  
  mvwprintw(w_jogo, 1, 2, "F1: %d", KEY_F(1));
  mvwprintw(w_jogo, 2, 2, "KEY_LEFT: %d", KEY_LEFT);
  mvwprintw(w_jogo, 3, 2, "KEY_RIGHT: %d", KEY_RIGHT);
  mvwprintw(w_jogo, 4, 2, "Tecla: ");
  mvwprintw(w_jogo, 5, 2, "Lim: ");
  mvwprintw(w_jogo, 6, 2, "(  ,  )");
  
  while (finaliza == 0) {
    tecla = wgetch(w_jogo);

    // DEBUG
    mvwprintw(w_jogo, 4, 9, "%d", tecla);
    mvwprintw(w_jogo, 5, 7, "%d", no_limite(wraq1.raquete));
    mvwprintw(w_jogo, 6, 3, "%2d,%2d", wraq1.raquete.pos.x, wraq1.raquete.pos.x + wraq1.raquete.tamanho);
    switch(tecla) {
    case KEY_F(1):
      finaliza = 1;
      break;
    case KEY_LEFT:
      mover_raquete(&wraq1, -1);
      mover_raquete(&wraq2, -1);
      break;
    case KEY_RIGHT:
      mover_raquete(&wraq1, 1);
      mover_raquete(&wraq2, 1);
      break;
    default:
      break;
    }
    mover_bola(&wbola);
  }

  endwin();
  
  return 0;
}
