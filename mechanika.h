#ifndef MECHANIKA_H_INCLUDED
#define MECHANIKA_H_INCLUDED

int ruch_w_prawo(int x_gracz, int szybkosc_gracza, int szerokosc_gracza);
int ruch_w_lewo(int x_gracz, int szybkosc_gracza);
int ruch_pilki_x(struct Pilka pilka);
int ruch_pilki_y(struct Pilka pilka);
int sprawdz_nacisniecie_przycisku(int mysz_x, int mysz_y, struct Przycisk przyciski[]);
bool czy_dodac_bonus(struct Ustawienia_gry *ustawienia_gry);
void ruch_w_dol_bonusu(struct Bonus *bonus, struct Ustawienia_gry *ustawienia_gry);
void ustawienie_bonusu(struct Bonus *bonus, struct Cegielki *trafiona_cegielka);
void ustaw_pilki(struct Pilka *pilka, struct Ustawienia_gry *ustawienia_gry);

#endif // MECHANIKA_H_INCLUDED
