#ifndef SDD_SYNONYMES_H
#define SDD_SYNONYMES_H

#define TAILLE_MOT 64

typedef char T_Mot[TAILLE_MOT];

typedef struct noeud_syn {
    T_Mot mot;
    int eq;   /* facteur d'équilibre: -1, 0 ou +1 */
    struct noeud_syn *gauche;
    struct noeud_syn *droite;
} NoeudSyn;

typedef NoeudSyn* T_Syn;

/* Initialise un ensemble vide (pointeur NULL) */
void S_init_vide(T_Syn *s);

/* Ajoute un mot m dans l'ensemble s (AVL), si pas déjà présent.
   Maintient l’équilibre AVL. */
void ajout_synonyme(T_Syn *s, T_Mot m);

/* Renvoie 1 si m appartient à s, 0 sinon */
int appartient_a(T_Mot m, T_Syn s);

/* (facultatif pour le TP, mais pratique pour le main) */
void afficher_synonymes(T_Syn s);

/* Libère la mémoire allouée pour l'ensemble de synonymes s */
void liberer_synonymes(T_Syn s);

#endif
