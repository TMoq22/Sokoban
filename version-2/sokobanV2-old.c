/**
 * @file sokoban.c
 * @brief jeu du Sokoban en C
 * @author Titouan Moquet
 * @version V2.1.9
 * @date 17/11/2025
 *
 * Jeu du Sokoban réalisé en C jouable dans le terminal dans le cadre de la
 * SAE 1.01, IUT Lannion Info 1D2 2025-2026
 *
 * indentation et formatage automatique : clangd
 */

// librairies inclusent
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
// taille du tableau
#define TAILLE 12
#define TAILLE_DEPLACEMENT 10000

// Code d'échappement ANSI (http://www.linuxfocus.org/,
// https://www.codequoi.com/,
// https://en.wikipedia.org/wiki/ANSI_escape_code#In_C)
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
// blink ne marche pas dans tout les terminaux
#define BLINK "\033[5m"
#define NOT_BLINK "\033[25m"

/* -- Déclaration de type -- */
typedef char t_plateau[TAILLE][TAILLE];
typedef char t_tabDeplacement[TAILLE_DEPLACEMENT];

/* -- Déclaration des constantes --*/
// touches
const char HAUT = 'z', BAS = 's', GAUCHE = 'q', DROITE = 'd';
const char RELOAD = 'r', FIN = 'x', UNDO = 'u';
// murs et chaisses
const char BORD = '#', OBJECTIF = '*', CAISSE = '$';
// joueur
const char JOUEUR = '@';
const char JOUEUR_SUR_CIBLE = '+';
// vide et cibles
const char CIBLE = '.';
const char VIDE = ' ';

const char YES = 'y';
const char NO = 'n';

const char VRAI = true;
const char FAUX = false;

const char ZOOM = '+';
const char DEZOOM = '-';
const int MAX_ZOOM = 3;
const int MIN_ZOOM = 1;

const char HAUT_SANS_CAISSE = 'h';
const char BAS_SANS_CAISSE = 'b';
const char DROITE_SANS_CAISSE = 'd';
const char GAUCHE_SANS_CAISSE = 'g';

const char HAUT_AVEC_CAISSE = 'H';
const char BAS_AVEC_CAISSE = 'B';
const char DROITE_AVEC_CAISSE = 'D';
const char GAUCHE_AVEC_CAISSE = 'G';

const int avecCaisse = 2;
const int sansCaisse = 1;
const int pasDeplacement = 0;

// Les prototypes des fonctions et procédures, détaille avant chaque fonction et
// procédures.
int kbhit();
void charger_partie(t_plateau plateau, char fichier[]);
void enregistrer_partie(t_plateau plateau, char fichier[]);
void sauvegarde_jeu(t_plateau plateau);
void afficher_entete(int nbDeplacement, char nomNiveau[]);
void affiche_plateau(t_plateau plateau, int niveauZoom);
void affiche_plateau_largeur(t_plateau plateau, int niveauZoom, int longueur);
void affichage_complet(t_plateau plateau, char nomNiveau[], int nbDeplacement,
                       int niveauZoom);
void recherche_pos_jo(t_plateau plateau, int *posJoX, int *posJoY);
void deplacer(t_plateau plateau, t_plateau plateauBase,
              t_tabDeplacement tabDeplacement, char touche, int *posJoX,
              int *posJoY, int *nbDeplacement);

void remplace_carac_deplacement(t_plateau tableau, t_plateau plateauBase,
                                int *posJoX, int *posJoY);
void annuler_deplacement(t_plateau plateau, t_tabDeplacement tabDeplacement,
                         int *posJoX, int *posJoY, int *nbDeplacement);

bool gagne(t_plateau plateau, t_plateau plateauBase);
void affiche_message_fin(int nbDeplacement);
void affiche_abandon();
void affiche_niveau();
void jeu(char touche, t_plateau plateau, t_plateau plateauBase,
         t_tabDeplacement tabDeplacement, int *posJoX, int *posJoY,
         int *nbDeplacement, char nomNiveau[], int *niveauZoom, bool gagner);
void memoriser_deplacement(char touche, t_tabDeplacement tabDeplacement,
                           int leDeplacement);

/**
 * @brief Entrée du programme
 * @return EXIT_SUCCESS : arrêt normal du programme
 * Fait appel à toutes les procedures et fonctions necessaires au jeu.
 */
int main() {
  // Declaration des variables
  t_plateau plateau, plateauBase;
  t_tabDeplacement tabDeplacement;
  int posJoX, posJoY, nbDeplacement, niveauZoom = 1;
  char nomNiveau[20], touche, jouer = YES;
  // Initialisation des donnees
  bool gagner = FAUX;
  nbDeplacement = 0;
  touche = '\0';
  posJoX = 0;
  posJoY = 0;
  system("clear");
  while (jouer != NO) {
    jouer = NO;
    affiche_niveau();
    scanf("%s", nomNiveau);
    // deux plateau 1 pour le jeu et un pour la sauvegarde des élements de base
    charger_partie(plateau, nomNiveau);
    charger_partie(plateauBase, nomNiveau);
    // premier affichage
    affichage_complet(plateau, nomNiveau, nbDeplacement, niveauZoom);
    // position initiale du joueur
    recherche_pos_jo(plateau, &posJoX, &posJoY);
    // jeu
    gagner = gagne(plateau, plateauBase);
    while ((touche != FIN) && (gagner == FAUX)) {
      if (kbhit()) {
        touche = getchar();
        jeu(touche, plateau, plateauBase, tabDeplacement, &posJoX, &posJoY,
            &nbDeplacement, nomNiveau, &niveauZoom, gagner);
        gagner = gagne(plateau, plateauBase);
        affichage_complet(plateau, nomNiveau, nbDeplacement, niveauZoom);
      }
      if (touche == FIN) {
        sauvegarde_jeu(plateau);
      }
    }
    if (gagner == VRAI) {
      system("clear");
      affiche_plateau(plateau, niveauZoom);
      affiche_message_fin(nbDeplacement);
    }
    if ((touche == FIN) || (gagner == VRAI)) {
      printf("voulez vous rejouer ?(y,n) : ");
      scanf(" %c", &jouer);
      touche = '\0';
    }
  }
  printf(YELLOW "\nAu revoir !\n" RESET);
  return EXIT_SUCCESS;
}

/**
 * @brief procedure qui permet de reduire la taille du main elle sert pour
 * l'appel de déplacer, gère le reload et le zoom/dezoom
 * @param touche
 * @param plateau
 * @param plateauBase
 * @param posJoX
 * @param posJoY
 * @param nbDeplacement
 * @param nomNiveau
 * @param niveauZoom
 * @param gagner
 */
void jeu(char touche, t_plateau plateau, t_plateau plateauBase,
         t_tabDeplacement tabDeplacement, int *posJoX, int *posJoY,
         int *nbDeplacement, char nomNiveau[], int *niveauZoom, bool gagner) {

  deplacer(plateau, plateauBase, tabDeplacement, touche, &(*posJoX), &(*posJoY),
           &(*nbDeplacement));
  if (touche == RELOAD) {
    charger_partie(plateau, nomNiveau);
    recherche_pos_jo(plateau, &(*posJoX), &(*posJoY));
    *nbDeplacement = 0;
    affichage_complet(plateau, nomNiveau, *nbDeplacement, *niveauZoom);
  }
  if ((touche == ZOOM) && (*niveauZoom < MAX_ZOOM)) {
    *niveauZoom += 1;
  }
  if ((touche == DEZOOM) && (*niveauZoom > MIN_ZOOM)) {
    *niveauZoom -= 1;
  }
}

// ---- fonctions et procedures ----

/**
 * @brief fonction qui permet la lecture d’une touche au clavier sans écho
 * @return 1 si un caractere est present 0 si pas de caractere présent
 */
int kbhit() {
  // la fonction retourne :
  // 1 si un caractere est present
  // 0 si pas de caractere présent
  int unCaractere = 0;
  struct termios oldt, newt;
  int ch;
  int oldf;

  // mettre le terminal en mode non bloquant
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  // restaurer le mode du terminal
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    unCaractere = 1;
  }
  return unCaractere;
}

/**
 * @brief Procédure pour le chargement du niveau, transforme un fichier texte en
 * tableau 2D.
 * @param plateau De type t_plateau pour remplir celui-ci grâce au fichier, en
 * entrer et en sortie
 * @param fichier le nom du fichier
 */
void charger_partie(t_plateau plateau, char fichier[]) {
  FILE *f;
  char finDeLigne;

  f = fopen(fichier, "r");
  if (f == NULL) {
    printf("ERREUR SUR FICHIER");
    exit(EXIT_FAILURE);
  } else {
    for (int ligne = 0; ligne < TAILLE; ligne++) {
      for (int colonne = 0; colonne < TAILLE; colonne++) {
        fread(&plateau[ligne][colonne], sizeof(char), 1, f);
      }
      fread(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
  }
}

/**
 * @brief Procédure pour enregistrer la partie, transforme un tableau 2D en
 * fichier texte.
 * @param plateau De type t_plateau pour récupérer les emplacements des
 * caractères du niveau
 * @param fichier le nom du fichier
 */
void enregistrer_partie(t_plateau plateau, char fichier[]) {
  FILE *f;
  char finDeLigne = '\n';

  f = fopen(fichier, "w");
  for (int ligne = 0; ligne < TAILLE; ligne++) {
    for (int colonne = 0; colonne < TAILLE; colonne++) {
      fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
    }
    fwrite(&finDeLigne, sizeof(char), 1, f);
  }
  fclose(f);
}

/**
 * @brief Procédure pour le choix en fin de partie (sauvegarde ou non), utilise
 * la procédure.
 * @param plateau de type t_plateau pour que la procedure "enregistrer_partie"
 * puisse avoir le plateau.
 */
void sauvegarde_jeu(t_plateau plateau) {
  char sauvegarde;
  char nomSauvegarde[20];
  affiche_abandon();
  printf(BOLD YELLOW "voulez vous sauvegarder ? (y,n) : " RESET);
  scanf("%c", &sauvegarde);
  if (sauvegarde == YES) {
    printf(BOLD YELLOW
           "nom du fichier (19 caractères max)(ex :nom.sok) : " RESET);
    scanf("%s", nomSauvegarde);
    enregistrer_partie(plateau, nomSauvegarde);
    printf(YELLOW "Partie sauvegardé !\n" RESET);
  }
}

/**
 * @brief Procédure pour le  déplacement du personnage et des caisses.
 * @param plateau de type t_plateau, tableau de jeu.
 * @param plateauBase de type t_plateau qui est une sauvegarde du plateau de
 * base.
 * @param touche de type caractère, est la valeur de la touche pressé,
 * par exemple 'z'.
 * @param posJoX de type entier, sur quel ligne du tableau est le joueur,
 * en entrer et sortie
 * @param posJoY de type entier; sur quel colonne du tableau est le joueur,
 * en entrer et sortie
 * @param nbDeplacement de type entier, compte le nombre de déplacements
 * effectué, en entrer et sortie
 */
void deplacer(t_plateau plateau, t_plateau plateauBase,
              t_tabDeplacement tabDeplacement, char touche, int *posJoX,
              int *posJoY, int *nbDeplacement) {
  int directionX = 0;
  int directionY = 0;
  int x = *posJoX;
  int y = *posJoY;
  int leDeplacement = pasDeplacement;

  // +1 ou -1 sur les x ou y en fonctions de la direction
  if (touche == HAUT) {
    directionX = -1; // haut -> indice ligne -1
  } else if (touche == BAS) {
    directionX = 1; //  bas ->indice ligne +1
  } else if (touche == GAUCHE) {
    directionY = -1; // gauche ->indice colonne -1
  } else if (touche == DROITE) {
    directionY = 1; // droite ->indice colonne +1
  }
  char caseSuivante = plateau[x + directionX][y + directionY];
  char caseApres;
  // verification si sortie du tableau pour eviter les erreurs
  if (((x + 2 * directionX >= 0) && (x + 2 * directionX < TAILLE)) &&
      ((y + 2 * directionY >= 0) && (y + 2 * directionY < TAILLE))) {
    caseApres = plateau[x + 2 * directionX][y + 2 * directionY];
  } else {
    return;
  }
  // deplacement :
  if (caseSuivante == BORD) {
    return;
  }
  if ((caseSuivante == CAISSE) || (caseSuivante == OBJECTIF)) {

    if ((caseApres == VIDE) || (caseApres == CIBLE)) {
      if (caseApres == CIBLE) {
        plateau[x + 2 * directionX][y + 2 * directionY] = OBJECTIF;

      } else {
        plateau[x + 2 * directionX][y + 2 * directionY] = CAISSE;
      }

      // ici pour les maguscules
      leDeplacement = avecCaisse;

    } else {
      return; // bloqué
    }
  }
  // remplacement du caractères ou etait le joueur
  remplace_carac_deplacement(plateau, plateauBase, posJoX, posJoY);
  // deplacement joueur
  if (plateauBase[x + directionX][y + directionY] == CIBLE) {
    plateau[x + directionX][y + directionY] = JOUEUR_SUR_CIBLE;
    if (leDeplacement == pasDeplacement) {
      leDeplacement = sansCaisse;
    }
    // ici sans maguscules

  } else {
    plateau[x + directionX][y + directionY] = JOUEUR;
    // ici sans maguscules
    if (leDeplacement == pasDeplacement) {
      leDeplacement = sansCaisse;
    }
  }
  memoriser_deplacement(touche, tabDeplacement, leDeplacement);

  *posJoX += directionX; // actualisation des coordonnées
  *posJoY += directionY;
  // comptage des déplacements
  if ((touche == HAUT) || (touche == BAS) || (touche == GAUCHE) ||
      (touche == DROITE)) {
    *nbDeplacement += 1;
  }
}

void memoriser_deplacement(char touche, t_tabDeplacement tabDeplacement,
                           int leDeplacement) {
  char caracDeplacement;
  if (leDeplacement == avecCaisse) {
    //printf("avec caisse");
    if (touche == HAUT) {
      caracDeplacement = HAUT_AVEC_CAISSE;
    } else if (touche == BAS) {
      caracDeplacement = BAS_AVEC_CAISSE;
    } else if (touche == GAUCHE) {
      caracDeplacement = GAUCHE_AVEC_CAISSE;
    } else if (touche == DROITE) {
      caracDeplacement = DROITE_AVEC_CAISSE;
    }

  } else if (leDeplacement == sansCaisse) {
    //printf("sans caisse");
    if (touche == HAUT) {
      caracDeplacement = HAUT_SANS_CAISSE;
    } else if (touche == BAS) {
      caracDeplacement = BAS_SANS_CAISSE;
    } else if (touche == GAUCHE) {
      caracDeplacement = GAUCHE_SANS_CAISSE;
    } else if (touche == DROITE) {
      caracDeplacement = DROITE_SANS_CAISSE;
    }
  }

}

void annuler_deplacement(t_plateau plateau, t_tabDeplacement tabDeplacement,
                         int *posJoX, int *posJoY, int *nbDeplacement) {
  if (tabDeplacement[*nbDeplacement]) {
    printf("otot");
  }
}

/**
 * @brief Procédure qui donne les coordonnées du joueur
 * @param plateau de type t_plateau, tableau de jeu.
 * @param posJoX de type entier, sur quel ligne du tableau est le joueur,
 * en entrer et sortie
 * @param posJoY de type entier; sur quel colonne du tableau est le joueur,
 * en entrer et sortie
 */
void recherche_pos_jo(t_plateau plateau, int *posJoX, int *posJoY) {
  /* pour chercher et ensuite récuperer les coordonnées du caractere*/
  for (int longueur = 0; longueur < TAILLE; longueur++) {
    for (int largeur = 0; largeur < TAILLE; largeur++) {
      if ((plateau[longueur][largeur] == JOUEUR) ||
          (plateau[longueur][largeur] == JOUEUR_SUR_CIBLE)) {
        *posJoX = longueur;
        *posJoY = largeur;
        return;
      }
    }
  }
}

/**
 * @brief Procédure pour remplacelement de caractère "après" déplacement du
 * joueur
 * @param plateau de type t_plateau, tableau de jeu.
 * @param plateauBase de type t_plateau qui est une sauvegarde du plateau de
 * base.
 * @param posJoX de type entier, sur quel ligne du tableau est le joueur,
 * en entrer et sortie
 * @param posJoY de type entier; sur quel colonne du tableau est le joueur,
 * en entrer et sortie
 */
void remplace_carac_deplacement(t_plateau plateau, t_plateau plateauBase,
                                int *posJoX, int *posJoY) {
  // remplace le joueurs par un point si il y a un point à la base
  if (plateauBase[*posJoX][*posJoY] == CIBLE) {
    plateau[*posJoX][*posJoY] = CIBLE;
  } else if (plateauBase[*posJoX][*posJoY] == OBJECTIF) {
    // remplace * par un point après le déplacement de la caisse
    plateau[*posJoX][*posJoY] = CIBLE;

  } else if (plateauBase[*posJoX][*posJoY] == JOUEUR_SUR_CIBLE) {
    // remplacement de + par . lors du déplacement
    plateau[*posJoX][*posJoY] = CIBLE;
  } else {
    // remplace le joueurs par un espace apres déplacement
    plateau[*posJoX][*posJoY] = VIDE;
  }
}

/**
 * @brief Fonction qui retourne vrai ou faux en fonction de si le joueus à
 * gagner ou non
 * @param plateau de type t_plateau, tableau de jeu.
 * @param plateauBase de type t_plateau qui est une sauvegarde du plateau de
 * base.
 */
bool gagne(t_plateau plateau, t_plateau plateauBase) {
  int nbCible = 0;
  int nbObjectifs = 0;
  bool gagner;
  for (int longueur = 0; longueur < TAILLE; longueur++) {
    for (int largeur = 0; largeur < TAILLE; largeur++) {
      if ((plateauBase[longueur][largeur] == CIBLE) ||
          (plateauBase[longueur][largeur] == OBJECTIF) ||
          (plateauBase[longueur][largeur] == JOUEUR_SUR_CIBLE)) {
        nbCible += 1;
        if (plateau[longueur][largeur] == OBJECTIF) {
          // dans le tableau c'est des * pas des $
          nbObjectifs += 1;
        }
      }
    }
  }
  if (nbObjectifs == nbCible) {
    gagner = VRAI;
  } else {
    gagner = FAUX;
  }
  return gagner;
}

// AFFICHAGE

/**
 * @brief Procédure qui affiche l'entete du jeu.
 * @param nbDeplacement de type entier, nombre de déplacement du joueur.
 * @param nomNiveau de type chaine de caractère, nom du niveau en cours.
 */
void afficher_entete(int nbDeplacement, char nomNiveau[]) {
  printf(BLUE "╔══════════════════════════════════════════╗\n");
  printf("║                                          ║\n");
  printf("║" RESET "                " BOLD " SOKOBAN " RESET
         "                 " BLUE "║\n" RESET);
  printf(BLUE "║                                          ║\n");
  printf("╚══════════════════════════════════════════╝\n" RESET);
  printf(BOLD CYAN "  Commandes :" RESET "         %20s\n", nomNiveau);
  printf(CYAN "────────────────────────────────────────────\n" RESET);
  printf(GREEN "   z : haut        ↑  ");
  printf("   s : bas         ↓\n");
  printf("   q : gauche      ←  ");
  printf("   d : droite      →\n" RESET);
  printf("\n");
  printf(GREEN "u : annuler le dernier déplacement" RESET);
  printf(YELLOW "   r : recommencer la partie\n" RESET);
  printf(RED "   x : quitter le jeu\n" RESET);
  // affichage du nombre déplacements (changement de l'alignement)
  printf(BLUE "╔══════════════════════════════════════════╗\n" RESET);
  if (nbDeplacement < 10) {
    printf(BLUE "║" RESET "         nombre déplacements : %d          " BLUE
                "║\n" RESET,
           nbDeplacement);
  } else if ((nbDeplacement >= 10) && (nbDeplacement < 100)) {
    printf(BLUE "║" RESET "         nombre déplacements : %d         " BLUE
                "║\n" RESET,
           nbDeplacement);
  } else {
    printf(BLUE "║" RESET "         nombre déplacements : %d        " BLUE
                "║\n" RESET,
           nbDeplacement);
  }
  printf(BLUE "╚══════════════════════════════════════════╝\n\n" RESET);
}

/**
 * @brief Procédure qui affiche le plateau de jeu,
 * remplace certains caratère par d'autre à l'affichage : ex '+' -> '@'.
 * @param plateau de type t_plateau, tableau de jeu.
 */
void affiche_plateau(t_plateau plateau, int niveauZoom) {
  int longueur;
  int zooml;

  for (longueur = 0; longueur < TAILLE; longueur++) {
    for (zooml = 0; zooml < niveauZoom; zooml++) {
      affiche_plateau_largeur(plateau, niveauZoom, longueur);
      printf("\n");
    }
  }
}

void affiche_plateau_largeur(t_plateau plateau, int niveauZoom, int longueur) {
  int largeur;
  int zoomc;
  for (largeur = 0; largeur < TAILLE; largeur++) {
    for (zoomc = 0; zoomc < niveauZoom; zoomc++) {

      if (plateau[longueur][largeur] == JOUEUR) {
        printf(RED "%c" RESET, JOUEUR);
      } else if (plateau[longueur][largeur] == OBJECTIF) {
        // affiche '$' mais dans le tableau c'est '*'
        printf(CYAN "%c" RESET, CAISSE);
      } else if (plateau[longueur][largeur] == JOUEUR_SUR_CIBLE) {
        printf(RED "%c" RESET, JOUEUR);
      } else if (plateau[longueur][largeur] == CAISSE) {
        printf(GREEN "%c" RESET, CAISSE);
      } else {
        printf("%c", plateau[longueur][largeur]);
      }
    }
  }
}

/**
 * @brief Procédure qui regroupe l'appet de affiche_entete et affiche_plateau.
 * @param plateau de type t_plateau, tableau de jeu.
 * @param nomNiveau chaine de caratères, nom du niveau en cours.
 * @param nbDeplacement entier, nombre de déplacement du joueur.
 */
void affichage_complet(t_plateau plateau, char nomNiveau[], int nbDeplacement,
                       int niveauZoom) {
  system("clear");

  afficher_entete(nbDeplacement, nomNiveau);
  affiche_plateau(plateau, niveauZoom);
  printf("Titouan Moquet 1D2 IUT Lannion 2025-2026\n");
}

/**
 * @brief Procédure qui affiche un message en fin de partie
 * @param nbDeplacement entier, nombre de déplacement du joueur.
 */
void affiche_message_fin(int nbDeplacement) {
  printf(BLUE "╔══════════════════════════════════════════╗\n");
  printf("║                                          ║\n");
  printf("║" RESET "              " BLINK BOLD "FÉLICITATIONS !" RESET NOT_BLINK
         "             " BLUE "║\n");
  printf("║                                          ║\n");
  printf("║" RESET "       " CYAN "Vous avez réussi le niveau !" RESET
         "       " BLUE "║\n");
  printf("║                                          ║\n");
  printf("╚══════════════════════════════════════════╝\n" RESET);
  printf("\n");
  printf(BOLD YELLOW "      Nombre total de déplacements : " RESET GREEN
                     "%d\n\n",
         nbDeplacement);
  printf(CYAN "────────────────────────────────────────────\n" RESET);
  printf(BOLD YELLOW "            Merci d’avoir joué !\n" RESET);
  printf(CYAN "────────────────────────────────────────────\n" RESET);
  printf("  Titouan Moquet 1D2 IUT Lannion 2025-2026\n");
  printf("\n");
}

/**
 * @brief Procédure qui affiche un message en cas  d'abandon
 */
void affiche_abandon() {
  system("clear");
  printf(RED "╔══════════════════════════════════════════╗\n");
  printf("║                                          ║\n");
  printf("║" RESET "                 " BLINK BOLD "ABANDON !" RESET NOT_BLINK
         "                " RED "║\n");
  printf("║                                          ║\n");
  printf("╚══════════════════════════════════════════╝\n\n" RESET);
}

/**
 * @brief Procédure qui affiche la liste de tout les niveaux disponibles.
 */
void affiche_niveau() {
  system("clear");
  printf("liste des niveaux : \n");
  system("ls *.sok");
  printf("Entrer un niveau (voir liste): ");
}

// Titouan Moquet 1D2 IUT Lannion 2025-2026