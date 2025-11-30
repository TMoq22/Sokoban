/**
 * @file sokoban.c
 * @brief jeu du Sokoban en C
 * @author Titouan Moquet
 * @version V2.3.6
 * @date 27/11/2025
 *
 * Jeu du Sokoban réalisé en C jouable dans le terminal dans le cadre de la
 * SAE 1.01, IUT Lannion Info 1D2 2025-2026
 *
 * indentation et formatage automatique : clangd
 */

// librairies inclusent
#include <complex.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
// taille du tableau
#define TAILLE 12
#define TAILLE_DEPLACEMENT 5000

// Code d'échappement ANSI (http://www.linuxfocus.org/,
// https://www.codequoi.com/,
// https://en.wikipedia.org/wiki/ANSI_escape_code#In_C)
#define RESET "\033[0m"
#define BOLD "\033[1m"
// #define RED "\033[31m"
#define RED "\033[38;5;196m"
#define GREEN "\033[32m"
#define BRIGHT_GREEN "\033[38;5;118m"
// #define YELLOW "\033[33m"
#define YELLOW "\033[38;5;220m"
#define BLUE "\033[34m"
#define BLUE2 "\033[38;5;12m"
#define CYAN "\033[36m"
#define BRIGHT_MAGENTA "\033[38;5;207m"
#define ORANGE "\033[38;5;202m"
// blink ne marche pas dans tout les terminaux
#define BLINK "\033[5m"
#define NOT_BLINK "\033[25m"
// constante pour le fond
#define BG_WHITE "\033[48;5;239m"

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
// vrai et faux, oui et non
const char YES = 'y';
const char NO = 'n';
const bool VRAI = true;
const bool FAUX = false;

// pour zoom
const char ZOOM = '+';
const char DEZOOM = '-';
const int MAX_ZOOM = 3;
const int MIN_ZOOM = 1;
// pour sauvegarde des déplacements
const char HAUT_SANS_CAISSE = 'h';
const char BAS_SANS_CAISSE = 'b';
const char DROITE_SANS_CAISSE = 'd';
const char GAUCHE_SANS_CAISSE = 'g';
const char HAUT_AVEC_CAISSE = 'H';
const char BAS_AVEC_CAISSE = 'B';
const char DROITE_AVEC_CAISSE = 'D';
const char GAUCHE_AVEC_CAISSE = 'G';
const int AVEC_CAISSE = 2;
const int SANS_CAISSE = 1;
const int PAS_DEPLACEMENT = 0;

const char QUITTER[6] = "exit"; // pour quitter le jeu lors du choix des niveau
const char TOUCHE_NULL = '\0';  // touche 'null'

const char FICHIER_SOK[] = ".sok";
const char FICHIER_DEP[] = ".dep";

// Les prototypes des fonctions et procédures, détaille avant chaque fonction
// et procédures.
int kbhit();
void charger_partie(t_plateau plateau, char fichier[]);
void enregistrer_partie(t_plateau plateau, char fichier[]);
void sauvegarde_jeu(t_plateau plateau);
void sauvegarde_deplacements(t_tabDeplacement tabDeplacement,
                             int nbDeplacement);
void enregistrerDeplacements(t_tabDeplacement t, int nb, char fic[]);
void afficher_entete(int nbDeplacement, char nomNiveau[]);
void affiche_plateau(t_plateau plateau, int niveauZoom);
void affiche_plateau_largeur(t_plateau plateau, int niveauZoom, int longueur);
void affiche_debut();
void affichage_complet(t_plateau plateau, t_tabDeplacement tabDeplacement,
                       char nomNiveau[], int nbDeplacement, int niveauZoom,
                       bool error_pos_jo);
void recherche_pos_jo(t_plateau plateau, int *posJoX, int *posJoY);
void deplacer(t_plateau plateau, t_plateau plateauBase,
              t_tabDeplacement tabDeplacement, char touche, int *posJoX,
              int *posJoY, int *nbDeplacement);
void deplacer_joueur(t_plateau plateau, t_plateau plateauBase, int directionX,
                     int directionY);
void remplace_caractere(t_plateau tableau, t_plateau plateauBase, int posX,
                        int posY);
void annuler_deplacement(t_plateau plateau, t_plateau plateauBase,
                         t_tabDeplacement tabDeplacement, int *posJoX,
                         int *posJoY, int *nbDeplacement);
bool gagne(t_plateau plateau, t_plateau plateauBase);
void affiche_message_fin(int nbDeplacement);
void affiche_abandon();
void affiche_niveau();
void jeu(char touche, t_plateau plateau, t_plateau plateauBase,
         t_tabDeplacement tabDeplacement, int *posJoX, int *posJoY,
         int *nbDeplacement, char nomNiveau[], int *niveauZoom, bool gagner,
         bool error_pos_jo);
void memoriser_deplacement(char touche, t_tabDeplacement tabDeplacement,
                           int leDeplacement, int nbDeplacement);
void affiche_tab_dep(t_tabDeplacement tabDeplacement, int nbDeplacement);
void rejouer(char *touche, char *jouer, bool gagner);
void affiche_erreur();
char get_touche_y_n();

/**
 * @brief Entrée du programme
 * @return EXIT_SUCCESS : arrêt normal du programme
 * Fait appel à toutes les procedures et fonctions necessaires au jeu.
 */
int main() {
  // Declaration des variables
  t_plateau plateau, plateauBase;
  t_tabDeplacement tabDeplacement;
  int posJoX, posJoY, nbDeplacement, niveauZoom = 1, comparaison;
  char nomNiveau[20], touche, jouer = YES;
  // Initialisation des données
  bool gagner = FAUX, error_pos_jo = FAUX;
  touche = TOUCHE_NULL;
  posJoX = 0;
  posJoY = 0;
  system("clear");
  while (jouer != NO) {
    jouer = NO;
    nbDeplacement = 0;
    affiche_debut();
    scanf("%s", nomNiveau);
    comparaison = strcmp(nomNiveau, QUITTER);
    if (comparaison != 0) {
      // deux plateau, 1 pour le jeu et 1 pour la sauvegarde des élèments
      charger_partie(plateau, nomNiveau);
      charger_partie(plateauBase, nomNiveau);
      recherche_pos_jo(plateau, &posJoX, &posJoY);
      if ((posJoX == -1) || (posJoY == -1)) {
        error_pos_jo = VRAI;
        printf(
            RED
            "Erreur sur la position du joueur vérifiez le niveau !\n\n " RESET);
      }
    }
    gagner = gagne(plateau, plateauBase);

    while ((touche != FIN) && (gagner == FAUX) &&
           (nbDeplacement < TAILLE_DEPLACEMENT) && (error_pos_jo == FAUX)) {
      if (kbhit()) {
        touche = getchar();
        jeu(touche, plateau, plateauBase, tabDeplacement, &posJoX, &posJoY,
            &nbDeplacement, nomNiveau, &niveauZoom, gagner, error_pos_jo);
        gagner = gagne(plateau, plateauBase);
        affichage_complet(plateau, tabDeplacement, nomNiveau, nbDeplacement,
                          niveauZoom, error_pos_jo);
      }
      if (touche == FIN) {
        sauvegarde_jeu(plateau);
      }
    }
    if (nbDeplacement >= TAILLE_DEPLACEMENT) {
      affiche_erreur();
    }
    if (gagner == VRAI) {
      system("clear");
      affiche_plateau(plateau, niveauZoom);
      affiche_message_fin(nbDeplacement);
    }
    if ((comparaison != 0) && (error_pos_jo == FAUX)) {
      sauvegarde_deplacements(tabDeplacement, nbDeplacement);
    }
    rejouer(&touche, &jouer, gagner);
    gagner = FAUX;
  }
  printf(BOLD YELLOW "\n\nAu revoir !\n" RESET);
  return EXIT_SUCCESS;
}

/**
 * @brief procedure pour confirmer si le joueur veux rejouer ou non
 * @param touche, entrer sortie
 * @param jouer, caractere 'y' ou 'n'  entrer sortie
 * @param gagner booléen vrai ou faux si joueur à gagner ou non
 */
void rejouer(char *touche, char *jouer, bool gagner) {
  if ((*touche == FIN) || (gagner == VRAI)) {
    printf(BRIGHT_MAGENTA "\nVoulez vous rejouer ?(y,n) : " RESET);
    *touche = get_touche_y_n();
    *jouer = *touche;
    // scanf(" %c", &(*jouer));
    *touche = TOUCHE_NULL;
  }
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
         int *nbDeplacement, char nomNiveau[], int *niveauZoom, bool gagner,
         bool error_pos_jo) {

  deplacer(plateau, plateauBase, tabDeplacement, touche, &(*posJoX), &(*posJoY),
           &(*nbDeplacement));
  if (touche == RELOAD) {
    charger_partie(plateau, nomNiveau);
    recherche_pos_jo(plateau, &(*posJoX), &(*posJoY));
    *nbDeplacement = 0;
    affichage_complet(plateau, tabDeplacement, nomNiveau, *nbDeplacement,
                      *niveauZoom, error_pos_jo);
    error_pos_jo = FAUX;
  }
  if (touche == UNDO) {
    annuler_deplacement(plateau, plateauBase, tabDeplacement, &(*posJoX),
                        &(*posJoY), &(*nbDeplacement));
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
 * @brief Procédure pour l'enregistrement du tableau des déplacemement
 * @param t t_tabDeplacement
 * @param nb int , nb déplacements
 * @param fic string , nom du fichier
 */
void enregistrerDeplacements(t_tabDeplacement t, int nb, char fic[]) {
  FILE *f;

  f = fopen(fic, "w");
  fwrite(t, sizeof(char), nb, f);
  fclose(f);
}

/**
 * @brief Focntion qui renvoie soit y si la touche y est préssé ou n si la
 * touche n est préssé
 * @return toucheS le caractère de la touche
 * */
char get_touche_y_n() {
  char toucheS = TOUCHE_NULL;
  while ((toucheS != NO) && (toucheS != YES)) {
    if (kbhit()) {
      toucheS = getchar();
    }
  }
  return toucheS;
}

/**
 * @brief Procédure pour le choix en fin de partie (sauvegarde ou non), utilise
 * la procédure.
 * @param plateau de type t_plateau pour que la procedure "enregistrer_partie"
 * puisse avoir le plateau.
 */
void sauvegarde_jeu(t_plateau plateau) {
  char nomSauvegarde[40];
  char toucheS = TOUCHE_NULL; // carac vide
  affiche_abandon();
  printf(BOLD YELLOW "\nVoulez vous sauvegarder ? (y,n) : " RESET);
  toucheS = get_touche_y_n();
  if (toucheS == YES) {
    printf(BOLD YELLOW
           "\nnom du fichier (30 caractères max) sans extention : " RESET);
    scanf("%s", nomSauvegarde);
    strcat(nomSauvegarde, FICHIER_SOK);
    enregistrer_partie(plateau, nomSauvegarde);
    printf(ORANGE "Partie sauvegardé dans le fichier : %s !\n" RESET,
           nomSauvegarde);
  }
}
/**
 * @brief Procédure pour le  déplacement du personnage et des caisses.
 * @param tabDeplacement type t_tabDeplacement, tableau des déplacements
 * @param nbDeplacement de type int
 *
 */
void sauvegarde_deplacements(t_tabDeplacement tabDeplacement,
                             int nbDeplacement) {
  char nomSauvegarde[40];
  char toucheS = TOUCHE_NULL; // carac vide
  printf(BOLD YELLOW "\nVoulez vous sauvegarder la liste de vos déplacements ? "
                     "(y,n) : " RESET);
  toucheS = get_touche_y_n();
  if (toucheS == YES) {
    printf(BOLD YELLOW
           "\nnom du fichier (30 caractères max) sans extention : " RESET);
    scanf("%s", nomSauvegarde);
    strcat(nomSauvegarde, FICHIER_DEP);
    enregistrerDeplacements(tabDeplacement, nbDeplacement, nomSauvegarde);
    printf(ORANGE "Déplacements sauvegardé dans le fichier : %s !\n" RESET,
           nomSauvegarde);
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
  int directionX = 0, directionY = 0;
  int x = *posJoX, y = *posJoY;
  int leDeplacement = PAS_DEPLACEMENT;
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
      leDeplacement = AVEC_CAISSE;
    } else {
      return; // bloqué
    }
  }
  // remplacement du caractères ou etait le joueur
  remplace_caractere(plateau, plateauBase, *posJoX, *posJoY);
  // deplacement joueur
  deplacer_joueur(plateau, plateauBase, x + directionX, y + directionY);
  // si leDeplacement = et que on arrive ici alors leDeplacement = SANS_CAISSE
  memoriser_deplacement(touche, tabDeplacement, leDeplacement, *nbDeplacement);
  *posJoX += directionX; // actualisation des coordonnées
  *posJoY += directionY;
  // comptage des déplacements
  if ((touche == HAUT) || (touche == BAS) || (touche == GAUCHE) ||
      (touche == DROITE)) {
    *nbDeplacement += 1;
  }
}

void deplacer_joueur(t_plateau plateau, t_plateau plateauBase, int directionX,
                     int directionY) {
  if (plateauBase[directionX][directionY] == CIBLE ||
      plateauBase[directionX][directionY] == OBJECTIF) {
    plateau[directionX][directionY] = JOUEUR_SUR_CIBLE;
  } else {
    plateau[directionX][directionY] = JOUEUR;
  }
}

/**
 * @brief procedure qui enregistre les déplacements du joueur.
 * @param touche char, touche préssé par le joueur.
 * @param tabDeplacement t_tabDeplacement, tableau des déplacements.
 * @param leDeplacement int, permet de savoir si le déplacements c'est fait avec
 * une caisse ou non
 * @param nbDeplacement int, pour l'insertion des déplacements
 */
void memoriser_deplacement(char touche, t_tabDeplacement tabDeplacement,
                           int leDeplacement, int nbDeplacement) {
  char caracDeplacement = TOUCHE_NULL;
  if (leDeplacement == PAS_DEPLACEMENT) {
    leDeplacement = SANS_CAISSE;
  }

  if (touche == HAUT) {
    caracDeplacement = HAUT_SANS_CAISSE;
  } else if (touche == BAS) {
    caracDeplacement = BAS_SANS_CAISSE;
  } else if (touche == GAUCHE) {
    caracDeplacement = GAUCHE_SANS_CAISSE;
  } else if (touche == DROITE) {
    caracDeplacement = DROITE_SANS_CAISSE;
  }
  if (leDeplacement == AVEC_CAISSE) {
    caracDeplacement -= 32;
    // changement des minuscules en majuscules (tables ascii)
  }
  if (nbDeplacement < TAILLE_DEPLACEMENT) {
    tabDeplacement[nbDeplacement] = caracDeplacement;
  } else {
    printf(RED "erreur trop de déplacement ! " RESET);
  }
}

/**
 * @brief Procédure qui permet d'annuler un déplacement
 * @param plateau de type t_plateau, tableau de jeu.
 * @param plateauBase de type t_plateau, tableau de sauvegarde des
 * déplacements
 * @param posJoX de type entier, sur quel ligne du tableau est le joueur,
 * en entrer et sortie
 * @param posJoY de type entier; sur quel colonne du tableau est le joueur,
 * en entrer et sortie
 * @param nbDeplacement int, nombre de déplacements du joueur
 */
void annuler_deplacement(t_plateau plateau, t_plateau plateauBase,
                         t_tabDeplacement tabDeplacement, int *posJoX,
                         int *posJoY, int *nbDeplacement) {
  char dep = tabDeplacement[*nbDeplacement - 1];
  int dx = 0, dy = 0;
  if (*nbDeplacement > 0) {
    if (dep == HAUT_SANS_CAISSE || dep == HAUT_AVEC_CAISSE)
      dx = 1;
    else if (dep == BAS_SANS_CAISSE || dep == BAS_AVEC_CAISSE)
      dx = -1;
    else if (dep == GAUCHE_SANS_CAISSE || dep == GAUCHE_AVEC_CAISSE)
      dy = 1;
    else if (dep == DROITE_SANS_CAISSE || dep == DROITE_AVEC_CAISSE)
      dy = -1;
    int avecCaisse = (dep == HAUT_AVEC_CAISSE || dep == BAS_AVEC_CAISSE ||
                      dep == GAUCHE_AVEC_CAISSE || dep == DROITE_AVEC_CAISSE);
    remplace_caractere(plateau, plateauBase, *posJoX, *posJoY);
    if (avecCaisse) {
      if ((plateauBase[*posJoX][*posJoY] == CIBLE) ||
          (plateauBase[*posJoX][*posJoY] == OBJECTIF)) {
        plateau[*posJoX][*posJoY] = OBJECTIF;
      } else {
        plateau[*posJoX][*posJoY] = CAISSE;
      }
      remplace_caractere(plateau, plateauBase, *posJoX - dx, *posJoY - dy);
    }
    *posJoX += dx;
    *posJoY += dy;
    deplacer_joueur(plateau, plateauBase, *posJoX, *posJoY);
    (*nbDeplacement) -= 1;
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
  for (int longueur = 0; longueur < TAILLE; longueur++) {
    for (int largeur = 0; largeur < TAILLE; largeur++) {
      if ((plateau[longueur][largeur] == JOUEUR) ||
          (plateau[longueur][largeur] == JOUEUR_SUR_CIBLE)) {
        *posJoX = longueur;
        *posJoY = largeur;
        // return;
      }
    }
  }
  if ((*posJoX == 0) || (*posJoY == 0)) {
    *posJoX = -1;
    *posJoY = -1;
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
void remplace_caractere(t_plateau plateau, t_plateau plateauBase, int posX,
                        int posY) {
  // remplace le caractère à cette position par un point si il y a un point à
  // la base

  if (plateauBase[posX][posY] == CIBLE) {
    plateau[posX][posY] = CIBLE;
  } else if (plateauBase[posX][posY] == OBJECTIF) {
    // remplace '*' par un point
    plateau[posX][posY] = CIBLE;
  } else if (plateauBase[posX][posY] == JOUEUR_SUR_CIBLE) {
    // remplacement de '+' par '.' lors du déplacement
    plateau[posX][posY] = CIBLE;
  } else {
    // remplace le joueurs par un espace apres déplacement
    plateau[posX][posY] = VIDE;
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
  printf(BOLD CYAN " Commandes :" RESET GREEN "%31s\n" RESET, nomNiveau);
  printf(CYAN "────────────────────────────────────────────\n" RESET);
  printf(BRIGHT_GREEN "   z : haut        ↑  ");
  printf("   s : bas         ↓\n");
  printf("   q : gauche      ←  ");
  printf("   d : droite      →\n");
  printf("   + : zoom           ");
  printf("   - : dezoom       \n" RESET);
  printf("\n");
  printf(YELLOW "   u : annuler le dernier déplacement\n" RESET);
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
 * remplace certains caratère, par d'autre à l'affichage : ex '+' -> '@'.
 * @param plateau de type t_plateau, tableau de jeu.
 * @param niveauZoom int, niveau du zoom
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

/**
 * @brief Procédure qui affiche le plateau de jeu,
 * remplace certains caratère par d'autre à l'affichage : ex '+' -> '@'.
 * @param plateau de type t_plateau, tableau de jeu.
 * @param niveauZoom int, niveau du zoom
 */
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
        printf(BRIGHT_MAGENTA "%c" RESET, JOUEUR);
      } else if (plateau[longueur][largeur] == CAISSE) {
        printf(GREEN "%c" RESET, CAISSE);
      } else if (plateau[longueur][largeur] == BORD) {
        printf(BG_WHITE "%c" RESET, BORD);
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
void affichage_complet(t_plateau plateau, t_tabDeplacement tabDeplacement,
                       char nomNiveau[], int nbDeplacement, int niveauZoom,
                       bool error_pos_jo) {
  system("clear");

  afficher_entete(nbDeplacement, nomNiveau);
  affiche_plateau(plateau, niveauZoom);
  // affiche_tab_dep(tabDeplacement, nbDeplacement);
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
  printf(" © Titouan Moquet 1D2 IUT Lannion 2025-2026\n");
  printf("\n");
}

/**
 * @brief Procédure qui affiche un message en cas d'abandon
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
 * @brief Procédure qui affiche un message au début du jeu puis la liste des
 * niveaux
 */
void affiche_debut() {
  system("clear");
  printf(BLUE "╔══════════════════════════════════════════╗\n");
  printf("║                                          ║\n");
  printf("║" RESET "                " BOLD " SOKOBAN " RESET
         "                 " BLUE "║\n" RESET);
  printf(BLUE "║                                          ║\n");
  printf("╚══════════════════════════════════════════╝\n" RESET);
  printf(GREEN "\n © Titouan Moquet 1D2 IUT Lannion 2025-2026\n" RESET);
  printf(CYAN "\n────────────────────────────────────────────\n" RESET);

  affiche_niveau();
}

/**
 * @brief Procédure qui affiche la liste de tout les niveaux disponibles.
 */
void affiche_niveau() {
  printf(CYAN "liste des niveaux : \n" RESET);
  // j'ai remplacer le 'system("ls *.sok");' car je ne pouvais pas mettre de
  // la couleur comme je le voulais
  char cmd[256];
  snprintf(cmd, sizeof(cmd),
           "bash -c 'printf \"%s\"; ls --color=never *.sok; printf \"%s\"'",
           BRIGHT_MAGENTA,
           RESET); // sizeof permet à snprintf() de ne jamais dépasser le
                   // tableau et d’éviter un overflow.
  system(cmd);
  printf(RED "\n('exit' pour quitter)\n" RESET);
  printf(YELLOW "Entrer un niveau : " RESET);
}

/**
 * @brief Procédure qui affiche le tableau de déplacement.
 */
void affiche_tab_dep(t_tabDeplacement tabDeplacement, int nbDeplacement) {
  for (int i = 0; i < nbDeplacement; i++) {
    printf("\033[38;5;250m %c " RESET, tabDeplacement[i]);
  }
  printf("\n");
}
/**
 * @brief Procédure qui affiche un message d'erreur quand on dépasse le nb de
 * déplacement max
 */
void affiche_erreur() {
  system("clear");
  printf(RED "nombre de déplacement trop grand : fatal error \n" RESET);
}

// Copyright (c) 2025 Titouan Moquet
// MIT License
// 1D2 IUT Lannion 2025-2026