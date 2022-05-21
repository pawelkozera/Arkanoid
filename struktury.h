#ifndef STRUKTURY_H_INCLUDED
#define STRUKTURY_H_INCLUDED

#include <allegro5/allegro5.h>

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

struct QuadTree {
    int lewy_x, lewy_y;
    int prawy_x, prawy_y;

    struct Cegielki *cegielki;

    struct QuadTree *pg; // prawy gorny itd
    struct QuadTree *lg;
    struct QuadTree *pd;
    struct QuadTree *ld;
};

struct Przycisk {
    int x, y, szerokosc, wysokosc;
    char napis[20];
};

#endif // STRUKTURY_H_INCLUDED
