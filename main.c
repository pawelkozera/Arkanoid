#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define KEY_SEEN     1
#define KEY_RELEASED 2

/*
    Dokumentacja Allegro5
    https://github.com/liballeg/allegro_wiki/wiki/Allegro-Vivace%3A-Basic-game-structure
*/


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
    ALLEGRO_BITMAP* gracz = al_load_bitmap("obrazki/platforma_gracza.png");
    int x_gracz = (int) SZEROKOSC_EKRANU/2;
    int szybkosc_gracza = 5;

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    bool zakoncz_program = false;
    bool redraw = true;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                if(key[ALLEGRO_KEY_LEFT])
                    x_gracz -= szybkosc_gracza;
                    if (x_gracz <= 0) {
                        x_gracz = 0;
                    }
                if(key[ALLEGRO_KEY_RIGHT])
                    x_gracz += szybkosc_gracza;
                    if (x_gracz >= SZEROKOSC_EKRANU - al_get_bitmap_width(gracz)) {
                        x_gracz = SZEROKOSC_EKRANU - al_get_bitmap_width(gracz);
                    }

                if(key[ALLEGRO_KEY_ESCAPE])
                    zakoncz_program = true;

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
                zakoncz_program = true;
                break;
        }

        if (zakoncz_program) {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // rysowanie platformy gracza
            al_draw_bitmap(gracz, x_gracz, WYSOKOSC_EKRANU - 20, 0);

            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    al_destroy_bitmap(gracz);

    return 0;
}
