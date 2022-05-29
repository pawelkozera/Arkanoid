#include "struktury.h"

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define SZEROKOSC_CEGIELKI 101
#define ILOSC_PRZYCISKOW 2

/// RUCH PLATFORMY W PRAWO
/** Przyjmuje 3 argumenty, pozycje x gracza, szybkosc poruszania sie gracza, szerokosc platformy gracza.
Wciśnięcie lewej strzałki porusza platformą w lewo.
Zwraca pozycje x gracza.*/
int ruch_w_prawo(int x_gracz, int szybkosc_gracza, int szerokosc_gracza) {
    x_gracz += szybkosc_gracza;
    if (x_gracz >= SZEROKOSC_EKRANU - szerokosc_gracza) {
        x_gracz = SZEROKOSC_EKRANU - szerokosc_gracza;
    }

    return x_gracz;
}

/// RUCH PLATFORMY W LEWO
/** Przyjmuje 2 argumenty, pozycje x gracza, szybkosc poruszania sie gracza.
Wciśnięcie prawej strzałki porusza platformą w prawo.
Zwraca pozycje x gracza.*/
int ruch_w_lewo(int x_gracz, int szybkosc_gracza) {
    x_gracz -= szybkosc_gracza;
    if (x_gracz <= 0) {
        x_gracz = 0;
    }

    return x_gracz;
}

/// RUCH PIŁKI PO OSI X
/** Przyjmuje strukture pilka
Zwraca wartosc int przesuniecie na osi x, w zaleznosci czy pilka leci w lewo czy w prawo.*/
int ruch_pilki_x(struct Pilka pilka) {
    if (pilka.ruch_lewo) {
        return pilka.x -= pilka.szybkosc;
    }
    else {
        return pilka.x += pilka.szybkosc;
    }
}

/// RUCH PIŁKI PO OSI Y
/** Przyjmuje strukture pilka
Zwraca wartosc int przesuniecie na osi y, w zaleznosci czy pilki leci w dol czy w gore.*/
int ruch_pilki_y(struct Pilka pilka) {
    if (pilka.ruch_dol) {
        return pilka.y += pilka.szybkosc;
    }
    else {
        return pilka.y -= pilka.szybkosc;
    }
}

/// SPRAWDZANIE NACIŚNIĘCIA PRZYCISKU MYSZKĄ
/** Przyjmuje 3 argumenty, pozycje x kursora, pozycje y kursora, tablice przyciskow
Zwraca index przycisku jesli kursor znajduje sie nad przyciskiem, w przeciwnym wypadku zwraca -1. */
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

/// LOSOWANIE BONUSU I GENERACJA
/** Przyjmuje 1 argument, wskaznik na ustawienia gry
Przy zniszczeniu cegły istnieje 30% szansa pojawienia się bonusu.
Zwraca true jesli mozna dodac bonus.*/
bool czy_dodac_bonus(struct Ustawienia_gry *ustawienia_gry) {
    if (ustawienia_gry->wyswietlany_bonus == false) {
        srand(time(NULL));
        int szansa = rand()%100 + 1;

        if (szansa <= 30) { // szansa
            ustawienia_gry->wyswietlany_bonus = true;
            return true;
        }
        return false;
    }
    return false;
}

/// RUCH BONUSU W DÓŁ
/** Przyjmuje 2 argumenty, wskaznik na bonus, wskaznik na ustawienia gry.
Zmienia pozycje y bonusu, az nie bedzie wiekszy niz wysokosc ekranu, wtedy zmienia ustawienie wyswietlania bonusu na false */
void ruch_w_dol_bonusu(struct Bonus *bonus, struct Ustawienia_gry *ustawienia_gry) {
    if (bonus->y > WYSOKOSC_EKRANU) {
        ustawienia_gry->wyswietlany_bonus = false;
    }
    else {
        bonus->y = bonus->y + bonus->szybkosc;
    }
}

/// USTAWIENIE BONUSU
/** Przyjmuje 2 argumenty, wskaznik na bonus, wskaznik na trafiona cegielke
Pojawienie się bonusu na środku zniszczonej cegły */
void ustawienie_bonusu(struct Bonus *bonus, struct Cegielki *trafiona_cegielka) {
    srand(time(NULL));
    bonus->typ_bonusu = rand()%3;
    bonus->x = trafiona_cegielka->x_pozycja + SZEROKOSC_CEGIELKI/2;
    bonus->y = trafiona_cegielka->y_pozycja;
}

/// USTAWIENIE POCZĄTKOWE PIŁKI
/** Przyjmuje 2 argumenty, wskaznik na pilke, wskaznik na ustawienia gry.
Ustawia dostepne pilki na pozycje startowe.
 */
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
