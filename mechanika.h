#ifndef MECHANIKA_H_INCLUDED
#define MECHANIKA_H_INCLUDED

int ruch_w_prawo(int x_gracz, int szybkosc_gracza, int szerokosc_gracza);
int ruch_w_lewo(int x_gracz, int szybkosc_gracza);
int ruch_pilki_x(struct Pilka pilka);
int ruch_pilki_y(struct Pilka pilka);
int sprawdz_nacisniecie_przycisku(int mysz_x, int mysz_y, struct Przycisk przyciski[]);

#endif // MECHANIKA_H_INCLUDED
