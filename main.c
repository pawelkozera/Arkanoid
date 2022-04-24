#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define SZEROKOSC_CEGIELKI 80
#define WYSOKOSC_CEGIELKI 50
#define KOLUMNA_CEGIELEK 4
#define WIERSZ_CEGIELEK 10
#define KEY_SEEN     1
#define KEY_RELEASED 2

/*
    Dokumentacja Allegro5
    https://github.com/liballeg/allegro_wiki/wiki/Allegro-Vivace%3A-Basic-game-structure
*/

struct Ustawienia_gry {
    int poziom_gry;
    bool zakoncz_program;
};

struct Gracz {
    int zycie;
    int szybkosc_gracza;
    int x_pozycja;
};

struct Cegielki {
    int wytrzymalosc;
    int x_pozycja;
    int y_pozycja;
};

struct Pilka {
    int x, y, szybkosc;
    bool ruch_lewo, ruch_dol;
};

struct Grafiki {
    ALLEGRO_BITMAP* tlo;
    ALLEGRO_BITMAP* platforma;
    ALLEGRO_BITMAP* cegla;
    ALLEGRO_BITMAP* pilka;
};

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

void inicjalizacja_cegielek(struct Cegielki *cegielki, struct Ustawienia_gry ustawienia_gry) {
    srand(NULL);
    for(int i = 0; i < KOLUMNA_CEGIELEK; i++) {
        for(int j = 0; j < WIERSZ_CEGIELEK; j++) {
            cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc = 1 + rand()%ustawienia_gry.poziom_gry;
            cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI;
            cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja = i * WYSOKOSC_CEGIELKI;
        }
    }
}

void rysowanie_cegielek(struct Cegielki *cegielki, struct Grafiki grafiki) {
    for(int i = 0; i < KOLUMNA_CEGIELEK; i++) {
        for(int j = 0; j < WIERSZ_CEGIELEK; j++) {
            al_draw_bitmap(grafiki.cegla, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
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

void pilka_kolizja_z_ramka(struct Pilka *pilka) {
    if (pilka->y >= WYSOKOSC_EKRANU) { // to bedzie warunek przegranej na pozniej !!!!!!!!!!
        pilka->ruch_dol = false;
    }
    if (pilka->y <= 0) {
        pilka->ruch_dol = true;
    }
    if (pilka->x >= SZEROKOSC_EKRANU) {
        pilka->ruch_lewo = true;
    }
    if (pilka->x <= 0) {
        pilka->ruch_lewo = false;
    }
}

int main()
{
    al_init();
    al_install_keyboard();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(SZEROKOSC_EKRANU, WYSOKOSC_EKRANU);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    ALLEGRO_EVENT event;

    if(!al_init_image_addon()) {
        printf("Inicjalizaja obrazow nie powiodla sie.\n");
        return 1;
    }

    // Ustawienia grafik gry
    struct Grafiki grafiki = {al_load_bitmap("obrazki/tlo.png"), al_load_bitmap("obrazki/platforma_gracza.png"), al_load_bitmap("obrazki/cegla.png"), al_load_bitmap("obrazki/pilka.png")};

    // Ustawienia gry
    struct Ustawienia_gry ustawienia_gry = {1, false};

    // Ustawienia gracza
    struct Gracz gracz = {3, 5, (int) SZEROKOSC_EKRANU/2};

    // Ustawienia pilki
    struct Pilka pilka = {(int) SZEROKOSC_EKRANU/2, (int) WYSOKOSC_EKRANU/2, 3, false, true};

    // Ustawienia cegielek
    struct Cegielki *cegielki = (struct Cegielki *)malloc(WIERSZ_CEGIELEK * KOLUMNA_CEGIELEK * sizeof(struct Cegielki));
    inicjalizacja_cegielek(cegielki, ustawienia_gry);

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    bool redraw = true;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                // pilka sterowanie
                pilka_kolizja_z_ramka(&pilka);
                pilka.x = ruch_pilki_x(pilka);
                pilka.y = ruch_pilki_y(pilka);

                if(key[ALLEGRO_KEY_LEFT])
                    gracz.x_pozycja = ruch_w_lewo(gracz.x_pozycja, gracz.szybkosc_gracza);
                if(key[ALLEGRO_KEY_RIGHT])
                    gracz.x_pozycja = ruch_w_prawo(gracz.x_pozycja, gracz.szybkosc_gracza, al_get_bitmap_width(grafiki.platforma));

                if(key[ALLEGRO_KEY_ESCAPE])
                    ustawienia_gry.zakoncz_program = true;

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
        }

        if (ustawienia_gry.zakoncz_program) {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue))
        {
            // rysowanie tla
            al_draw_bitmap(grafiki.tlo, 0, 0, 0);

            // rysowanie platformy gracza
            al_draw_bitmap(grafiki.platforma, gracz.x_pozycja, WYSOKOSC_EKRANU - 20, 0);

            // rysowanie cegielek
            rysowanie_cegielek(cegielki, grafiki);

            // rysowanie pilki
            al_draw_bitmap(grafiki.pilka, pilka.x, pilka.y, 0);


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
    al_destroy_bitmap(grafiki.cegla);

    return 0;
}
