#ifndef SDD_DICO_H
#define SDD_DICO_H

#include "sdd_synonymes.h"

typedef struct noeud_dico {
    T_Mot mot;       /* mot vedette */
    T_Syn syn;       /* AVL des synonymes */
    int eq;          /* facteur d'équilibre */
    struct noeud_dico *gauche;
    struct noeud_dico *droite;
} NoeudDico;

typedef NoeudDico* T_Dico;

/* Initialise un dictionnaire vide (pointeur NULL) */
void D_init_vide(T_Dico *d);

/* Ajoute une entrée (mot, ensemble de synonymes s) dans le dico (AVL).
   Si le mot existe déjà, on peut décider de fusionner les ensembles s et d->syn. */
void D_ajout_entree(T_Dico *d, T_Mot mot, T_Syn s);

/* Chargement depuis un fichier texte au format décrit dans l’annexe */
void charger_dico(const char *nom_fichier, T_Dico *d);

/* Renvoie l'ensemble des synonymes d'un mot.
   Si le mot n'existe pas, renvoie NULL. (ou ensemble vide) */
T_Syn liste_syn(T_Dico d, T_Mot mot);

/* Renvoie 1 si mot2 est synonyme de mot1 dans ce dictionnaire, 0 sinon */
int est_synonyme_de(T_Dico d, T_Mot mot1, T_Mot mot2);

/* Renvoie un ensemble avec les synonymes communs à mot1 et mot2.
   Si l'un des mots n'existe pas, renvoie NULL ou ensemble vide. */
T_Syn synonymes_communs(T_Dico d, T_Mot mot1, T_Mot mot2);

/* (facultatif, mais utile pour debug) */
void afficher_dico(T_Dico d);

#endif
