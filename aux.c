#include "aux.h"

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

WINDOW *mover_raquete(WRAQUETE *wr, int direcao) {
  int i;
  int limites;
  WINDOW *w = wr->janela;

  /* Move cursor para início da raquete, altera cores para padrão e preenche raquete com ' ' */
  wmove(w, 0, 0);
  wattron(w, COLOR_PAIR(COR_BOLA));
  for (i = 0; i < wr->raquete.tamanho; i++) {
    wprintw(w, "%c", ' ');
  }
  wrefresh(w);

  /* Libera memoria da janela da raquete antiga, altera a posicao da raquete e retorna uma nova raquete */
  delwin(w);

  limites = no_limite(wr->raquete);
  /* limite esquerdo: 01
   * limite direito: 10 */
  if (((direcao == -1) && !((limites & 1) > 0)) ||
      ((direcao == 1) && !((limites & 2) > 0))) {
    wr->raquete.pos.x += direcao;
  }

  return criar_raquete(wr->raquete);
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

void iniciar_jogo(WRAQUETE *wr1, WRAQUETE *wr2, WBOLA *wb) {
  POSICAO pos;
  POSICAO dir;

  // Cria raquetes
  inicia_raquete(wr1, JOGO_ALT - 2);
  inicia_raquete(wr2, 1);

  // Cria bola
  pos.x = (wr1->raquete.pos.x + wr1->raquete.tamanho) / 2;
  pos.y = (wr1->raquete.pos.y - 1);

  dir.x = 1;
  dir.y = -1;

  inicia_bola(wb, pos, dir);
}

void inicia_raquete (WRAQUETE *wr, int altura) {
  wr->raquete.tamanho = DEFAULT_LARG_RAQUETE;
  wr->raquete.pos.x = (JOGO_LARG - wr->raquete.tamanho) / 2;
  wr->raquete.pos.y = altura;
  wr->janela = criar_raquete(wr->raquete);
}

void inicia_bola (WBOLA *wb, POSICAO posicao, POSICAO direcao) {
  BOLA *b = &(wb->bola);

  b->pos = posicao;
  b->direcao = direcao;
  b->periodo = TIMEOUT_JOGO;
  wb->janela = criar_bola(wb->bola);
}

void mover_bola(WBOLA *wb) {
  /* Move cursor para posicao da bola, altera cores para padrão e apaga bola */
  WINDOW *w = wb->janela;
  BOLA *b = &(wb->bola);

  wmove(w, 0, 0);
  wattron(w, COLOR_PAIR(COR_BOLA));
  waddch(w, ' ');

  wrefresh(w);

  /* Libera memoria da janela da bola antiga e altera a posicao da bola */
  delwin(w);

  /* Colisao da bola deve ser verificada antes desse metodo */
  verificar_colisao(wb);

  b->pos.x += b->direcao.x;
  b->pos.y += b->direcao.y;
  w = criar_bola(*b);
}

void verificar_colisao(WBOLA *wb) {
  BOLA *b = &(wb->bola);

  if (b->pos.x >= JOGO_LARG ||
      b->pos.x <= 0) {
    b->direcao.x = -(b->direcao.x);
  }
}

/*
 * no_limite retorna:
 *  00: nao esta no limite.
 *  01: limite direito.
 *  10: limite esquerdo.
 */
int no_limite(RAQUETE r) {
  int pos_dir = r.pos.x + r.tamanho;
  int lim_esq = r.pos.x > 1 ? 0 : 1;
  int lim_dir = pos_dir < JOGO_LARG - 1 ? 0 : 1;

  return lim_esq | (lim_dir << 1);
}
