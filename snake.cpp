// Projet Snake Yakub ONAL

#include <fcntl.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

using namespace std;


// Constantes définissant la taille du terrain de jeu et de la queue
#define SIZE_MAXI 200
#define LARGEUR 60
#define HAUTEUR 30


// Structure représentant un fruit sur le terrain
struct fruit {
    int fruitX;
    int fruitY;
};


// Enumération des directions possibles pour le serpent
enum direction {
    STOP = 0,
    GAUCHE,
    DROITE,
    HAUT,
    BAS
};


// Structure représentant l'état du jeu
struct jeu {
    bool jeuEnCours;
    int teteX;
    int teteY;
    int score;
    int tailleQueue;
    int temps;
    direction d;
    fruit f;
};


// Déclaration des fonctions
int menu();
jeu initialiser(jeu j, int queueX[], int queueY[]);
int dessiner(jeu j, int tabX[], int tabY[]);
direction entrerDirection(direction d);
jeu deplacer(jeu j, int queueX[], int queueY[], int niveau);
fruit genererFruit(jeu j, int queueX[], int queueY[]);
int userInput();


int main() {
    int niveau = menu();

    // Initialisation générateur nombre aléatoire
    srand(time(NULL));

    jeu j;
    int queueX[SIZE_MAXI], queueY[SIZE_MAXI];

    // Initialisation du jeu
    j = initialiser(j, queueX, queueY);

    // Boucle principale du jeu
    while (j.jeuEnCours) {
        dessiner(j, queueX, queueY);
        if (userInput()) {
            j.d = entrerDirection(j.d);
        }
        j = deplacer(j, queueX, queueY, niveau);
        usleep(j.temps);
    }
    cout << "Game Over! Votre score est: " << j.score << endl;

    return 0;
}


// Initialisation des variables de la struct jeu
jeu initialiser(jeu j, int queueX[], int queueY[]) {
    j.jeuEnCours = true;
    j.teteX = LARGEUR / 2;
    j.teteY = HAUTEUR / 2;
    j.score = 0;
    j.tailleQueue = 0;
    j.temps = 200000;
    j.d = STOP;
    j.f = genererFruit(j, queueX, queueY);
    return j;
}


// Dessine le terrain de jeu en fonction du niveau
int dessiner(jeu je, int tabX[], int tabY[]) {
    system("clear");
    int i, j, k;
    bool vide;

    // Dessine le cadre horizontal supérieur
    for (i = 0; i < LARGEUR; ++i) {
        cout << "█";
    }
    cout << endl;

    // Dessine le cadre vertical gauche et droit avec des espaces à l'intérieur et snake
    for (i = 0; i < HAUTEUR; ++i) {
        for (j = 0; j < LARGEUR; ++j) {
            vide = true;
            if (j == 0 || j == LARGEUR - 1) {
                cout << "█";
                vide = false;
            }
            if (j == je.teteX && i == je.teteY) {
                // Dessine la tête du serpent
                cout << "O";
                vide = false;
            } else if (j == je.f.fruitX && i == je.f.fruitY) {
                // Dessine le fruit
                cout << "$";
                vide = false;
            } else {
                // Dessine la queue du serpent si on est sur une case du serpent
                for (k = 0; k < je.tailleQueue; k++) {
                    if (tabX[k] == j && tabY[k] == i) {
                        cout << "o";
                        vide = false;
                    }
                }
            }

            // Dessine un espace si la case est vide
            if (vide) {
                cout << " ";
            }
        }

        cout << endl;
    }

    // Dessine le cadre horizontal inférieur
    for (i = 0; i < LARGEUR; ++i) {
        cout << "█";
    }
    cout << endl;

    // Dessine le score
    cout << "                         SCORE : " << je.score << " $" << endl;

    return 0;
}


// Attend une entrée de l'utilisateur et met à jour la direction du serpent
direction entrerDirection(direction d) {
    system("stty raw");

    char caractere;
    caractere = getchar();
    if ((caractere == 'Z' || caractere == 'z') && (d != BAS)) {
        d = HAUT;
    }
    if ((caractere == 'S' || caractere == 's') && (d != HAUT)) {
        d = BAS;
    }
    if ((caractere == 'Q' || caractere == 'q') && (d != DROITE)) {
        d = GAUCHE;
    }
    if ((caractere == 'D' || caractere == 'd') && (d != GAUCHE)) {
        d = DROITE;
    }
    if (caractere == 'P' || caractere == 'p') {
        system("stty sane");
        exit(0);
    }
    system("stty sane");
    return d;
}


// Met à jour la position du serpent et vérifie les collisions
jeu deplacer(jeu j, int queueX[], int queueY[], int niveau) {
    int i;
    int ancienTeteX;
    int ancienTeteY;
    int ancienQueueX;
    int ancienQueueY;
    int tempX;
    int tempY;

    ancienTeteX = j.teteX;
    ancienTeteY = j.teteY;

    // Déplace la tête du serpent en fonction de la direction
    if (j.d == HAUT) {
        j.teteY--;
    } else if (j.d == BAS) {
        j.teteY++;
    } else if (j.d == GAUCHE) {
        j.teteX--;
    } else if (j.d == DROITE) {
        j.teteX++;
    }

    // Vérifie les collisions avec les bords du terrain
    if (j.teteX <= 0 || j.teteX >= LARGEUR - 1 || j.teteY < 0 || j.teteY >= HAUTEUR) {
        j.jeuEnCours = false;
    }

    // Vérifier les collisions avec la queue (Game Over si collision)
    for (i = 0; i < j.tailleQueue; i++) {
        if (queueX[i] == j.teteX && queueY[i] == j.teteY) {
            j.jeuEnCours = false;
        }
    }

    // Mettre à jour la queue du serpent
    ancienQueueX = queueX[0];
    ancienQueueY = queueY[0];
    queueX[0] = ancienTeteX;
    queueY[0] = ancienTeteY;

    for (i = 1; i < j.tailleQueue; i++) {
        tempX = queueX[i];
        tempY = queueY[i];
        queueX[i] = ancienQueueX;
        queueY[i] = ancienQueueY;
        ancienQueueX = tempX;
        ancienQueueY = tempY;
    }

    // Vérifie les collisions avec le fruit et met à jour le score et la taille du serpent
    if (j.teteX == j.f.fruitX && j.teteY == j.f.fruitY) {
        j.score++;
        j.f = genererFruit(j, queueX, queueY);
        j.tailleQueue++;
        if (niveau == 2) {
            // Ajuste la vitesse en fonction du score
            if (j.score > 40) {
                j.temps = 40000;
            } else if (j.score > 30) {
                j.temps = 60000;
            } else if (j.score > 20) {
                j.temps = 100000;
            } else if (j.score > 10) {
                j.temps = 150000;
            }
        }
    }
    return j;
}


// Génère un fruit sur le terrain de jeu, en évitant les collisions avec le serpent
fruit genererFruit(jeu j, int queueX[], int queueY[]) {
    bool b;
    int l;
    fruit fr;
    do {
        b = false;
        fr.fruitX = (rand() % (LARGEUR - 2)) + 1;
        fr.fruitY = (rand() % (HAUTEUR - 2)) + 1;
        for (l = 0; l < j.tailleQueue; l++) {
            if (queueX[l] == fr.fruitX && queueY[l] == fr.fruitY) {
                b = true;
            }
        }
    } while (j.teteX == fr.fruitX && j.teteY == fr.fruitY || b);
    return fr;
}


// Affichage du menu et récupération du choix du niveau
int menu() {
    char niveau;

    do {
        cout << endl;
        cout << "    SSSS  N   N    A    K   K   EEEE" << endl;
        cout << "   S      NN  N   A A   K  K    E   " << endl;
        cout << "    SSS   N N N  A A A  KKK     EEEE" << endl;
        cout << "       S  N  NN  A   A  K  K    E   " << endl;
        cout << "   SSSS   N   N  A   A  K   K   EEEE" << endl;
        cout << endl;
        cout << "         Bienvenue sur Snake 1" << endl;
        cout << "          -- Niveau 1 (1) --" << endl;
        cout << "          -- Niveau 2 (2) --" << endl;
        cout << endl;
        cout << "         Je veux le niveau : ";
        cin >> niveau;
        cin.clear();
        cin.ignore(100, '\n');

    } while (!(niveau >= 49 && niveau <= 52));
    cout << niveau - 48;

    return (int)niveau - 48;
}


// Fonction qui vérifie si une touche est pressée sans bloquer le programme
// http://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
int userInput() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}