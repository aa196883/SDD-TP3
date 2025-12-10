#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdd_dico.h"
#include "sdd_synonymes.h"

/* ============================================================
 *   Fonctions AVL pour le dictionnaire (T_Dico)
 * ============================================================ */

/* Hauteur d'un AVL de dictionnaire */
int hauteur_dico(T_Dico d) {
    if (d == NULL) return 0;
    int hg = hauteur_dico(d->gauche);
    int hd = hauteur_dico(d->droite);
    return (hg > hd ? hg : hd) + 1;
}

/* Met à jour d->eq et renvoie le facteur d'équilibre */
int facteur_eq_dico(T_Dico d) {
    if (d == NULL) return 0;
    int hg = hauteur_dico(d->gauche);
    int hd = hauteur_dico(d->droite);
    d->eq = hd - hg;   /* même convention que pour les synonymes */
    return d->eq;
}

/* Création d'un noeud de dictionnaire */
NoeudDico* creer_noeud_dico(T_Mot mot, T_Syn s) {
    NoeudDico *n = (NoeudDico*)malloc(sizeof(NoeudDico));
    if (n == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour NoeudDico\n");
        exit(EXIT_FAILURE);
    }
    strncpy(n->mot, mot, TAILLE_MOT);
    n->mot[TAILLE_MOT - 1] = '\0';
    n->syn = s;      /* on stocke l'ensemble de synonymes tel quel */
    n->eq = 0;
    n->gauche = NULL;
    n->droite = NULL;
    return n;
}

/* Rotations pour le dictionnaire */

void rotation_droite_dico(T_Dico *d) {
    T_Dico x = *d;
    T_Dico y = x->gauche;
    T_Dico T2 = y->droite;

    y->droite = x;
    x->gauche = T2;
    *d = y;

    facteur_eq_dico(x);
    facteur_eq_dico(y);
}

void rotation_gauche_dico(T_Dico *d) {
    T_Dico x = *d;
    T_Dico y = x->droite;
    T_Dico T2 = y->gauche;

    y->gauche = x;
    x->droite = T2;
    *d = y;

    facteur_eq_dico(x);
    facteur_eq_dico(y);
}

/* Fusion des ensembles de synonymes :
 * on parcourt 'source' et on ajoute tous ses mots dans *dest.
 */
void fusion_synonymes(T_Syn source, T_Syn *dest) {
    if (source == NULL) return;
    fusion_synonymes(source->gauche, dest);
    ajout_synonyme(dest, source->mot);
    fusion_synonymes(source->droite, dest);
}

/* Rééquilibrage du dictionnaire après insertion du mot 'mot_insere' */
void equilibrer_dico(T_Dico *d, T_Mot mot_insere) {
    if (*d == NULL) return;

    int balance = facteur_eq_dico(*d);

    /* Cas gauche-gauche */
    if (balance < -1 && strcmp(mot_insere, (*d)->gauche->mot) < 0) {
        rotation_droite_dico(d);
    }
    /* Cas droite-droite */
    else if (balance > 1 && strcmp(mot_insere, (*d)->droite->mot) > 0) {
        rotation_gauche_dico(d);
    }
    /* Cas gauche-droite */
    else if (balance < -1 && strcmp(mot_insere, (*d)->gauche->mot) > 0) {
        rotation_gauche_dico(&((*d)->gauche));
        rotation_droite_dico(d);
    }
    /* Cas droite-gauche */
    else if (balance > 1 && strcmp(mot_insere, (*d)->droite->mot) < 0) {
        rotation_droite_dico(&((*d)->droite));
        rotation_gauche_dico(d);
    }
}

/* Insertion récursive dans le dictionnaire */
void insertion_dico(T_Dico *d, T_Mot mot, T_Syn s) {
    if (*d == NULL) {
        *d = creer_noeud_dico(mot, s);
        return;
    }

    int cmp = strcmp(mot, (*d)->mot);

    if (cmp == 0) {
        /* Le mot existe déjà : on fusionne les ensembles de synonymes */
        fusion_synonymes(s, &((*d)->syn));
        return;
    }
    else if (cmp < 0) {
        insertion_dico(&((*d)->gauche), mot, s);
    }
    else { /* cmp > 0 */
        insertion_dico(&((*d)->droite), mot, s);
    }

    /* Rééquilibrage */
    equilibrer_dico(d, mot);
}

static void ajouter_mots_du_bloc(T_Dico *d, T_Syn ensemble, T_Syn syn) {
    if (ensemble == NULL) return;

    ajouter_mots_du_bloc(d, ensemble->gauche, syn);
    D_ajout_entree(d, ensemble->mot, syn);
    ajouter_mots_du_bloc(d, ensemble->droite, syn);
}

/* Recherche d'un mot dans le dictionnaire */
NoeudDico* chercher_mot_dico(T_Dico d, T_Mot mot) {
    if (d == NULL) return NULL;
    int cmp = strcmp(mot, d->mot);
    if (cmp == 0) return d;
    else if (cmp < 0) return chercher_mot_dico(d->gauche, mot);
    else return chercher_mot_dico(d->droite, mot);
}

/* ============================================================
 *   Interface sdd_dico.h
 * ============================================================ */

void D_init_vide(T_Dico *d) {
    *d = NULL;
}

void D_ajout_entree(T_Dico *d, T_Mot mot, T_Syn s) {
    insertion_dico(d, mot, s);
}

/* Chargement depuis un fichier :
 * Format :
 *   N_ENT
 *   mot1
 *   mot2
 *   ...
 *   N_ENT
 *   motX
 *   ...
 */
void charger_dico(const char *nom_fichier, T_Dico *d) {
    FILE *f = fopen(nom_fichier, "r");
    if (f == NULL) {
        perror("Erreur ouverture fichier dictionnaire");
        return;
    }

    char ligne[256];
    T_Syn ensemble;
    int dans_bloc = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        /* retirer \r et \n éventuels */
        ligne[strcspn(ligne, "\r\n")] = '\0';

        /* On change d'ensemble de synonymes */
        if (strcmp(ligne, "N_ENT") == 0) {
            /* Si on était déjà dans un bloc, on l'ajoute au dico */
            if (dans_bloc) {
                ajouter_mots_du_bloc(d, ensemble, ensemble);
            }

            dans_bloc = 1;
            S_init_vide(&ensemble);
            continue;
        }
        /* On est dans un bloc */
        else if (dans_bloc && ligne[0] != '\0') {
            /* mot du bloc courant */
            T_Mot mot;
            strncpy(mot, ligne, TAILLE_MOT);
            mot[TAILLE_MOT - 1] = '\0';
            ajout_synonyme(&ensemble, mot);
        }
    }

    /* Ajouter le dernier bloc si besoin */
    if (dans_bloc) {
        ajouter_mots_du_bloc(d, ensemble, ensemble);
    }

    fclose(f);
}

/* Renvoie l'ensemble de synonymes associé à 'mot' */
T_Syn liste_syn(T_Dico d, T_Mot mot) {
    NoeudDico *n = chercher_mot_dico(d, mot);
    if (n == NULL) return NULL;
    return n->syn;
}

/* Teste si mot2 est synonyme de mot1 dans le dico */
int est_synonyme_de(T_Dico d, T_Mot mot1, T_Mot mot2) {
    T_Syn s = liste_syn(d, mot1);
    if (s == NULL) return 0;
    return appartient_a(mot2, s);
}

/* Construction de l'intersection de deux ensembles de synonymes :
 * on parcourt s1 et on teste l'appartenance à s2.
 */
void synonymes_communs_rec(T_Syn s1, T_Syn s2, T_Syn *res) {
    if (s1 == NULL) return;
    synonymes_communs_rec(s1->gauche, s2, res);
    if (appartient_a(s1->mot, s2)) {
        ajout_synonyme(res, s1->mot);
    }
    synonymes_communs_rec(s1->droite, s2, res);
}

T_Syn synonymes_communs(T_Dico d, T_Mot mot1, T_Mot mot2) {
    T_Syn s1 = liste_syn(d, mot1);
    T_Syn s2 = liste_syn(d, mot2);
    T_Syn res;

    if (s1 == NULL || s2 == NULL) {
        /* mots inconnus ou sans synonymes */
        return NULL;
    }

    S_init_vide(&res);
    synonymes_communs_rec(s1, s2, &res);
    return res;
}

/* Affichage récursif du dictionnaire (ordre alphabétique) */

void afficher_dico(T_Dico d) {
    if (d == NULL) return;

    afficher_dico(d->gauche);

    printf("Mot : %s\n", d->mot);
    printf("Synonymes :\n");
    if (d->syn == NULL) {
        printf("  (aucun)\n");
    } else {
        afficher_synonymes(d->syn);
    }
    printf("----------\n");

    afficher_dico(d->droite);
}

/* Libération mémoire du dictionnaire */
void liberer_dico(T_Dico d) {
    if (d == NULL) return;
    liberer_dico(d->gauche);
    liberer_dico(d->droite);
    liberer_synonymes(d->syn);
    free(d);
}
