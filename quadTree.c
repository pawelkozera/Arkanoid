#include "struktury.h"

#define SZEROKOSC_CEGIELKI 101
#define WYSOKOSC_CEGIELKI 76

struct QuadTree* stworz_drzewo(int lewy_x, int lewy_y, int prawy_x, int prawy_y) {
    struct QuadTree* drzewo = (struct QuadTree*)malloc(sizeof(struct QuadTree));

    drzewo->lewy_x = lewy_x;
    drzewo->lewy_y = lewy_y;
    drzewo->prawy_x = prawy_x;
    drzewo->prawy_y = prawy_y;

    drzewo->cegielki = NULL;
    drzewo->pg = NULL;
    drzewo->lg = NULL;
    drzewo->pd = NULL;
    drzewo->ld = NULL;

    return drzewo;
}

bool czy_jest_w_granicy_cegly(struct QuadTree *quadTree, struct Cegielki *cegielka) {
    bool x_granica = cegielka->x_pozycja >= quadTree->lewy_x && cegielka->x_pozycja <= quadTree->prawy_x;
    bool y_granica = cegielka->y_pozycja >= quadTree->lewy_y && cegielka->y_pozycja <= quadTree->prawy_y;

    return x_granica && y_granica;
}

bool czy_jest_w_granicy_pilka(struct QuadTree *quadTree, struct Pilka *pilka) {
    bool x_granica = pilka->x >= quadTree->lewy_x && pilka->x <= quadTree->prawy_x;
    bool y_granica = pilka->y >= quadTree->lewy_y && pilka->y <= quadTree->prawy_y;

    return x_granica && y_granica;
}

void dodaj_do_drzewa(struct QuadTree *quadTree, struct Cegielki *cegielka) {
    if (cegielka == NULL) {
        return;
    }

    if (!czy_jest_w_granicy_cegly(quadTree, cegielka)) {
        return;
    }

    if (abs(quadTree->lewy_x - quadTree->prawy_x) <= SZEROKOSC_CEGIELKI && abs(quadTree->lewy_y - quadTree->prawy_y) <= WYSOKOSC_CEGIELKI) {
        if (quadTree->cegielki == NULL) {
            quadTree->cegielki = cegielka;
            /*
            printf("dodano cegle na %d\n", quadTree->lewy_x);
            printf("dodano cegle na %d\n", quadTree->lewy_y);
            printf("dodano cegle na %d\n", quadTree->prawy_x);
            printf("dodano cegle na %d\n", quadTree->prawy_y);
            printf("dodano cegle na %d\n", quadTree->cegielki->x_pozycja);
            puts("----");
            */
        }
        return;
    }

    if ((quadTree->lewy_x + quadTree->prawy_x)/2 >= cegielka->x_pozycja) {
        if ((quadTree->lewy_y + quadTree->prawy_y)/2 >= cegielka->y_pozycja) {
            if (quadTree->lg == NULL) {
                quadTree->lg = stworz_drzewo(quadTree->lewy_x, quadTree->lewy_y, (quadTree->lewy_x + quadTree->prawy_x)/2, (quadTree->lewy_y + quadTree->prawy_y)/2);
            }
            dodaj_do_drzewa(quadTree->lg, cegielka);
        }
        else {
            if (quadTree->ld == NULL) {
                quadTree->ld = stworz_drzewo(quadTree->lewy_x, (quadTree->lewy_y + quadTree->prawy_y)/2, (quadTree->lewy_x + quadTree->prawy_x)/2, quadTree->prawy_y);
            }
            dodaj_do_drzewa(quadTree->ld, cegielka);
        }
    }
    else {
        if ((quadTree->lewy_y + quadTree->prawy_y)/2 >= cegielka->y_pozycja) {
            if (quadTree->pg == NULL) {
                quadTree->pg = stworz_drzewo((quadTree->lewy_x + quadTree->prawy_x)/2, quadTree->lewy_y, quadTree->prawy_x, (quadTree->lewy_y + quadTree->prawy_y)/2);
            }
            dodaj_do_drzewa(quadTree->pg, cegielka);
        }
        else {
            if (quadTree->pd == NULL) {
                quadTree->pd = stworz_drzewo((quadTree->lewy_x + quadTree->prawy_x)/2, (quadTree->lewy_y + quadTree->prawy_y)/2, quadTree->prawy_x, quadTree->prawy_y);
            }
            dodaj_do_drzewa(quadTree->pd, cegielka);
        }
    }
}

struct Cegielki* szukaj_w_drzewie(struct QuadTree *quadTree, struct Pilka *pilka) {
    if (!czy_jest_w_granicy_pilka(quadTree, pilka)) {
        return NULL;
    }

    if (quadTree->cegielki != NULL) {
        return quadTree->cegielki;
    }

    if ((quadTree->lewy_x + quadTree->prawy_x)/2 >= pilka->x) {
        if ((quadTree->lewy_y + quadTree->prawy_y)/2 >= pilka->y) {
            if (quadTree->lg == NULL) {
                return NULL;
            }
            return szukaj_w_drzewie(quadTree->lg, pilka);
        }
        else {
            if (quadTree->ld == NULL) {
                return NULL;
            }
            return szukaj_w_drzewie(quadTree->ld, pilka);
        }
    }
    else {
        if ((quadTree->lewy_y + quadTree->prawy_y)/2 >= pilka->y) {
            if (quadTree->pg == NULL) {
                return NULL;
            }
            return szukaj_w_drzewie(quadTree->pg, pilka);
        }
        else {
            if (quadTree->pd == NULL) {
                return NULL;
            }
            return szukaj_w_drzewie(quadTree->pd, pilka);
        }
    }
}
