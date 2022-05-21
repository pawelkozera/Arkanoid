#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

struct QuadTree* stworz_drzewo(int lewy_x, int lewy_y, int prawy_x, int prawy_y);
bool czy_jest_w_granicy_cegly(struct QuadTree *quadTree, struct Cegielki *cegielka);
bool czy_jest_w_granicy_pilka(struct QuadTree *quadTree, struct Pilka *pilka);
void dodaj_do_drzewa(struct QuadTree *quadTree, struct Cegielki *cegielka);
struct Cegielki* szukaj_w_drzewie(struct QuadTree *quadTree, struct Pilka *pilka);

#endif // QUADTREE_H_INCLUDED
