#ifndef KOLIZJE_H_INCLUDED
#define KOLIZJE_H_INCLUDED

void pilka_kolizja_z_ramka(struct Pilka *pilka, struct Gracz *gracz, ALLEGRO_SAMPLE* hit_sound1);
void pilka_kolizja_z_graczem(struct Pilka *pilka, struct Gracz gracz, ALLEGRO_SAMPLE* hit_sound1);
void kolizja_cegla(struct Cegielki *trafiona_cegielka, struct Pilka *pilka, ALLEGRO_SAMPLE* hit_sound2);

#endif // KOLIZJE_H_INCLUDED
