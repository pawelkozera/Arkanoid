#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

#include "struktury.h"
#include "quadTree.h"
#include "rysowanie.h"
#include "kolizje.h"
#include "mechanika.h"
#include "stany_gry.h"

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define KEY_SEEN     1
#define KEY_RELEASED 2

int KOLUMNA_CEGIELEK = 3;
int WIERSZ_CEGIELEK = 8;

/*
    Dokumentacja Allegro5
    https://github.com/liballeg/allegro_wiki/wiki/Allegro-Vivace%3A-Basic-game-structure
*/

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
