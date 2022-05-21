#ifndef STANY_GRY_H_INCLUDED
#define STANY_GRY_H_INCLUDED

void inicjalizacja_cegielek(struct Cegielki *cegielki, struct Ustawienia_gry *ustawienia_gry);
void dodaj_punkty(struct Gracz *gracz, struct Cegielki *trafiona_cegla);
void przejscie_do_kolejnego_poziomu(struct Ustawienia_gry *ustawienia_gry, struct Pilka *pilka, struct Gracz *gracz);
void reset_gry(struct Gracz *gracz, struct Ustawienia_gry *ustawienia_gry, struct Pilka *pilka);

#endif // STANY_GRY_H_INCLUDED
