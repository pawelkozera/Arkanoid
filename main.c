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

struct Grafiki {
    ALLEGRO_BITMAP* tlo;
    ALLEGRO_BITMAP* platforma;
    ALLEGRO_BITMAP* cegla;
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

    // Ustawienia gracza
    struct Gracz gracz = {3, 5, (int) SZEROKOSC_EKRANU/2};

    // Ustawienia gry
    struct Ustawienia_gry ustawienia_gry = {0, false};

    // Ustawienia cegielek
    struct Cegielki cegielki[10][4] = {1, 0, 0};

    // Ustawienia grafik gry
    struct Grafiki grafiki = {al_load_bitmap("obrazki/tlo.png"), al_load_bitmap("obrazki/platforma_gracza.png"), al_load_bitmap("obrazki/cegla.png")};

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
            int i, j;
            for(i = 0; i < 4; i++)
            {
                for(j = 0; j < 10; j++)
                {
                    cegielki[j][i].wytrzymalosc = 1;
                    cegielki[j][i].x_pozycja = j * SZEROKOSC_CEGIELKI;
                    cegielki[j][i].y_pozycja = i * WYSOKOSC_CEGIELKI;
                    al_draw_bitmap(grafiki.cegla, cegielki[j][i].x_pozycja, cegielki[j][i].y_pozycja, 0);
                }
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
    al_destroy_bitmap(grafiki.cegla);

    return 0;
}
