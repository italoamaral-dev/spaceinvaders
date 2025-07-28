#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h> 
/* Libraries:
   - Standard I/O, random and time for game logic
   - Allegro for graphics, primitives, and font rendering */

#define NUM_LINHAS 4
#define NUM_COLUNAS 6
#define ESPACO_X 30
/* Constants defining grid layout of aliens */

const int SCREEN_W = 960;
const int SCREEN_H = 540;
const int GRASS_H = 60;
const int NAVE_W = 100;
const int NAVE_H = 50;
const int ALIEN_W = 60;
const int ALIEN_H = 32;
const float FPS = 60.0;
/* Game configuration constants: screen size, object dimensions, and frame rate */

typedef struct Nave {
    float x;
    float vel;
    int dir, esq;
    ALLEGRO_COLOR cor;
} Nave;
/* Player spaceship:
   - x: horizontal position
   - vel: movement speed
   - dir, esq: directional flags (right/left)
   - cor: ship color */

typedef struct Alien {
    float x, y;
    float x_vel, y_vel;
    int vivo;
    ALLEGRO_COLOR cor;
} Alien;
/* Alien entity:
   - x, y: position
   - x_vel, y_vel: movement velocity
   - vivo: alive flag (1=alive, 0=dead)
   - cor: color */

typedef struct Tiro {
    float x, y;
    float vel;
    int ativo;
} Tiro;
/* Bullet (shot):
   - x, y: position
   - vel: speed
   - ativo: active flag (1=on screen, 0=inactive) */

int carregarRecorde() {
    FILE *f = fopen("recorde.txt", "r");
    if (f == NULL) return 0;
    int r = 0;
    fscanf(f, "%d", &r);
    fclose(f);
    return r;
} /* Loads the saved high score from a text file (returns 0 if no file exists) */

void salvarRecorde(int r) {
    FILE *f = fopen("recorde.txt", "w");
    if (f == NULL) return;
    fprintf(f, "%d", r);
    fclose(f);
} /* Saves the new high score to a text file */

void initNave(Nave *n){
    n->x = SCREEN_W / 2;
    n->vel = 3;
    n->dir = 0;
    n->esq = 0;
    n->cor = al_map_rgb(0, 0, 255);
} /* Initializes the spaceship in the center with default speed and blue color */

void initAliens(Alien a[NUM_LINHAS][NUM_COLUNAS]){
    for(int i=0;i<NUM_LINHAS;i++){
        for(int j=0;j<NUM_COLUNAS;j++){
            a[i][j].x = 100 + j*(ALIEN_W + ESPACO_X);
            a[i][j].y = 50 + i*(ALIEN_H + 20);
            a[i][j].x_vel = 2;
            a[i][j].y_vel = ALIEN_H;
            a[i][j].vivo = 1;
            a[i][j].cor = al_map_rgb(rand()%256, rand()%256, rand()%256);
        }
    }
} /* Initializes aliens in a grid with random colors and default velocities */

int colisao_alien_nave(Alien a, Nave *n){
    float topo = SCREEN_H - GRASS_H - NAVE_H;
    float base = SCREEN_H - GRASS_H;
    float esq = n->x - NAVE_W/2;
    float dir = n->x + NAVE_W/2;

    if (!a.vivo) return 0;

    return !(a.x + ALIEN_W < esq || a.x > dir || a.y + ALIEN_H < topo || a.y > base);
} /* Checks collision between an alien and the spaceship (returns 1 if colliding) */

void draw_alien(Alien a){
    if(a.vivo){
        al_draw_filled_rectangle(a.x, a.y, a.x+ALIEN_W, a.y+ALIEN_H, a.cor);
    }
} /* Draws an alien only if it is alive */

void updateAliens(Alien a[NUM_LINHAS][NUM_COLUNAS]){
    int inv = 0;

    for(int i=0;i<NUM_LINHAS;i++){
        for(int j=0;j<NUM_COLUNAS;j++){
            Alien *ali = &a[i][j];
            if(ali->vivo){
                if(ali->x + ALIEN_W + ali->x_vel > SCREEN_W || ali->x + ali->x_vel < 0){
                    inv = 1;
                }
            }
        }
    }

    for(int i=0;i<NUM_LINHAS;i++){
        for(int j=0;j<NUM_COLUNAS;j++){
            Alien *ali = &a[i][j];
            if(ali->vivo){
                if(inv){
                    ali->y += ali->y_vel;
                    ali->x_vel *= -1;
                }
                ali->x += ali->x_vel;
            }
        }
    }
} /* Updates alien positions, reversing direction and moving down when hitting screen edges */

int colisao_alien_solo(Alien a){
    return a.vivo && (a.y + ALIEN_H > SCREEN_H - GRASS_H);
} /* Checks if an alien reached the ground (returns 1 if true) */

void draw_scenario(){
    al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_filled_rectangle(0, SCREEN_H - GRASS_H, SCREEN_W, SCREEN_H, al_map_rgb(0,245,0));
} /* Clears the screen and draws the ground */

void draw_nave(Nave *n){
    float yb = SCREEN_H - GRASS_H/2;
    al_draw_filled_triangle(n->x, yb - NAVE_H,
                            n->x - NAVE_W/2, yb,
                            n->x + NAVE_W/2, yb,
                            n->cor);
} /* Draws the spaceship as a triangle */

void update_nave(Nave *n){
    if(n->dir && n->x + n->vel <= SCREEN_W){
        n->x += n->vel;
    }
    if(n->esq && n->x - n->vel >= 0){
        n->x -= n->vel;
    }
} /* Updates spaceship position based on directional flags */

void init_tiro(Tiro *t){
    t->x = 0;
    t->y = 0;
    t->vel = 5;
    t->ativo = 0;
} /* Initializes the bullet as inactive */

void draw_tiro(Tiro t){
    if(t.ativo){
        al_draw_filled_circle(t.x, t.y, 5, al_map_rgb(255,255,255));
    }
} /* Draws the bullet if active */

void update_tiro(Tiro *t){
    if(t->ativo){
        t->y -= t->vel;
        if(t->y < 0){
            t->ativo = 0;
        }
    }
} /* Updates bullet movement upward, deactivating it when it leaves the screen */

int colidiu(Alien a, Tiro t){
    if(!a.vivo || !t.ativo) return 0;
    if(t.x >= a.x && t.x <= a.x + ALIEN_W && t.y >= a.y && t.y <= a.y + ALIEN_H){
        return 1;
    }
    return 0;
} /* Checks collision between a bullet and an alien (returns 1 if hit) */

int main(){

    srand(time(NULL)); /* Initializes random seed for alien colors */

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;

    if(!al_init()) return -1;

    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display) return -1;

    al_init_primitives_addon();
    al_init_font_addon();

    timer = al_create_timer(1.0 / FPS);
    if(!timer) return -1;

    event_queue = al_create_event_queue();
    if(!event_queue) return -1;

    if(!al_install_keyboard()) return -1;
    if(!al_install_mouse()) return -1;

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    /* Allegro initialization: display, timer, input devices, and event queue */

    Nave nave;
    initNave(&nave);

    Alien aliens[NUM_LINHAS][NUM_COLUNAS];
    initAliens(aliens);

    Tiro tiro;
    init_tiro(&tiro);

    int pontos = 0;
    int recorde = carregarRecorde();
    int playing = 1;
    int venceu = 0;

    al_start_timer(timer); /* Starts the game loop timer */

    while(playing){
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_TIMER){

            draw_scenario();

            al_draw_textf(al_create_builtin_font(), al_map_rgb(255,255,255), 10, 10, 0, "Pontos: %d", pontos);
            al_draw_textf(al_create_builtin_font(), al_map_rgb(255,255,0), 10, 30, 0, "Recorde: %d", recorde);

            update_nave(&nave);
            update_tiro(&tiro);
            updateAliens(aliens);

            for(int i=0;i<NUM_LINHAS;i++){
                for(int j=0;j<NUM_COLUNAS;j++){
                    Alien *a = &aliens[i][j];

                    if(colisao_alien_solo(*a)){
                        playing = 0;
                        printf("\nUm alien chegou ao chão. Fim de jogo!\n");
                    }

                    if(colisao_alien_nave(*a, &nave)){
                        playing = 0;
                        printf("\nAlien colidiu com a nave! Fim de jogo!\n");
                    }

                    if(colidiu(*a, tiro)){
                        a->vivo = 0;
                        tiro.ativo = 0;
                        pontos++;
                        printf("\nPontos: %d", pontos);
                    }

                    draw_alien(*a);
                }
            }

            int todos_mortos = 1;
            for (int i = 0; i < NUM_LINHAS; i++) {
                for (int j = 0; j < NUM_COLUNAS; j++) {
                    if (aliens[i][j].vivo) {
                        todos_mortos = 0;
                        break;
                    }
                }
                if (!todos_mortos) break;
            }

            if (todos_mortos) {
                playing = 0;
                venceu = 1;
                printf("\nTodos os aliens foram destruídos! Você venceu!\n");
            }

            draw_nave(&nave);
            draw_tiro(tiro);
            al_flip_display();

        } else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            playing = 0;
        } else if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
            switch(ev.keyboard.keycode){
                case ALLEGRO_KEY_A:
                    nave.esq = 1;
                    break;
                case ALLEGRO_KEY_D:
                    nave.dir = 1;
                    break;
                case ALLEGRO_KEY_SPACE:
                    if(!tiro.ativo){
                        tiro.x = nave.x;
                        tiro.y = SCREEN_H - GRASS_H - NAVE_H;
                        tiro.ativo = 1;
                    }
                    break;
            }
        } else if(ev.type == ALLEGRO_EVENT_KEY_UP){
            switch(ev.keyboard.keycode){
                case ALLEGRO_KEY_A:
                    nave.esq = 0;
                    break;
                case ALLEGRO_KEY_D:
                    nave.dir = 0;
                    break;
            }
        }
    }

    if(pontos > recorde){
        recorde = pontos;
        salvarRecorde(recorde);
        printf("\nNovo recorde: %d\n", recorde);
    }

    al_clear_to_color(al_map_rgb(0, 0, 0));

    if (venceu) {
        al_draw_textf(al_create_builtin_font(), al_map_rgb(0, 255, 0),
                      SCREEN_W/2, SCREEN_H/2 - 20, ALLEGRO_ALIGN_CENTER,
                      "VOCÊ VENCEU! Pontos: %d", pontos);
    } else {
        al_draw_textf(al_create_builtin_font(), al_map_rgb(255, 255, 255),
                      SCREEN_W/2, SCREEN_H/2 - 20, ALLEGRO_ALIGN_CENTER,
                      "FIM DE JOGO! Pontos: %d", pontos);
    }

    al_draw_textf(al_create_builtin_font(), al_map_rgb(255, 255, 0),
                  SCREEN_W/2, SCREEN_H/2 + 20, ALLEGRO_ALIGN_CENTER,
                  "Recorde: %d", recorde);
    al_flip_display();
    al_rest(3.0);
    /* Final game screen: displays win/lose message and waits 3 seconds */

    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    system("pause");

    return 0;
} /* Main game loop: initializes Allegro, runs the game, handles events, and displays end screen */
