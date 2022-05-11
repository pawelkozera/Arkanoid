#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define SZEROKOSC_CEGIELKI 101
#define WYSOKOSC_CEGIELKI 76
#define KOLUMNA_CEGIELEK 3
#define WIERSZ_CEGIELEK 8
#define KEY_SEEN     1
#define KEY_RELEASED 2
#define ILOSC_PRZYCISKOW 2

/*
    Dokumentacja Allegro5
    https://github.com/liballeg/allegro_wiki/wiki/Allegro-Vivace%3A-Basic-game-structure
*/



struct Ustawienia_gry {
    int poziom_gry;
    int ilosc_cegiel_do_zbicia;
    bool zakoncz_program;
    bool wyswietl_menu;
    bool wyswietl_ekran_przegranej;
};

struct Gracz {
    int zycie;
    int szybkosc_gracza;
    int x_pozycja;
    int y_pozycja;
    int szerokosc_platformy;
    int ilosc_punktow;
};

struct Cegielki {
    int wytrzymalosc;
    int x_pozycja;
    int y_pozycja;
    int typ_cegly;
};

struct Pilka {
    int x, y, szybkosc;
    bool ruch_lewo, ruch_dol;
};

struct Grafiki {
    ALLEGRO_BITMAP* tlo;
    ALLEGRO_BITMAP* platforma;
    ALLEGRO_BITMAP* cegla_zi1;
    ALLEGRO_BITMAP* cegla_zo1;
    ALLEGRO_BITMAP* cegla_zo2;
    ALLEGRO_BITMAP* cegla_cz1;
    ALLEGRO_BITMAP* cegla_cz2;
    ALLEGRO_BITMAP* cegla_cz3;
    ALLEGRO_BITMAP* pilka;
};

// implementacja quadtree
struct QuadTree {
    int lewy_x, lewy_y;
    int prawy_x, prawy_y;

    struct Cegielki *cegielki;

    struct QuadTree *pg; // prawy gorny itd
    struct QuadTree *lg;
    struct QuadTree *pd;
    struct QuadTree *ld;
};

struct QuadTree* stworz_drzewo(int lewy_x, int lewy_y, int prawy_x, int prawy_y) {
    struct QuadTree* drzewo = (struct QuadTree*)malloc(sizeof(struct QuadTree));

    drzewo->lewy_x = lewy_x;
    drzewo->lewy_y = lewy_y;
    drzewo->prawy_x = prawy_x;
    drzewo->prawy_y = prawy_y;

    drzewo->cegielki = NULL;
    drzewo->pg = NULL;
    drzewo->lg = NULL;
    drzewo->pd = NULL;
    drzewo->ld = NULL;

    return drzewo;
}

struct Przycisk {
    int x, y, szerokosc, wysokosc;
    char napis[20];
};

//Prototypy funkcji
int ruch_w_prawo(int x_gracz, int szybkosc_gracza, int szerokosc_gracza);
int ruch_w_lewo(int x_gracz, int szybkosc_gracza);
void inicjalizacja_cegielek(struct Cegielki *cegielki, struct Ustawienia_gry *ustawienia_gry);
void rysowanie_cegielek(struct Cegielki *cegielki, struct Grafiki grafiki);
int ruch_pilki_x(struct Pilka pilka);
int ruch_pilki_y(struct Pilka pilka);
void pilka_kolizja_z_ramka(struct Pilka *pilka, struct Gracz *gracz, ALLEGRO_SAMPLE* hit_sound1);
void pilka_kolizja_z_graczem(struct Pilka *pilka, struct Gracz gracz, ALLEGRO_SAMPLE* hit_sound1);
bool czy_jest_w_granicy_cegly(struct QuadTree *quadTree, struct Cegielki *cegielka);
bool czy_jest_w_granicy_pilka(struct QuadTree *quadTree, struct Pilka *pilka);
void dodaj_do_drzewa(struct QuadTree *quadTree, struct Cegielki *cegielka);
void kolizja_cegla(struct Cegielki *trafiona_cegielka, struct Pilka *pilka, ALLEGRO_SAMPLE* hit_sound2);
void dodaj_punkty(struct Gracz *gracz, struct Cegielki *trafiona_cegla);
void narysuj_interfejs(ALLEGRO_FONT* font, struct Gracz gracz, struct Ustawienia_gry ustawienia_gry);
void przejscie_do_kolejnego_poziomu(struct Ustawienia_gry *ustawienia_gry, struct Pilka *pilka, struct Gracz *gracz);
void narysuj_ekran_przegranej(ALLEGRO_FONT* font, struct Ustawienia_gry ustawienia_gry, struct Gracz gracz);
void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk);
int sprawdz_nacisniecie_przycisku(int mysz_x, int mysz_y, struct Przycisk przyciski[]);
void reset_gry(struct Gracz *gracz, struct Ustawienia_gry *ustawienia_gry, struct Pilka *pilka);

struct Cegielki* szukaj_w_drzewie(struct QuadTree *quadTree, struct Pilka *pilka) {
    if (!czy_jest_w_granicy_pilka(quadTree, pilka)) {
        return NULL;
    }

    if (quadTree->cegielki != NULL) {
        return quadTree->cegielki;
    }

    if ((quadTree->lewy_x + quadTree->prawy_x)/2 >= pilka->x) {
        if ((quadTree->lewy_y + quadTree->prawy_y)/2 >= pilka->y) {
            if (quadTree->lg == NULL) {
                return NULL;
            }
            return szukaj_w_drzewie(quadTree->lg, pilka);
        }
        else {
            if (quadTree->ld == NULL) {
                return NULL;
            }
            return szukaj_w_drzewie(quadTree->ld, pilka);
        }
    }
    else {
        if ((quadTree->lewy_y + quadTree->prawy_y)/2 >= pilka->y) {
            if (quadTree->pg == NULL) {
                return NULL;
            }
            return szukaj_w_drzewie(quadTree->pg, pilka);
        }
        else {
            if (quadTree->pd == NULL) {
                return NULL;
            }
            return szukaj_w_drzewie(quadTree->pd, pilka);
        }
    }
}

int main()
{
    struct Przycisk przycisk_graj = {300, 300 - 60, 80, 40, "GRAJ"};
    struct Przycisk przycisk_wyjdz = {300, 400 - 60, 80, 40, "WYJDZ"};

    struct Przycisk przyciski[] = {przycisk_graj, przycisk_wyjdz};

    al_init();
    al_install_keyboard();
    al_install_mouse();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(SZEROKOSC_EKRANU, WYSOKOSC_EKRANU);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

    al_install_audio(), "audio";
    al_init_acodec_addon(), "audio codecs";
    al_reserve_samples(16), "reserve samples";

    ALLEGRO_SAMPLE* music = al_load_sample("music.wav");
    ALLEGRO_SAMPLE* hit_sound1 = al_load_sample("hit_sound1.wav");
    ALLEGRO_SAMPLE* hit_sound2 = al_load_sample("hit_sound2.wav");
    //al_play_sample(music, 0.02, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

    al_init_primitives_addon();

    ALLEGRO_EVENT event;

    if(!al_init_image_addon()) {
        printf("Inicjalizaja obrazow nie powiodla sie.\n");
        return 1;
    }

    // Ustawienia grafik gry
    struct Grafiki grafiki = {al_load_bitmap("obrazki/tlo.png"), al_load_bitmap("obrazki/platforma_gracza.png"), al_load_bitmap("obrazki/cegla_zi1.png"), al_load_bitmap("obrazki/cegla_zo1.png"),
                                al_load_bitmap("obrazki/cegla_zo2.png"), al_load_bitmap("obrazki/cegla_cz1.png"), al_load_bitmap("obrazki/cegla_cz2.png"),
                                al_load_bitmap("obrazki/cegla_cz3.png"), al_load_bitmap("obrazki/pilka.png")};

    // Ustawienia gry
    struct Ustawienia_gry ustawienia_gry = {1, WIERSZ_CEGIELEK*KOLUMNA_CEGIELEK, false, true, false};

    // Ustawienia gracza
    struct Gracz gracz = {3, 5, (int) SZEROKOSC_EKRANU/2, WYSOKOSC_EKRANU - 20, 50, 0};

    // Ustawienia pilki
    struct Pilka pilka = {180, 300, 4, false, true};

    // Ustawienia cegielek
    struct Cegielki *cegielki = (struct Cegielki *)malloc(WIERSZ_CEGIELEK * KOLUMNA_CEGIELEK * sizeof(struct Cegielki));
    inicjalizacja_cegielek(cegielki, &ustawienia_gry);

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    int nacisniety_przycisk = -1;
    bool wyswietlanie_gry = true;
    bool redraw = true;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        // drzewo quadtree
        struct QuadTree* quadTree = stworz_drzewo(0, 0, SZEROKOSC_EKRANU, WYSOKOSC_EKRANU);

        for(int i = 0; i < KOLUMNA_CEGIELEK; i++) {
            for(int j = 0; j < WIERSZ_CEGIELEK; j++) {
                dodaj_do_drzewa(quadTree, &cegielki[i*WIERSZ_CEGIELEK + j]);
            }
        }

        struct Cegielki *trafiona_cegielka = szukaj_w_drzewie(quadTree, &pilka);
        if (trafiona_cegielka != NULL && trafiona_cegielka->wytrzymalosc > 0) {
            trafiona_cegielka->wytrzymalosc --;
            if (trafiona_cegielka->wytrzymalosc <= 0) {
                ustawienia_gry.ilosc_cegiel_do_zbicia--;
            }
            kolizja_cegla(trafiona_cegielka, &pilka, hit_sound2);
            dodaj_punkty(&gracz, trafiona_cegielka);

            if (ustawienia_gry.ilosc_cegiel_do_zbicia <= 0) {
                przejscie_do_kolejnego_poziomu(&ustawienia_gry, &pilka, &gracz);
                inicjalizacja_cegielek(cegielki, &ustawienia_gry);
            }
        }

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if (!ustawienia_gry.wyswietl_ekran_przegranej && !ustawienia_gry.wyswietl_menu) {
                    // pilka sterowanie
                    pilka_kolizja_z_ramka(&pilka, &gracz, hit_sound1);
                    if (gracz.zycie <= 0) {
                        ustawienia_gry.wyswietl_ekran_przegranej = true;
                    }
                    pilka_kolizja_z_graczem(&pilka, gracz, hit_sound1);
                    pilka.x = ruch_pilki_x(pilka);
                    pilka.y = ruch_pilki_y(pilka);

                    if(key[ALLEGRO_KEY_LEFT])
                        gracz.x_pozycja = ruch_w_lewo(gracz.x_pozycja, gracz.szybkosc_gracza);
                    if(key[ALLEGRO_KEY_RIGHT])
                        gracz.x_pozycja = ruch_w_prawo(gracz.x_pozycja, gracz.szybkosc_gracza, al_get_bitmap_width(grafiki.platforma));
                }
                else if (ustawienia_gry.wyswietl_ekran_przegranej) {
                    if (key[ALLEGRO_KEY_ESCAPE]) {
                        ustawienia_gry.wyswietl_menu = true;
                        ustawienia_gry.wyswietl_ekran_przegranej = false;
                        reset_gry(&gracz, &ustawienia_gry, &pilka);
                        inicjalizacja_cegielek(cegielki, &ustawienia_gry);
                    }
                    if (key[ALLEGRO_KEY_ENTER]) {
                        reset_gry(&gracz, &ustawienia_gry, &pilka);
                        inicjalizacja_cegielek(cegielki, &ustawienia_gry);
                        ustawienia_gry.wyswietl_ekran_przegranej = false;
                    }
                }
                else {
                    // sterowanie dla menu
                }

                //if(key[ALLEGRO_KEY_ESCAPE])
                    //ustawienia_gry.zakoncz_program = true;

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;

                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                ustawienia_gry.zakoncz_program = true;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (event.mouse.button == 1 && ustawienia_gry.wyswietl_menu) {
                    nacisniety_przycisk = sprawdz_nacisniecie_przycisku(event.mouse.x, event.mouse.y, przyciski);
                    if (nacisniety_przycisk == 0) {
                        ustawienia_gry.wyswietl_menu = false;
                        ustawienia_gry.wyswietl_ekran_przegranej = false;
                    }
                    else if (nacisniety_przycisk == 1) {
                        ustawienia_gry.zakoncz_program = true;
                    }
                }
                break;
        }

        if (ustawienia_gry.zakoncz_program) {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue))
        {
            if (!ustawienia_gry.wyswietl_menu && !ustawienia_gry.wyswietl_ekran_przegranej) {
                // rysowanie tla
                al_draw_bitmap(grafiki.tlo, 0, 0, 0);

                // rysowanie interfejsu
                narysuj_interfejs(font, gracz, ustawienia_gry);

                // rysowanie platformy gracza
                al_draw_bitmap(grafiki.platforma, gracz.x_pozycja, gracz.y_pozycja, 0);

                // rysowanie cegielek
                rysowanie_cegielek(cegielki, grafiki);

                // rysowanie pilki
                al_draw_bitmap(grafiki.pilka, pilka.x, pilka.y, 0);
            }
            else if (ustawienia_gry.wyswietl_ekran_przegranej) {
                narysuj_ekran_przegranej(font, ustawienia_gry, gracz);
            }
            else {
                // rysowanie menu
                al_clear_to_color(al_map_rgb(0, 0, 0));
                narysuj_menu(font, przycisk_graj);
                narysuj_menu(font, przycisk_wyjdz);
            }

            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    al_destroy_bitmap(grafiki.platforma);
    al_destroy_bitmap(grafiki.tlo);
    al_destroy_bitmap(grafiki.cegla_zi1);
    al_destroy_bitmap(grafiki.cegla_zo1);
    al_destroy_bitmap(grafiki.cegla_zo2);
    al_destroy_bitmap(grafiki.cegla_cz1);
    al_destroy_bitmap(grafiki.cegla_cz2);
    al_destroy_bitmap(grafiki.cegla_cz3);
    al_destroy_bitmap(grafiki.pilka);
    al_destroy_sample(music);
    al_destroy_sample(hit_sound1);
    al_destroy_sample(hit_sound2);

    return 0;
}

// ruch_w_prawo(int pozycja_obiektu, int szybkosc_przesuwania, int szerokosc_obiektu)
int ruch_w_prawo(int x_gracz, int szybkosc_gracza, int szerokosc_gracza) {
    x_gracz += szybkosc_gracza;
    if (x_gracz >= SZEROKOSC_EKRANU - szerokosc_gracza) {
        x_gracz = SZEROKOSC_EKRANU - szerokosc_gracza;
    }

    return x_gracz;
}

// ruch_w_lewo(int pozycja_obiektu, int szybkosc_przesuwania)
int ruch_w_lewo(int x_gracz, int szybkosc_gracza) {
    x_gracz -= szybkosc_gracza;
    if (x_gracz <= 0) {
        x_gracz = 0;
    }

    return x_gracz;
}

void inicjalizacja_cegielek(struct Cegielki *cegielki, struct Ustawienia_gry *ustawienia_gry) {
    srand(time(NULL));
    int hp = 1;
    for(int i = 0; i < KOLUMNA_CEGIELEK; i++) {
        for(int j = 0; j < WIERSZ_CEGIELEK; j++) {
            hp = 1 + rand()%ustawienia_gry->poziom_gry;
            if (hp > 3) {
                hp = 3;
            }
            cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc = hp;
            cegielki[i*WIERSZ_CEGIELEK + j].typ_cegly = hp;
            cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI;
            cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja = i * WYSOKOSC_CEGIELKI;
        }
    }
    ustawienia_gry->ilosc_cegiel_do_zbicia = KOLUMNA_CEGIELEK*WIERSZ_CEGIELEK;
}

void rysowanie_cegielek(struct Cegielki *cegielki, struct Grafiki grafiki) {
    for(int i = 0; i < KOLUMNA_CEGIELEK; i++) {
        for(int j = 0; j < WIERSZ_CEGIELEK; j++) {
            if (cegielki[i*WIERSZ_CEGIELEK + j].typ_cegly == 1 && cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 1) {
                al_draw_bitmap(grafiki.cegla_zi1, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
            }
            else if (cegielki[i*WIERSZ_CEGIELEK + j].typ_cegly == 2) {
                if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 2) {
                    al_draw_bitmap(grafiki.cegla_zo2, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
                else if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 1) {
                    al_draw_bitmap(grafiki.cegla_zo1, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
            }
            else if (cegielki[i*WIERSZ_CEGIELEK + j].typ_cegly == 3) {
                if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 3) {
                    al_draw_bitmap(grafiki.cegla_cz3, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
                else if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 2) {
                    al_draw_bitmap(grafiki.cegla_cz2, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
                else if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 1){
                    al_draw_bitmap(grafiki.cegla_cz1, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
            }
        }
    }
}

int ruch_pilki_x(struct Pilka pilka) {
    if (pilka.ruch_lewo) {
        return pilka.x -= pilka.szybkosc;
    }
    else {
        return pilka.x += pilka.szybkosc;
    }
}

int ruch_pilki_y(struct Pilka pilka) {
    if (pilka.ruch_dol) {
        return pilka.y += pilka.szybkosc;
    }
    else {
        return pilka.y -= pilka.szybkosc;
    }
}

void pilka_kolizja_z_ramka(struct Pilka *pilka, struct Gracz *gracz, ALLEGRO_SAMPLE* hit_sound1) {
    if (pilka->y >= WYSOKOSC_EKRANU) {
        gracz->zycie--;
        gracz->x_pozycja = 450;
        pilka->x = 180;
        pilka->y = 300;
        pilka->ruch_lewo = false;
        pilka->ruch_dol = true;
    }
    if (pilka->y <= 0) {
        pilka->ruch_dol = true;
        al_play_sample(hit_sound1, 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    if (pilka->x >= SZEROKOSC_EKRANU) {
        pilka->ruch_lewo = true;
        al_play_sample(hit_sound1, 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    if (pilka->x <= 0) {
        pilka->ruch_lewo = false;
        al_play_sample(hit_sound1, 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

void pilka_kolizja_z_graczem(struct Pilka *pilka, struct Gracz gracz, ALLEGRO_SAMPLE* hit_sound1) {
    bool pilka_w_szerokosci_platformy = pilka->x >= gracz.x_pozycja && pilka->x <= gracz.x_pozycja + gracz.szerokosc_platformy;
    bool pilka_na_wysokosci_platformy = pilka->y + 10 >= WYSOKOSC_EKRANU - 20 && pilka->y + 10 <= WYSOKOSC_EKRANU - 10;

    if (pilka_w_szerokosci_platformy && pilka_na_wysokosci_platformy && pilka->ruch_dol) {
        if (pilka->ruch_dol) {
            pilka->ruch_dol = false;
            al_play_sample(hit_sound1, 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
    }
}

bool czy_jest_w_granicy_cegly(struct QuadTree *quadTree, struct Cegielki *cegielka) {
    bool x_granica = cegielka->x_pozycja >= quadTree->lewy_x && cegielka->x_pozycja <= quadTree->prawy_x;
    bool y_granica = cegielka->y_pozycja >= quadTree->lewy_y && cegielka->y_pozycja <= quadTree->prawy_y;

    return x_granica && y_granica;
}

bool czy_jest_w_granicy_pilka(struct QuadTree *quadTree, struct Pilka *pilka) {
    bool x_granica = pilka->x >= quadTree->lewy_x && pilka->x <= quadTree->prawy_x;
    bool y_granica = pilka->y >= quadTree->lewy_y && pilka->y <= quadTree->prawy_y;

    return x_granica && y_granica;
}

void dodaj_do_drzewa(struct QuadTree *quadTree, struct Cegielki *cegielka) {
    if (cegielka == NULL) {
        return;
    }

    if (!czy_jest_w_granicy_cegly(quadTree, cegielka)) {
        return;
    }

    if (abs(quadTree->lewy_x - quadTree->prawy_x) <= SZEROKOSC_CEGIELKI && abs(quadTree->lewy_y - quadTree->prawy_y) <= WYSOKOSC_CEGIELKI) {
        if (quadTree->cegielki == NULL) {
            quadTree->cegielki = cegielka;
            /*
            printf("dodano cegle na %d\n", quadTree->lewy_x);
            printf("dodano cegle na %d\n", quadTree->lewy_y);
            printf("dodano cegle na %d\n", quadTree->prawy_x);
            printf("dodano cegle na %d\n", quadTree->prawy_y);
            printf("dodano cegle na %d\n", quadTree->cegielki->x_pozycja);
            puts("----");
            */
        }
        return;
    }

    if ((quadTree->lewy_x + quadTree->prawy_x)/2 >= cegielka->x_pozycja) {
        if ((quadTree->lewy_y + quadTree->prawy_y)/2 >= cegielka->y_pozycja) {
            if (quadTree->lg == NULL) {
                quadTree->lg = stworz_drzewo(quadTree->lewy_x, quadTree->lewy_y, (quadTree->lewy_x + quadTree->prawy_x)/2, (quadTree->lewy_y + quadTree->prawy_y)/2);
            }
            dodaj_do_drzewa(quadTree->lg, cegielka);
        }
        else {
            if (quadTree->ld == NULL) {
                quadTree->ld = stworz_drzewo(quadTree->lewy_x, (quadTree->lewy_y + quadTree->prawy_y)/2, (quadTree->lewy_x + quadTree->prawy_x)/2, quadTree->prawy_y);
            }
            dodaj_do_drzewa(quadTree->ld, cegielka);
        }
    }
    else {
        if ((quadTree->lewy_y + quadTree->prawy_y)/2 >= cegielka->y_pozycja) {
            if (quadTree->pg == NULL) {
                quadTree->pg = stworz_drzewo((quadTree->lewy_x + quadTree->prawy_x)/2, quadTree->lewy_y, quadTree->prawy_x, (quadTree->lewy_y + quadTree->prawy_y)/2);
            }
            dodaj_do_drzewa(quadTree->pg, cegielka);
        }
        else {
            if (quadTree->pd == NULL) {
                quadTree->pd = stworz_drzewo((quadTree->lewy_x + quadTree->prawy_x)/2, (quadTree->lewy_y + quadTree->prawy_y)/2, quadTree->prawy_x, quadTree->prawy_y);
            }
            dodaj_do_drzewa(quadTree->pd, cegielka);
        }
    }
}

void kolizja_cegla(struct Cegielki *trafiona_cegielka, struct Pilka *pilka, ALLEGRO_SAMPLE* hit_sound2) {
    printf("c: %d p: %d\n", trafiona_cegielka->y_pozycja + WYSOKOSC_CEGIELKI, pilka->y);

    // od dolu
    if (pilka->y >= trafiona_cegielka->y_pozycja + WYSOKOSC_CEGIELKI - pilka->szybkosc && !pilka->ruch_dol) {
        puts("dol");
        pilka->ruch_dol = true;
        al_play_sample(hit_sound2, 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    // od prawej
    else if (pilka->y > trafiona_cegielka->y_pozycja && pilka->y < trafiona_cegielka->y_pozycja + WYSOKOSC_CEGIELKI && pilka->x > trafiona_cegielka->x_pozycja + SZEROKOSC_CEGIELKI/2 && pilka->ruch_lewo) {
        puts("prawo");
        pilka->ruch_lewo = false;
        al_play_sample(hit_sound2, 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    // od lewej
    else if (pilka->y > trafiona_cegielka->y_pozycja && pilka->y < trafiona_cegielka->y_pozycja + WYSOKOSC_CEGIELKI && pilka->x < trafiona_cegielka->x_pozycja + SZEROKOSC_CEGIELKI/2 && !pilka->ruch_lewo) {
        puts("lewo");
        pilka->ruch_lewo = true;
        al_play_sample(hit_sound2, 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
    // od gory
    else if (pilka->y <= trafiona_cegielka->y_pozycja + pilka->szybkosc && pilka->ruch_dol) {
        puts("gora");
        pilka->ruch_dol = false;
        al_play_sample(hit_sound2, 0.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

void dodaj_punkty(struct Gracz *gracz, struct Cegielki *trafiona_cegla) {
    if (trafiona_cegla->typ_cegly == 3 && trafiona_cegla->wytrzymalosc == 0) {
        gracz->ilosc_punktow += 150;
    }
    else if (trafiona_cegla->typ_cegly == 2 && trafiona_cegla->wytrzymalosc == 0) {
        gracz->ilosc_punktow += 100;
    }
    else {
        gracz->ilosc_punktow += 50;
    }
}

void narysuj_interfejs(ALLEGRO_FONT* font, struct Gracz gracz, struct Ustawienia_gry ustawienia_gry) {
    char komunikat_zycie[20] = "Ilosc zyc: ";
    char ilosc_zyc[2];
    sprintf(ilosc_zyc, "%d", gracz.zycie);
    strncat(komunikat_zycie, ilosc_zyc, 2);

    char komunikat_poziom[20] = "Poziom: ";
    char poziom[2];
    sprintf(poziom, "%d", ustawienia_gry.poziom_gry);
    strncat(komunikat_poziom, poziom, 2);

    char komunikat_punkty[20] = "Punkty: ";
    char punkty[10];
    sprintf(punkty, "%d", gracz.ilosc_punktow);
    strncat(komunikat_punkty, punkty, 10);

    al_draw_text(font, al_map_rgb(255, 255, 255), 50, WYSOKOSC_EKRANU - 100, 0, komunikat_zycie);
    al_draw_text(font, al_map_rgb(255, 255, 255), 200, WYSOKOSC_EKRANU - 100, 0, komunikat_poziom);
    al_draw_text(font, al_map_rgb(255, 255, 255), 350, WYSOKOSC_EKRANU - 100, 0, komunikat_punkty);
}

void przejscie_do_kolejnego_poziomu(struct Ustawienia_gry *ustawienia_gry, struct Pilka *pilka, struct Gracz *gracz) {
    ustawienia_gry->poziom_gry++;
    pilka->x = 180;
    pilka->y = 300;
    pilka->ruch_lewo = false;
    pilka->ruch_dol = true;
    gracz->x_pozycja = SZEROKOSC_EKRANU/2;
}

void narysuj_ekran_przegranej(ALLEGRO_FONT* font, struct Ustawienia_gry ustawienia_gry, struct Gracz gracz) {
    char komunikat_poziom[20] = "Poziom: ";
    char poziom[2];
    sprintf(poziom, "%d", ustawienia_gry.poziom_gry);
    strncat(komunikat_poziom, poziom, 2);

    char komunikat_punkty[20] = "Punkty: ";
    char punkty[10];
    sprintf(punkty, "%d", gracz.ilosc_punktow);
    strncat(komunikat_punkty, punkty, 10);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/3, 0, komunikat_poziom);
    al_draw_text(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/2, WYSOKOSC_EKRANU/3, 0, komunikat_punkty);

    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/2, 0, "Nacisnij [Enter]  rozpoczac nowa gra");
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/1.8, 0, "Nacisnij [Escape] aby wyjsc do menu");
}

void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk)
{
    al_draw_filled_rectangle(przycisk.x, przycisk.y, przycisk.x + przycisk.szerokosc, przycisk.y + przycisk.wysokosc, al_map_rgb(112, 110, 104));
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk.x + 10, przycisk.y + (przycisk.wysokosc/2), 0, przycisk.napis);
}

int sprawdz_nacisniecie_przycisku(int mysz_x, int mysz_y, struct Przycisk przyciski[])
{
    bool myszka_w_zakresie_x = false;
    bool myszka_w_zakresie_y = false;

    for (int i = 0; i < ILOSC_PRZYCISKOW; i++) {
        myszka_w_zakresie_x = (mysz_x >= przyciski[i].x && mysz_x <= przyciski[i].x + przyciski[i].szerokosc);
        myszka_w_zakresie_y = (mysz_y >= przyciski[i].y && mysz_y <= przyciski[i].y + przyciski[i].wysokosc);
        if (myszka_w_zakresie_x && myszka_w_zakresie_y) {
            return i;
        }
    }

    return -1;
}

void reset_gry(struct Gracz *gracz, struct Ustawienia_gry *ustawienia_gry, struct Pilka *pilka) {
    gracz->ilosc_punktow = 0;
    gracz->zycie = 3;
    gracz->x_pozycja = SZEROKOSC_EKRANU/2;

    ustawienia_gry->poziom_gry = 1;

    pilka->x = 180;
    pilka->y = 300;
}
