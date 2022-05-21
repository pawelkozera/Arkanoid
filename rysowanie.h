#ifndef RYSOWANIE_H_INCLUDED
#define RYSOWANIE_H_INCLUDED

void narysuj_ekran_przegranej(ALLEGRO_FONT* font, struct Ustawienia_gry ustawienia_gry, struct Gracz gracz);
void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk);
void rysowanie_cegielek(struct Cegielki *cegielki, struct Grafiki grafiki);
void narysuj_interfejs(ALLEGRO_FONT* font, struct Gracz gracz, struct Ustawienia_gry ustawienia_gry);

#endif // RYSOWANIE_H_INCLUDED
