#include <ncurses.h>
#include <string.h>

/* OBS: as unidades do jogo são consideradas em distância Chebyshev */

#define TIMEOUT_JOGO 50 // Milissegundos para leitura de caractere
#define PERIODO_PADRAO 2
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
  int direcao; // Direcao em que raquete se move neste ciclo (-1, 0, 1)
} RAQUETE;

/* Representa a bola */
typedef struct bola_str {
  POSICAO pos;    // A posição horizontal e vertical da bola
  int periodo; // Período que a bola demora para se mexer 1 unidade, em microssegundos
  int ct_periodo; // Contador de ciclos para movimento da bola
  POSICAO direcao; // Vetor de direção do movimento
} BOLA;

typedef struct powerup_str {
  int x, y; // Posição horizontal e vertical do powerup
  int periodo; /* Período que o powerup demora pra cair 1 unidade,
                * ou 0 para parado, em microssegundos */
  int ct_periodo;
  int tipo; // Tipo de powerup
} POWERUP;

typedef struct wraquete_str {
  RAQUETE raquete;
  WINDOW *janela;
} WRAQUETE;

typedef struct wbola_str {
  BOLA bola;
  WINDOW *janela;
} WBOLA;

void iniciar_jogo(WRAQUETE *wr1, WRAQUETE *wr2, WBOLA *wb);
void imprime_no_meio(WINDOW *win, char *texto);
void criar_raquete(WRAQUETE *wr);
void mover_raquete(WRAQUETE *wr);
void inicia_raquete (WRAQUETE *wr, int altura);
void inicia_bola(WBOLA *wb, POSICAO posicao, POSICAO direcao);
void mover_bola(WBOLA *wb);
int no_limite(RAQUETE r);
void verificar_colisao (BOLA *b, RAQUETE *r1, RAQUETE *r2);
void alterar_direcao (BOLA *b, RAQUETE *r, int colisao);
int deve_mover_bola(BOLA b);
