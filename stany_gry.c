#include "struktury.h"

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define SZEROKOSC_CEGIELKI 101
#define WYSOKOSC_CEGIELKI 76

extern int KOLUMNA_CEGIELEK;
extern int WIERSZ_CEGIELEK;

/// LOSOWANIE CEGIEŁEK
/** Losowanie pozycji oraz odporności cegieł. */
void inicjalizacja_cegielek(struct Cegielki *cegielki, struct Ustawienia_gry *ustawienia_gry) {
    srand(time(NULL));
    int hp = 1;

    int numer_pozycji = rand()%5;
    int cegly_do_usuniecia = 0;

    if (numer_pozycji == 0) {
        KOLUMNA_CEGIELEK = 3;
        WIERSZ_CEGIELEK = 8;
    }
    else if (numer_pozycji == 1) {
        KOLUMNA_CEGIELEK = 3;
        WIERSZ_CEGIELEK = 4;
    }
    else if (numer_pozycji == 2) {
        KOLUMNA_CEGIELEK = 3;
        WIERSZ_CEGIELEK = 8;
    }
    else if (numer_pozycji == 3) {
        KOLUMNA_CEGIELEK = 3;
        WIERSZ_CEGIELEK = 8;
    }
    else if (numer_pozycji == 4) {
        KOLUMNA_CEGIELEK = 3;
        WIERSZ_CEGIELEK = 8;
    }

    for(int i = 0; i < KOLUMNA_CEGIELEK; i++) {
        for(int j = 0; j < WIERSZ_CEGIELEK; j++) {
            hp = 1 + rand()%ustawienia_gry->poziom_gry;
            if (hp > 3) {
                hp = 3;
            }
            cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc = hp;
            cegielki[i*WIERSZ_CEGIELEK + j].typ_cegly = hp;

            if (numer_pozycji == 0) {
                cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI;
                cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja = i * WYSOKOSC_CEGIELKI;
            }
            else if (numer_pozycji == 1) {
                cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI * 2;
                cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja = i * WYSOKOSC_CEGIELKI;
            }
            else if (numer_pozycji == 2) {
                if (i%2 == 0) {
                    cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI;
                }
                else {
                    cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI * 2;
                    if (j * SZEROKOSC_CEGIELKI * 2 >= SZEROKOSC_EKRANU) {
                        cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc = 0;
                        cegly_do_usuniecia++;
                    }
                }
                cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja = i * WYSOKOSC_CEGIELKI;
            }
            else if (numer_pozycji == 3) {
                if (i%2 != 0) {
                    cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI;
                }
                else {
                    cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI * 2;
                    if (j * SZEROKOSC_CEGIELKI * 2 >= SZEROKOSC_EKRANU) {
                        cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc = 0;
                        cegly_do_usuniecia++;
                    }
                }
                cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja = i * WYSOKOSC_CEGIELKI;
            }
            else if (numer_pozycji == 4) {
                if (i%2 != 0) {
                    cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI;
                    cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc = 0;
                    cegly_do_usuniecia++;
                }
                else {
                    cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja = j * SZEROKOSC_CEGIELKI;
                }
                cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja = i * WYSOKOSC_CEGIELKI;
            }
        }
    }
    ustawienia_gry->ilosc_cegiel_do_zbicia = KOLUMNA_CEGIELEK*WIERSZ_CEGIELEK - cegly_do_usuniecia;
}

/// DODAWANIE I ZLICZANIE PUNKTÓW
/**  */
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

/// PRZEJŚCIE DO NASTĘPNEGO POZIOMU
/** Po zbiciu wszystkich cegieł następuje przejście do kolejnego poziomu. */
void przejscie_do_kolejnego_poziomu(struct Ustawienia_gry *ustawienia_gry, struct Pilka *pilka, struct Gracz *gracz, struct Bonus *bonus) {
    ustawienia_gry->poziom_gry++;
    ustawienia_gry->wyswietlany_bonus = false;
    ustawienia_gry->ilosc_pilek = 1;
    bonus->trwa = false;
    pilka[0].x = 180;
    pilka[0].y = 300;
    pilka[0].ruch_lewo = false;
    pilka[0].ruch_dol = true;
    gracz->szerokosc_platformy = 50;
    gracz->x_pozycja = SZEROKOSC_EKRANU/2;
    gracz->y_pozycja = WYSOKOSC_EKRANU - 20;
}

/// URUCHOMIENIE GRY PONOWNIE
/**  */
void reset_gry(struct Gracz *gracz, struct Ustawienia_gry *ustawienia_gry, struct Pilka *pilka) {
    gracz->ilosc_punktow = 0;
    gracz->zycie = 3;
    gracz->x_pozycja = SZEROKOSC_EKRANU/2;

    ustawienia_gry->poziom_gry = 1;
    ustawienia_gry->wyswietlany_bonus = false;
    ustawienia_gry->ilosc_pilek = 1;

    pilka[0].x = 180;
    pilka[0].y = 300;
    pilka[0].widoczna = true;
    pilka[1].widoczna = pilka[2].widoczna = false;
}
