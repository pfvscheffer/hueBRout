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

void criar_raquete(WRAQUETE *wr) {
  WINDOW *nova_raquete;
  RAQUETE raquete = wr->raquete;
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
  
  wr->janela = nova_raquete;
}

void mover_raquete(WRAQUETE *wr) {
  int i;
  int limites;
  WINDOW *w = wr->janela;
  RAQUETE *r = &(wr->raquete);

  /* Move cursor para início da raquete, altera cores para padrão e preenche raquete com ' ' */
  wmove(w, 0, 0);
  wattron(w, COLOR_PAIR(COR_BOLA));
  for (i = 0; i < wr->raquete.tamanho; i++) {
    wprintw(w, "%c", ' ');
  }
  wrefresh(w);

  /* Libera memoria da janela da raquete antiga, altera a posicao da raquete e retorna uma nova raquete */
  delwin(w);

  limites = no_limite(*r);
  /* limite esquerdo: 01
   * limite direito: 10 */
  if (((r->direcao == -1) && !((limites & 1) > 0)) ||
      ((r->direcao == 1) && !((limites & 2) > 0))) {
    wr->raquete.pos.x += r->direcao;
  }

  criar_raquete(wr);
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
  RAQUETE *r = &(wr->raquete);
  r->tamanho = DEFAULT_LARG_RAQUETE;
  r->pos.x = (JOGO_LARG - wr->raquete.tamanho) / 2;
  r->pos.y = altura;
  r->direcao = 0;
  criar_raquete(wr);
}

void inicia_bola (WBOLA *wb, POSICAO posicao, POSICAO direcao) {
  BOLA *b = &(wb->bola);

  b->pos = posicao;
  b->direcao = direcao;
  b->periodo = PERIODO_PADRAO;
  b->ct_periodo = 0;
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
  b->pos.x += b->direcao.x;
  b->pos.y += b->direcao.y;
  w = criar_bola(*b);
}

/*
 * verificar_colisao retorna:
 * 0: sem colisao
 * 1: colisao com parede
 * 2: colisao com raquete
 * 4: colisao com bloco no lado
 * 8: colisao com bloco em cima ou abaixo
 */
void verificar_colisao(BOLA *b, RAQUETE *r1, RAQUETE *r2) {
  int colisao = 0;
  POSICAO prox_pos;

  prox_pos.x = b->pos.x + b->direcao.x;
  prox_pos.y = b->pos.y + b->direcao.y;

  if (prox_pos.x >= JOGO_LARG - 1 ||
      prox_pos.x <= 1) {
    colisao |= 1;
  }

  if ((prox_pos.y == r1->pos.y ||
       prox_pos.y == r2->pos.y) &&
      prox_pos.x >= r1->pos.x &&
      prox_pos.x <= r1->pos.x + r1->tamanho) {
    colisao |= 2;
  }

  alterar_direcao(b, r1, colisao);
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

void alterar_direcao(BOLA *b, RAQUETE *r, int colisao) {
  int colisao_horizontal = 1 | 4;
  int colisao_vertical = 2 | 8;

  if ((colisao & colisao_horizontal) != 0) {
    b->direcao.x = -(b->direcao.x);
  } else if ((colisao & 2) != 0 &&
	     r->direcao != 0){
    b->direcao.x = r->direcao;
  }

  if ((colisao & colisao_vertical) != 0) {
    b->direcao.y = -(b->direcao.y);
  }
}

int deve_mover_bola(BOLA b) {
  return b.ct_periodo >= b.periodo;
}
