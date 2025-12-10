#include <stdio.h>
#include <string.h>
#include "sdd_dico.h"

int main(void) {
    T_Dico d;
    int choix;
    char nom_fichier[256];
    T_Mot mot, mot2;

    D_init_vide(&d);

    do {
        printf("=== Dictionnaire de synonymes ===\n");
        printf("1. Charger un dictionnaire depuis un fichier\n");
        printf("2. Lister les synonymes d'un mot\n");
        printf("3. Tester si un mot est synonyme d'un autre\n");
        printf("4. Afficher les synonymes communs de deux mots\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        getchar(); /* mange le '\n' */

        switch (choix) {
            case 1:
                printf("Nom du fichier : ");
                fgets(nom_fichier, sizeof(nom_fichier), stdin);
                nom_fichier[strcspn(nom_fichier, "\r\n")] = '\0';
                charger_dico(nom_fichier, &d);
                break;

            case 2:
                printf("Mot : ");
                fgets(mot, sizeof(mot), stdin);
                mot[strcspn(mot, "\r\n")] = '\0';
                {
                    T_Syn s = liste_syn(d, mot);
                    if (s == NULL) {
                        printf("Mot inconnu ou pas de synonymes.\n");
                    } else {
                        afficher_synonymes(s);
                    }
                }
                break;

            case 3:
                printf("Mot 1 : ");
                fgets(mot, sizeof(mot), stdin);
                mot[strcspn(mot, "\r\n")] = '\0';
                printf("Mot 2 : ");
                fgets(mot2, sizeof(mot2), stdin);
                mot2[strcspn(mot2, "\r\n")] = '\0';

                if (est_synonyme_de(d, mot, mot2))
                    printf("%s est synonyme de %s.\n", mot2, mot);
                else
                    printf("%s n'est PAS synonyme de %s.\n", mot2, mot);
                break;

            case 4:
                printf("Mot 1 : ");
                fgets(mot, sizeof(mot), stdin);
                mot[strcspn(mot, "\r\n")] = '\0';
                printf("Mot 2 : ");
                fgets(mot2, sizeof(mot2), stdin);
                mot2[strcspn(mot2, "\r\n")] = '\0';
                {
                    T_Syn communs = synonymes_communs(d, mot, mot2);
                    if (communs == NULL) {
                        printf("Aucun synonyme commun ou mot(s) inconnu(s).\n");
                    } else {
                        afficher_synonymes(communs);
                    }
                }
                break;
        }
    } while (choix != 0);

    return 0;
}
