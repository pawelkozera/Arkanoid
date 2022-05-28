#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "struktury.h"

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define SZEROKOSC_CEGIELKI 101
#define WYSOKOSC_CEGIELKI 76

void pilka_kolizja_z_ramka(struct Pilka *pilka, struct Gracz *gracz, ALLEGRO_SAMPLE* hit_sound1, struct Ustawienia_gry *ustawienia_gry) {
    if (pilka->y >= WYSOKOSC_EKRANU) {
        pilka->widoczna = false;
        if (ustawienia_gry->ilosc_pilek > 1) {
            ustawienia_gry->ilosc_pilek--;
        }
        else {
            gracz->zycie--;
            gracz->x_pozycja = 450;
            pilka->x = 180;
            pilka->y = 300;
            pilka->ruch_lewo = false;
            pilka->ruch_dol = true;
            pilka->widoczna = true;
        }
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

bool kolizja_bonus(struct Gracz *gracz, struct Bonus *bonus) {
    bool y_pozycja = bonus->y >= gracz->y_pozycja && bonus->y <= gracz->y_pozycja + gracz->wysokosc;
    bool x_pozycja = bonus->x >= gracz->x_pozycja && bonus->x <= gracz->x_pozycja + gracz->szerokosc_platformy;
    bool x_pozycja_koncowa = bonus->x + bonus->szerokosc >= gracz->x_pozycja && bonus->x + bonus->szerokosc <= gracz->x_pozycja + gracz->szerokosc_platformy;
    bool y_pozycja_koncowa = bonus->y + bonus->wysokosc >= gracz->y_pozycja && bonus->y + bonus->wysokosc <= gracz->y_pozycja + gracz->wysokosc;

    if ((x_pozycja && y_pozycja) || (x_pozycja_koncowa && y_pozycja_koncowa)) {
        return true;
    }
    return false;
}
