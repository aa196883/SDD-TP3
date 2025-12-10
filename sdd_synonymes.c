#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdd_synonymes.h"

/* ============================================================
 *   Fonctions AVL pour les synonymes (T_Syn)
 * ============================================================ */

/* Hauteur d'un AVL de synonymes */
int hauteur_syn(T_Syn s) {
    if (s == NULL) return 0;
    int hg = hauteur_syn(s->gauche);
    int hd = hauteur_syn(s->droite);
    return (hg > hd ? hg : hd) + 1;
}

/* Calcule et met à jour le facteur d'équilibre s->eq */
int facteur_eq_syn(T_Syn s) {
    if (s == NULL) return 0;
    int hg = hauteur_syn(s->gauche);
    int hd = hauteur_syn(s->droite);
    s->eq = hd - hg;   /* convention : eq = hauteur(droite) - hauteur(gauche) */
    return s->eq;
}

/* Création d'un nouveau noeud de synonyme */
NoeudSyn* creer_noeud_syn(T_Mot m) {
    NoeudSyn *n = (NoeudSyn*)malloc(sizeof(NoeudSyn));
    if (n == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour NoeudSyn\n");
        exit(EXIT_FAILURE);
    }
    strncpy(n->mot, m, TAILLE_MOT);
    n->mot[TAILLE_MOT - 1] = '\0';
    n->eq = 0;
    n->gauche = NULL;
    n->droite = NULL;
    return n;
}

/* Rotation droite autour de *s */
void rotation_droite_syn(T_Syn *s) {
    T_Syn x = *s;
    T_Syn y = x->gauche;
    T_Syn T2 = y->droite;

    /* rotation */
    y->droite = x;
    x->gauche = T2;
    *s = y;

    /* mise à jour des eq */
    facteur_eq_syn(x);
    facteur_eq_syn(y);
}

/* Rotation gauche autour de *s */
void rotation_gauche_syn(T_Syn *s) {
    T_Syn x = *s;
    T_Syn y = x->droite;
    T_Syn T2 = y->gauche;

    /* rotation */
    y->gauche = x;
    x->droite = T2;
    *s = y;

    /* mise à jour des eq */
    facteur_eq_syn(x);
    facteur_eq_syn(y);
}

/* Rééquilibrage d'un noeud après insertion */
void equilibrer_syn(T_Syn *s, T_Mot mot_insere) {
    if (*s == NULL) return;

    int balance = facteur_eq_syn(*s);

    /* Cas gauche-gauche */
    if (balance < -1 && strcmp(mot_insere, (*s)->gauche->mot) < 0) {
        rotation_droite_syn(s);
    }
    /* Cas droite-droite */
    else if (balance > 1 && strcmp(mot_insere, (*s)->droite->mot) > 0) {
        rotation_gauche_syn(s);
    }
    /* Cas gauche-droite */
    else if (balance < -1 && strcmp(mot_insere, (*s)->gauche->mot) > 0) {
        rotation_gauche_syn(&((*s)->gauche));
        rotation_droite_syn(s);
    }
    /* Cas droite-gauche */
    else if (balance > 1 && strcmp(mot_insere, (*s)->droite->mot) < 0) {
        rotation_droite_syn(&((*s)->droite));
        rotation_gauche_syn(s);
    }
}

/* Insertion récursive dans un AVL de synonymes */
void insertion_syn(T_Syn *s, T_Mot m) {
    if (*s == NULL) {
        *s = creer_noeud_syn(m);
        return;
    }

    int cmp = strcmp(m, (*s)->mot);

    if (cmp == 0) {
        /* Le mot existe déjà : pas d'insertion doublon */
        return;
    }
    else if (cmp < 0) {
        insertion_syn(&((*s)->gauche), m);
    }
    else { /* cmp > 0 */
        insertion_syn(&((*s)->droite), m);
    }

    /* Rééquilibrage à la remontée */
    equilibrer_syn(s, m);
}

/* ============================================================
 *   Fonctions de l'interface sdd_synonymes.h
 * ============================================================ */

void S_init_vide(T_Syn *s) {
    *s = NULL;
}

void ajout_synonyme(T_Syn *s, T_Mot m) {
    insertion_syn(s, m);
}

/* Test d'appartenance dans l'AVL des synonymes */
int appartient_a(T_Mot m, T_Syn s) {
    if (s == NULL) return 0;
    int cmp = strcmp(m, s->mot);
    if (cmp == 0) return 1;
    else if (cmp < 0) return appartient_a(m, s->gauche);
    else return appartient_a(m, s->droite);
}

/* Affichage trié des synonymes */
void afficher_synonymes(T_Syn s) {
    if (s == NULL) return;
    afficher_synonymes(s->gauche);
    printf("%s\n", s->mot);
    afficher_synonymes(s->droite);
}

/* Libère la mémoire */
void liberer_synonymes(T_Syn s) {
    if (s == NULL) return;
    liberer_synonymes(s->gauche);
    liberer_synonymes(s->droite);
    free(s);
}
