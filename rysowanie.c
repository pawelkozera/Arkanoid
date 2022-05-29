#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "struktury.h"

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600

extern int KOLUMNA_CEGIELEK;
extern int WIERSZ_CEGIELEK;

/// RYSOWANIE CEGIEŁEK
/** Przyjmuje 2 argumenty, wskaznik na dynamiczna strukture cegielki, wskaznik na grafiki
Funkcja rysuje cegiełki na planszy. */
void rysowanie_cegielek(struct Cegielki *cegielki, struct Grafiki grafiki) {
    for(int i = 0; i < KOLUMNA_CEGIELEK; i++) {
        for(int j = 0; j < WIERSZ_CEGIELEK; j++) {
            if (cegielki[i*WIERSZ_CEGIELEK + j].typ_cegly == 1 && cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 1) {
                al_draw_bitmap(grafiki.cegla_zi1, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
            }
            else if (cegielki[i*WIERSZ_CEGIELEK + j].typ_cegly == 2) {
                if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 2) {
                    al_draw_bitmap(grafiki.cegla_zo2, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
                else if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 1) {
                    al_draw_bitmap(grafiki.cegla_zo1, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
            }
            else if (cegielki[i*WIERSZ_CEGIELEK + j].typ_cegly == 3) {
                if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 3) {
                    al_draw_bitmap(grafiki.cegla_cz3, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
                else if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 2) {
                    al_draw_bitmap(grafiki.cegla_cz2, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
                else if (cegielki[i*WIERSZ_CEGIELEK + j].wytrzymalosc == 1){
                    al_draw_bitmap(grafiki.cegla_cz1, cegielki[i*WIERSZ_CEGIELEK + j].x_pozycja, cegielki[i*WIERSZ_CEGIELEK + j].y_pozycja, 0);
                }
            }
        }
    }
}

/// RYSOWANIE INTERFEJSU
/** Przyjmuje 3 argumenty, wskaznik na font, wskaznik na gracza, wskaznik na ustawienia gry
Funkcja rysuje interfejs z podstawowymi informacjami do gry */
void narysuj_interfejs(ALLEGRO_FONT* font, struct Gracz gracz, struct Ustawienia_gry ustawienia_gry) {
    char komunikat_zycie[20] = "Ilosc zyc: ";
    char ilosc_zyc[2];
    sprintf(ilosc_zyc, "%d", gracz.zycie);
    strncat(komunikat_zycie, ilosc_zyc, 2);

    char komunikat_poziom[20] = "Poziom: ";
    char poziom[2];
    sprintf(poziom, "%d", ustawienia_gry.poziom_gry);
    strncat(komunikat_poziom, poziom, 2);

    char komunikat_punkty[20] = "Punkty: ";
    char punkty[10];
    sprintf(punkty, "%d", gracz.ilosc_punktow);
    strncat(komunikat_punkty, punkty, 10);

    al_draw_text(font, al_map_rgb(255, 255, 255), 50, WYSOKOSC_EKRANU - 100, 0, komunikat_zycie);
    al_draw_text(font, al_map_rgb(255, 255, 255), 200, WYSOKOSC_EKRANU - 100, 0, komunikat_poziom);
    al_draw_text(font, al_map_rgb(255, 255, 255), 350, WYSOKOSC_EKRANU - 100, 0, komunikat_punkty);
}

/// RYSOWANIE EKRAN PRZEGRANEJ
/** Przyjmuje 3 argumenty, wskaznik na font, wskaznik na ustawienia gry, wskaznik na gracza
Funkcja rysuje okno z podstawowymi informacjami z zakończonej gry. */
void narysuj_ekran_przegranej(ALLEGRO_FONT* font, struct Ustawienia_gry ustawienia_gry, struct Gracz gracz) {
    char komunikat_poziom[20] = "Poziom: ";
    char poziom[2];
    sprintf(poziom, "%d", ustawienia_gry.poziom_gry);
    strncat(komunikat_poziom, poziom, 2);

    char komunikat_punkty[20] = "Punkty: ";
    char punkty[10];
    sprintf(punkty, "%d", gracz.ilosc_punktow);
    strncat(komunikat_punkty, punkty, 10);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/3, 0, komunikat_poziom);
    al_draw_text(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/2, WYSOKOSC_EKRANU/3, 0, komunikat_punkty);

    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/2, 0, "Nacisnij [Enter]  aby rozpoczac nowa gra");
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/1.8, 0, "Nacisnij [Escape] aby wyjsc do menu");
}

/// RYSOWANIE MENU
/** Przyjmuje 2 argumenty, wskaznik na font, wskaznik na przycisk
Rysuje przycisk na ekran.
 */
void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk)
{
    al_draw_filled_rectangle(przycisk.x, przycisk.y, przycisk.x + przycisk.szerokosc, przycisk.y + przycisk.wysokosc, al_map_rgb(112, 110, 104));
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk.x + 10, przycisk.y + (przycisk.wysokosc/2), 0, przycisk.napis);
}

/// RYSOWANIE BONUSU
/** Przyjmuje 2 argumenty, wskaznik na grafike, wskaznik na bonus
Rysuje grafike bonusu na ekran */
void narysuj_bonus(ALLEGRO_BITMAP* bonus_grafika, struct Bonus bonus) {
    al_draw_bitmap(bonus_grafika, bonus.x, bonus.y, 0);
}
