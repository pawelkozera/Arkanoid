#include "struktury.h"

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define SZEROKOSC_CEGIELKI 101
#define ILOSC_PRZYCISKOW 2

int ruch_w_prawo(int x_gracz, int szybkosc_gracza, int szerokosc_gracza) {
    x_gracz += szybkosc_gracza;
    if (x_gracz >= SZEROKOSC_EKRANU - szerokosc_gracza) {
        x_gracz = SZEROKOSC_EKRANU - szerokosc_gracza;
    }

    return x_gracz;
}

int ruch_w_lewo(int x_gracz, int szybkosc_gracza) {
    x_gracz -= szybkosc_gracza;
    if (x_gracz <= 0) {
        x_gracz = 0;
    }

    return x_gracz;
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

bool czy_dodac_bonus(struct Ustawienia_gry *ustawienia_gry) {
    if (ustawienia_gry->wyswietlany_bonus == false) {
        srand(time(NULL));
        int szansa = rand()%100 + 1;

        if (szansa <= 10) { // szansa
            ustawienia_gry->wyswietlany_bonus = true;
            return true;
        }
        return false;
    }
    return false;
}

void ruch_w_dol_bonusu(struct Bonus *bonus, struct Ustawienia_gry *ustawienia_gry) {
    if (bonus->y > WYSOKOSC_EKRANU) {
        ustawienia_gry->wyswietlany_bonus = false;
    }
    else {
        bonus->y = bonus->y + bonus->szybkosc;
    }
}

void ustawienie_bonusu(struct Bonus *bonus, struct Cegielki *trafiona_cegielka) {
    srand(time(NULL));
    bonus->typ_bonusu = rand()%3;
    bonus->x = trafiona_cegielka->x_pozycja + SZEROKOSC_CEGIELKI/2;
    bonus->y = trafiona_cegielka->y_pozycja;
}

void ustaw_pilki(struct Pilka *pilka, struct Ustawienia_gry *ustawienia_gry) {
    int z = 0;
    for (int i = 0; i < ustawienia_gry->ilosc_pilek; i++) {
        pilka[i].x = 200 + z;
        pilka[i].y = WYSOKOSC_EKRANU - 200;
        pilka[i].ruch_dol = false;
        pilka[i].ruch_lewo = false;
        pilka[i].widoczna = true;
        z += 20;
    }
}
