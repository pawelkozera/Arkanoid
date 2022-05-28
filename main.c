#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>

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
    struct Grafiki grafiki = {al_load_bitmap("obrazki/tlo.png"), al_load_bitmap("obrazki/platforma_gracza.png"), al_load_bitmap("obrazki/platforma_gracza_bonus.png"),
                                al_load_bitmap("obrazki/cegla_zi1.png"), al_load_bitmap("obrazki/cegla_zo1.png"),
                                al_load_bitmap("obrazki/cegla_zo2.png"), al_load_bitmap("obrazki/cegla_cz1.png"), al_load_bitmap("obrazki/cegla_cz2.png"),
                                al_load_bitmap("obrazki/cegla_cz3.png"), al_load_bitmap("obrazki/pilka.png"), al_load_bitmap("obrazki/bonus.png")};

    // Ustawienia gry
    struct Ustawienia_gry ustawienia_gry = {1, WIERSZ_CEGIELEK*KOLUMNA_CEGIELEK, false, true, false, false, 1};

    // Ustawienia bonusu
    struct Bonus bonus = {0, 0, 2, 16, 16, 0, false};

    // Ustawienia gracza
    struct Gracz gracz = {3, 5, (int) SZEROKOSC_EKRANU/2, WYSOKOSC_EKRANU - 20, 50, 0, 10, 50};

    // Ustawienia pilki
    struct Pilka pilka[3] = {180, 300, 4, false, true};
    for (int i = 0; i < 3; i++) {
        int z = 0;
        pilka[i].x = 180 + z;
        pilka[i].y = 300;
        pilka[i].szybkosc = 4;
        pilka[i].ruch_lewo = false;
        pilka[i].ruch_dol = true;
        pilka[i].widoczna = true;
        z += 10;
    }
    pilka[1].widoczna = false;
    pilka[2].widoczna = false;

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

        for (int j = 0; j < 3; j++) {
            if (pilka[j].widoczna) {
                struct Cegielki *trafiona_cegielka = szukaj_w_drzewie(quadTree, &pilka[j]);
                // trafienie cegly przez pilke
                if (trafiona_cegielka != NULL && trafiona_cegielka->wytrzymalosc > 0) {
                    trafiona_cegielka->wytrzymalosc --;
                    if (trafiona_cegielka->wytrzymalosc <= 0) {
                        ustawienia_gry.ilosc_cegiel_do_zbicia--;
                        // czy wyswietlic bonus
                        if (czy_dodac_bonus(&ustawienia_gry)) {
                            ustawienie_bonusu(&bonus, trafiona_cegielka);
                        }
                    }
                    kolizja_cegla(trafiona_cegielka, &pilka[j], hit_sound2);
                    dodaj_punkty(&gracz, trafiona_cegielka);

                    if (ustawienia_gry.ilosc_cegiel_do_zbicia <= 0) {
                        przejscie_do_kolejnego_poziomu(&ustawienia_gry, &pilka[j], &gracz, &bonus);
                        inicjalizacja_cegielek(cegielki, &ustawienia_gry);
                        break;
                    }
                }
            }
        }

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if (!ustawienia_gry.wyswietl_ekran_przegranej && !ustawienia_gry.wyswietl_menu) {
                    // mechanika bonusu
                    if (ustawienia_gry.wyswietlany_bonus) {
                        ruch_w_dol_bonusu(&bonus, &ustawienia_gry);
                    }
                    if (kolizja_bonus(&gracz, &bonus) && ustawienia_gry.wyswietlany_bonus) {
                        if (bonus.typ_bonusu == 0) {
                            gracz.zycie++;
                        }
                        else if (bonus.typ_bonusu == 1) {
                            gracz.szerokosc_platformy = 100;
                            bonus.trwa = true;
                            bonus.czas_przed = clock();
                        }
                        else if (bonus.typ_bonusu == 2) {
                            ustawienia_gry.ilosc_pilek = 3;
                            ustaw_pilki(&pilka, &ustawienia_gry);
                        }
                        ustawienia_gry.wyswietlany_bonus = false;
                    }
                    // czas trwania bonusu
                    if (bonus.trwa) {
                        clock_t roznica = clock() - bonus.czas_przed;
                        int msec = roznica * 1000 / CLOCKS_PER_SEC;
                        if (msec > 10000) {
                            bonus.trwa = false;
                            gracz.szerokosc_platformy = 50;
                        }
                    }
                    // pilka sterowanie
                    for (int j = 0; j < 3; j++) {
                        if (pilka[j].widoczna) {
                            pilka_kolizja_z_ramka(&pilka[j], &gracz, hit_sound1, &ustawienia_gry);
                        }
                    }
                    if (gracz.zycie <= 0) {
                        ustawienia_gry.wyswietl_ekran_przegranej = true;
                    }

                    for (int j = 0; j < 3; j++) {
                        pilka_kolizja_z_graczem(&pilka[j], gracz, hit_sound1);
                        pilka[j].x = ruch_pilki_x(pilka[j]);
                        pilka[j].y = ruch_pilki_y(pilka[j]);
                    }

                    if(key[ALLEGRO_KEY_LEFT])
                        gracz.x_pozycja = ruch_w_lewo(gracz.x_pozycja, gracz.szybkosc_gracza);
                    if(key[ALLEGRO_KEY_RIGHT])
                        gracz.x_pozycja = ruch_w_prawo(gracz.x_pozycja, gracz.szybkosc_gracza, gracz.szerokosc_platformy);
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
                if (gracz.szerokosc_platformy == 100) {
                    al_draw_bitmap(grafiki.platforma_bonus, gracz.x_pozycja, gracz.y_pozycja, 0);
                }
                else {
                    al_draw_bitmap(grafiki.platforma, gracz.x_pozycja, gracz.y_pozycja, 0);
                }

                // rysowanie cegielek
                rysowanie_cegielek(cegielki, grafiki);

                // rysowanie bonusu
                if (ustawienia_gry.wyswietlany_bonus) {
                    narysuj_bonus(grafiki.bonus, bonus);
                }

                // rysowanie pilki
                for (int j = 0; j < 3; j++) {
                    if (pilka[j].widoczna) {
                        al_draw_bitmap(grafiki.pilka, pilka[j].x, pilka[j].y, 0);
                    }
                }
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
    al_destroy_bitmap(grafiki.platforma_bonus);
    al_destroy_bitmap(grafiki.tlo);
    al_destroy_bitmap(grafiki.cegla_zi1);
    al_destroy_bitmap(grafiki.cegla_zo1);
    al_destroy_bitmap(grafiki.cegla_zo2);
    al_destroy_bitmap(grafiki.cegla_cz1);
    al_destroy_bitmap(grafiki.cegla_cz2);
    al_destroy_bitmap(grafiki.cegla_cz3);
    al_destroy_bitmap(grafiki.pilka);
    al_destroy_bitmap(grafiki.bonus);
    al_destroy_sample(music);
    al_destroy_sample(hit_sound1);
    al_destroy_sample(hit_sound2);

    return 0;
}
