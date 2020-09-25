#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#define LMAX 50 //taille map max
#define CMAX 50 //taille map max
#define LMIN 10 //taille map mini
#define CMIN 10 //taille map mini
#define AMIN 2 //animaux mini
#define AMAX 20 //animaux max
#define WMIN 1 //ressources mini
#define WMAX 10 //ressources max
#define BMAX 10 //bush max
#define BMIN 1 //bush min
#define DMAX 1000 //jours max
#define DMIN 10 //jours mini
#define SOL 32 //sol
#define ANIMAL 2
#define WATER 42
#define BUSH 35
#define ERREUR 33
#define TIRET 45 // ligne de la map
#define NBDEPLACEMENT 2 // nb de deplacement autorise

struct Animal
{
    int thirst; //soif de l'animal 3j survie
    int hunger; //faim de l'animal 10j survie
    int coord_x;//abscisse de l'animal
    int coord_y;//ordonnee de l'animal
};
typedef struct Animal sAnimal;

struct Water
{
    int quantity; //point de vie du pt d'eau - faire en sorte qu'elle disparaisse quand elle atteint 0
    int coord_x;//abscisse de l'eau
    int coord_y;//ordonnee de l'eau
};
typedef struct Water sWater;

struct Bush
{
    int quantity; //point de vie du bush - faire en sorte qu'elle disparaisse quand elle atteint 0
    int coord_x;//abscisse du buisson
    int coord_y;//ordonnee du buisson
};
typedef struct Bush sBush;

struct caseMap
{
    int pdv;    //nombre d'entamation de la ressource
    int typeCase; //0: sol ; 1 : animal; 2 : water; 3 : bush
    int statutCase; //0 : vide; 1 : occupee
};
typedef struct caseMap sCaseMap;

void starting(); //Configuration des parametres de depart
int days_numbers(); //demande de nombre de jours
void environment_generation(); //Generation de l'environnement
void color();
void initialisation();
void animal_init();
void water_init();
void bush_init();
void deplacement();
void affichage_init();
void test();

int main()
{
    srand(time(NULL));  //fonction pour faire en sorte que le rand soit toujours aleatoire

    int d, l, c, animal, water, bush, animal_alive;

    starting(&l, &c, &animal, &water, &bush);
    sAnimal a[AMAX];
    sWater w[WMAX];
    sBush b[BMAX];
    sCaseMap map[LMAX][CMAX];

    d = days_numbers();
    animal_alive = 0;

    animal_alive = animal;
    initialisation(&l, &c, map);
    animal_init(&l, &c, &animal, map, a);
    water_init(&l, &c, &water, map, w);
    bush_init(&l, &c ,&bush, map, b);
    affichage_init(&l, &c, &animal, &water, &bush, map, a, w, b);
    deplacement(&l, &c, &animal, &water, &bush, map, a, w, b);
    affichage_init(&l, &c, &animal, &water, &bush, map, a, w, b);
    //test(&l, &c, map, &animal_alive);

    /*while(animal_alive != 0)
    {
        affichage(&l, &c, &animal, &water, &bush, map, a, w, b);
        test(map, &animal_alive);
    }*/
    return 0;
}

void delay(int s)   //temporisation de la map
{
    clock_t start,end;
    start = clock();
    while(((end = clock())- start) <= s*CLOCKS_PER_SEC);
}


void color(int textcolor, int backgroundcolor)
{
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, backgroundcolor*16+textcolor);

}

void starting(int *l, int *c, int *animal, int *water, int *bush)
{
    FILE *f_donnees;
    f_donnees = fopen("Donnees.txt", "w");  //ouverture du fichier en ecriture

    if (f_donnees == NULL)
    {
        printf ("Erreur lors de l'ouverture du fichier");
    }

    printf("Determinez le nombre de lignes et le nombre de colonnes que vous souhaitez pour votre environnement (Compris entre 10 et 50 respectivement :\n");

    printf("\nSaisissez le nombre de lignes : ");
    scanf("%d", &(*l));

    printf("Saisissez le nombre de colonnes : ");
    scanf("%d", &(*c));

    while((*l>LMAX) || (*l<LMIN))
    {
    printf("\nVeuillez saisir des nombres compris entre %d et %d : ", LMIN, LMAX);
    printf("\nLignes = ");
    scanf("%d", &(*l));
    }
    while((*c>CMAX) || (*c<CMIN))
    {
        printf("\nVeuillez saisir des nombres compris entre %d et %d : ", CMIN, CMAX);
        printf("Colonnes : ");
        scanf("%d", &(*c));
    }
    fprintf(f_donnees,"%d\n", *c);
    fprintf(f_donnees,"%d\n", *l);

    printf("\nSaisissez le nombre d'animaux que vous souhaitez (Compris entre %d et %d) : ", AMIN, AMAX);
    scanf("%d", &(*animal));

    while((*animal<AMIN) || (*animal>AMAX))
    {
        printf("Saisissez un nombre valide entre %d et %d : ", AMIN, AMAX);
        scanf("%d", &(*animal));
    }
    fprintf(f_donnees, "%d\n", *animal);

    printf("\nSaisissez le nombre de point(s) d'eau souhaite(s) (Compris entre %d et %d) : ", WMIN, WMAX);
    scanf("%d", &(*water));

    while((*water<WMIN) || (*water>WMAX))
    {
        printf("Saisissez un nombre valide entre %d et %d: ", WMIN, WMAX);
        scanf("%d", &(*water));
    }
    fprintf(f_donnees, "%d\n", *water);

    printf("\nSaisissez le nombre de buisson(s) souhaite(s) (Compris entre %d et %d) : ", BMIN, BMAX);
    scanf("%d", &(*bush));

    while((*bush<WMIN) || (*bush>WMAX))
    {
        printf("Saisissez un nombre valide entre %d et %d: ", BMIN, BMAX);
        scanf("%d", &(*bush));
    }
    fprintf(f_donnees, "%d\n", *bush);

    printf("\n\n");
    printf("Veuillez prendre en compte que les animaux se deplacent de haut en bas, de droite a gauche, et en diagonale");

    fclose(f_donnees);
}

int days_numbers()
{
    int day;

    printf("\n\nSaisissez le nombre de jours que vous souhaitez observer (au moins %d jours) : ", DMIN);
    scanf("%d", &day);

    while((day<DMIN) || (day>DMAX))
    {
        printf("Saisissez un nombre compris entre 10 et 1000 : ");
        scanf("%d", &day);
    }

    printf("Veuillez patienter un instant s'il vous plait. La map est en cours de chargement...\n\n");

    return(day);
}

void initialisation(int *l, int *c, sCaseMap map[LMAX][CMAX])
{
    int i, j;

    for(i=0; i<*l; i++)// on boucle for pour initialiser la map
    {
        for(j=0;j<*c; j++)
        {
            map[i][j].statutCase = 0;
            map[i][j].typeCase = 0;
            map[i][j].pdv = 0;
        }
        printf("\n");
    }
}

void  animal_init(int *l, int *c, int *animal, sCaseMap map[LMAX][CMAX], sAnimal a[AMAX])
{
    int i;

    for(i=0; i<*animal; i++) //ajout des animaux a la demande de l'utilsateur
    {
        a[i].thirst = 3;
        a[i].hunger = 10;
        a[i].coord_x = rand()%(*l); //placement aleatoire de l'animal
        a[i].coord_y = rand()%(*c);

        while(map[a[i].coord_x][a[i].coord_y].statutCase == 1)  //si l'animal est genere sur une case impassable
        {
            a[i].coord_x = rand()%(*l);
            a[i].coord_y = rand()%(*c);
        }
        map[a[i].coord_x][a[i].coord_y].statutCase = 1;
        map[a[i].coord_x][a[i].coord_y].typeCase = 1;
        map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
    }
}

void water_init(int *l, int *c, int *water, sCaseMap map[LMAX][CMAX], sWater w[WMAX])
{
    int i;

    for(i=0; i<*water; i++) //ajout des points d'eau a la demande de l'utilisateur
    {
        w[i].quantity = 4;
        w[i].coord_x = rand()%(*l);
        w[i].coord_y = rand()%(*c);

        while(map[w[i].coord_x][w[i].coord_y].statutCase == 1)    //si l'eau est genere sur une case impassable
        {
            w[i].coord_x = rand()%(*l);
            w[i].coord_y = rand()%(*c);
        }
        map[w[i].coord_x][w[i].coord_y].statutCase = 1;
        map[w[i].coord_x][w[i].coord_y].typeCase = 2;
        map[w[i].coord_x][w[i].coord_y].pdv = 3;    //peut se faire manger 3 fois avant de disparaitre
    }
}

 void bush_init(int *l, int *c, int *bush, sCaseMap map[LMAX][CMAX], sBush b[BMAX])
{
    int i;

    for(i=0; i<*bush; i++) //ajout des points d'eau a la demande de l'utilisateur
    {
        b[i].quantity = 4;
        b[i].coord_x = rand()%(*l);
        b[i].coord_y = rand()%(*c);

        while(map[b[i].coord_x][b[i].coord_y].statutCase == 1)  //si le buisson est genere sur une case impassable
        {
            b[i].coord_x = rand()%(*l);
            b[i].coord_y = rand()%(*c);
        }
        map[b[i].coord_x][b[i].coord_y].statutCase = 1;
        map[b[i].coord_x][b[i].coord_y].typeCase = 3;
        map[b[i].coord_x][b[i].coord_y].pdv = 5;
    }
}


void affichage_init(int *l, int *c, int *animal, int *water, int *bush, sCaseMap map[LMAX][CMAX],sAnimal a[AMAX], sWater w[WMAX], sBush b[BMAX])
{
    int p, q, i, j;

    for(q=0; q<=(*c*3); q++)
    {
        printf("%c", TIRET);
    }
    printf("\n");

    for (i=0; i<*l; i++)
    {
        printf("|");
        for (j=0; j<*c; j++)
        {
            switch(map[i][j].typeCase)
            {
                case 0:
                    color(0,0);
                    printf("%c ", SOL);
                    color(15,0);
                    printf("|");
                    break;
                case 1:
                    color(12,0);
                    printf("%c ", ANIMAL);
                    color(15,0);
                    printf("|");
                    break;
                case 2:
                    color(15,9);
                    printf("%c ", WATER);
                    color(15,0);
                    printf("|");
                    break;
                case 3:
                    color(6,0);
                    printf("%c ", BUSH);
                    color(15,0);
                    printf("|");
                    break;
                default:
                    color(15,0);
                    printf("%c ", ERREUR);
                    color(15,0);
                    printf("|");
                    break;
            }
        }

        printf("\n");
        for(p=0; p<=(*c*3); p++)
        {
            printf("%c", TIRET);
        }
        printf("\n");
    }

    printf("\nDescription :\n");
    printf("%c : sol \n", SOL);
    printf("%c : animaux ----- %d animaux demande(s)\n", ANIMAL, *animal);
    printf("%c : point(s) d'eau ----- %d point(s) d'eau demande(s)\n", WATER, *water);
    printf("%c : buisson(s) ----- %d buisson(s) demande(s)\n", BUSH, *bush);
    printf("%c : erreur \n", ERREUR);

    printf("\n");
    printf("\n");
}

 void deplacement(int *l, int *c, int *animal, int *water, int *bush, sCaseMap map[LMAX][CMAX],sAnimal a[AMAX], sWater w[WMAX], sBush b[BMAX])
{
    int check, s, i, m, n; // declaration de check pour 2 deplacements, i pour boulcer sur l'animal, s pour le switch et m et n pour le checking

    for(i=0; i<*animal; i++) //deplacement de chaque animal
    {
        for(check=0; check<NBDEPLACEMENT; check++) //variable qui incremente pour arriver au nb de deplacement
        {
            s = rand()%8;   // on considere toutes les cases sauf celui du milieu donc 8

            //� revoir la condition d'en dessous peut etre en dehors de la boucle
            if(a[i].hunger == 0 || a[i].thirst == 0)   //animal mort de faim ou de soif
            {
                map[a[i].coord_x][a[i].coord_y].statutCase = 0; //animal disparait et devient passable
                map[a[i].coord_x][a[i].coord_y].typeCase = 0;   //animal disparait et devient sol
            }
            else
            {
                if(a[i].coord_x == 0) //si animal tout en haut
                {
                    s = rand()%(8-3)+3; //genere que les cases 3/4/5/6/7
                }

                if(a[i].coord_x == (*l-1)) //si animal tout en bas
                {
                    s = rand()%5;   //genere que les cases 0/1/2/3/4
                }

                if(a[i].coord_y == 0)    //si animal tout � gauche
                {
                    s = rand()%5;   //genere que les cases 1/2/4/6/7
                    switch(s)
                    {
                    case 0:
                        s = 1;
                        break;
                    case 1:
                        s = 2;
                        break;
                    case 2:
                        s = 4;
                        break;
                    case 3:
                        s = 6;
                        break;
                    case 4:
                        s = 7;
                        break;
                    }
                }

                if(a[i].coord_y == (*c-1))   //si animal tout � droite
                {
                    s = rand()%5;  //genere que les cases 0/1/3/5/6
                    switch(s)
                    {
                    case 0:
                        s = 0;
                        break;
                    case 1:
                        s = 1;
                        break;
                    case 2:
                        s = 3;
                        break;
                    case 3:
                        s = 5;
                        break;
                    case 4:
                        s = 6;
                        break;
                    }
                }

                if(a[i].coord_x == 0 && a[i].coord_y == 0)  //si animal est dans le coin en haut � gauche
                {
                    s = rand()%3; //genere que les cases 4/6/7
                    switch(s)
                    {
                    case 0:
                        s = 4;
                        break;
                    case 1:
                        s = 6;
                        break;
                    case 2:
                        s = 7;
                        break;
                    }
                }

                if(a[i].coord_x == 0 && a[i].coord_y == (*c-1)) //si animal est dans le coin en haut � droite
                {
                    s = rand()%3;   //genere les cases 3/5/6
                    switch(s)
                    {
                    case 0:
                        s = 3;
                        break;
                    case 1:
                        s = 5;
                        break;
                    case 2:
                        s = 6;
                        break;
                    }
                }

                if(a[i].coord_x == (*l-1) && a[i].coord_y == 0)   //si animal est dans le coin en bas � gauche
                {
                    s = rand()%3;   //genere les cases 1/2/4
                    switch(s)
                    {
                    case 0:
                        s = 1;
                        break;
                    case 1:
                        s = 2;
                        break;
                    case 2:
                        s = 4;
                        break;
                    }
                }

                if(a[i].coord_x == (*l-1) && a[i].coord_y == (*c-1)) //si animal est dans le coin en bas � droite
                {
                    s = rand()%3; //genere les cases 0/1/3
                    switch(s)
                    {
                    case 0:
                        s = 0;
                        break;
                    case 1 :
                        s = 1;
                        break;
                    case 2:
                        s = 3;
                        break;
                    }printf("|");
                }

                switch(s)
                {
                    case 0:
                        map[a[i].coord_x][a[i].coord_y].statutCase = 0; //pour que la case devienne passable
                        map[a[i].coord_x][a[i].coord_y].typeCase = 0;   //pour que la case devienne sol
                        map[a[i].coord_x][a[i].coord_y].pdv = 0;        //pdv pour le sol
                        a[i].coord_x = (a[i].coord_x-1);    //prends les nouvelles coordonnees
                        a[i].coord_y = (a[i].coord_y-1);    //prends les nouvelles coordonnees
                        if(map[a[i].coord_x][a[i].coord_y].typeCase == 2)   //si la case sur laquelle il va se deplacer est de l'eau
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].thirst++;                          //l'animal gagne un point de soif
                            a[i].coord_x = (a[i].coord_x+1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y+1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else if(map[a[i].coord_x][a[i].coord_y].typeCase == 3)   //si la case sur laquelle il va se deplacer est un buisson
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].hunger++;                          //l'animal gagne un point de faim
                            a[i].coord_x = (a[i].coord_x-1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else
                        {
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; // si la case o� va le d�placement est libre, alors la case devient impassable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //si la case o� va le d�placement est libre, alors la case devient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        break;
                    case 1:
                        map[a[i].coord_x][a[i].coord_y].statutCase = 0;
                        map[a[i].coord_x][a[i].coord_y].typeCase = 0;
                        map[a[i].coord_x][a[i].coord_y].pdv = 0;        //pdv pour le sol
                        a[i].coord_x = (a[i].coord_x-1);
                        a[i].coord_y = (a[i].coord_y);
                        if(map[a[i].coord_x][a[i].coord_y].typeCase == 2)   //si la case sur laquelle il va se deplacer est de l'eau
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].thirst++;                          //l'animal gagne un point de soif
                            a[i].coord_x = (a[i].coord_x+1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else if(map[a[i].coord_x][a[i].coord_y].typeCase == 3)  //si la case sur laquelle il va se deplacer est un buisson
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].hunger++;                          //l'animal gagne un point de faim
                            a[i].coord_x = (a[i].coord_x+1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else
                        {
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; // si la case o� va le d�placement est libre, alors la case devient impassable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //si la case o� va le d�placement est libre, alors la case devient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        break;
                        case 2:
                        map[a[i].coord_x][a[i].coord_y].statutCase = 0;
                        map[a[i].coord_x][a[i].coord_y].typeCase = 0;
                        map[a[i].coord_x][a[i].coord_y].pdv = 0;        //pdv pour le sol
                        a[i].coord_x = (a[i].coord_x-1);
                        a[i].coord_y = (a[i].coord_y+1);
                        if(map[a[i].coord_x][a[i].coord_y].typeCase == 2)   //si la case sur laquelle il marche est de l'eau
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].thirst++;                          //l'animal gagne un point de soif
                            a[i].coord_x = (a[i].coord_x+1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y-1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else if(map[a[i].coord_x][a[i].coord_y].typeCase == 3)  //si la case sur laquelle il marche est un buisson
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].hunger++;                          //l'animal gagne un point de faim
                            a[i].coord_x = (a[i].coord_x+1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y-1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else
                        {
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; // si la case o� va le d�placement est libre, alors la case devient impassable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //si la case o� va le d�placement est libre, alors la case devient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        break;
                    case 3:
                        map[a[i].coord_x][a[i].coord_y].statutCase = 0;
                        map[a[i].coord_x][a[i].coord_y].typeCase = 0;
                        map[a[i].coord_x][a[i].coord_y].pdv = 0;        //pdv pour le sol
                        a[i].coord_x = (a[i].coord_x);
                        a[i].coord_y = (a[i].coord_y-1);
                        if(map[a[i].coord_x][a[i].coord_y].typeCase == 2)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].thirst++;                          //l'animal gagne un point de soif
                            a[i].coord_x = (a[i].coord_x);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y+1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else if(map[a[i].coord_x][a[i].coord_y].typeCase == 3)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].hunger++;                          //l'animal gagne un point de faim
                            a[i].coord_x = (a[i].coord_x);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y+1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else
                        {
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; // si la case o� va le d�placement est libre, alors la case devient impassable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //si la case o� va le d�placement est libre, alors la case devient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        break;
                    case 4:
                        map[a[i].coord_x][a[i].coord_y].statutCase = 0;
                        map[a[i].coord_x][a[i].coord_y].typeCase = 0;
                        map[a[i].coord_x][a[i].coord_y].pdv = 0;        //pdv pour le sol
                        a[i].coord_x = (a[i].coord_x);
                        a[i].coord_y = (a[i].coord_y+1);
                        if(map[a[i].coord_x][a[i].coord_y].typeCase == 2)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].thirst++;                          //l'animal gagne un point de soif
                            a[i].coord_x = (a[i].coord_x);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y-1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else if(map[a[i].coord_x][a[i].coord_y].typeCase == 3)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].hunger++;                          //l'animal gagne un point de faim
                            a[i].coord_x = (a[i].coord_x);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y-1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else
                        {
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; // si la case o� va le d�placement est libre, alors la case devient impassable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //si la case o� va le d�placement est libre, alors la case devient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        break;
                    case 5:
                        map[a[i].coord_x][a[i].coord_y].statutCase = 0;
                        map[a[i].coord_x][a[i].coord_y].typeCase = 0;
                        map[a[i].coord_x][a[i].coord_y].pdv = 0;        //pdv pour le sol
                        a[i].coord_x = (a[i].coord_x+1);
                        a[i].coord_y = (a[i].coord_y-1);
                        if(map[a[i].coord_x][a[i].coord_y].typeCase == 2)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].thirst++;                          //l'animal gagne un point de soif
                            a[i].coord_x = (a[i].coord_x-1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y+1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else if(map[a[i].coord_x][a[i].coord_y].typeCase == 3)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].hunger++;                          //l'animal gagne un point de faim
                            a[i].coord_x = (a[i].coord_x-1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y+1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else
                        {
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; // si la case o� va le d�placement est libre, alors la case devient impassable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //si la case o� va le d�placement est libre, alors la case devient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        break;
                    case 6:
                        map[a[i].coord_x][a[i].coord_y].statutCase = 0;
                        map[a[i].coord_x][a[i].coord_y].typeCase = 0;
                        map[a[i].coord_x][a[i].coord_y].pdv = 0;        //pdv pour le sol
                        a[i].coord_x = (a[i].coord_x+1);
                        a[i].coord_y = (a[i].coord_y);
                        if(map[a[i].coord_x][a[i].coord_y].typeCase == 2)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].thirst++;                          //l'animal gagne un point de soif
                            a[i].coord_x = (a[i].coord_x-1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else if(map[a[i].coord_x][a[i].coord_y].typeCase == 3)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].hunger++;                          //l'animal gagne un point de faim
                            a[i].coord_x = (a[i].coord_x-1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else
                        {
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; // si la case o� va le d�placement est libre, alors la case devient impassable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //si la case o� va le d�placement est libre, alors la case devient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        break;
                    case 7:
                        map[a[i].coord_x][a[i].coord_y].statutCase = 0;
                        map[a[i].coord_x][a[i].coord_y].typeCase = 0;
                        map[a[i].coord_x][a[i].coord_y].pdv = 0;        //pdv pour le sol
                        a[i].coord_x = (a[i].coord_x+1);
                        a[i].coord_y = (a[i].coord_y+1);
                        if(map[a[i].coord_x][a[i].coord_y].typeCase == 2)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].thirst++;                          //l'animal gagne un point de soif
                            a[i].coord_x = (a[i].coord_x-1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y-1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else if(map[a[i].coord_x][a[i].coord_y].typeCase == 3)
                        {
                            map[a[i].coord_x][a[i].coord_y].pdv--;  //la ressource est entam�e
                            a[i].hunger++;                          //l'animal gagne un point de faim
                            a[i].coord_x = (a[i].coord_x-1);        //l'animal "retourne"/reste sur sa case
                            a[i].coord_y = (a[i].coord_y-1);        //l'animal "retourne"/reste sur sa case
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; //la case redevient passable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //la case redevient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        else
                        {
                            map[a[i].coord_x][a[i].coord_y].statutCase = 1; // si la case o� va le d�placement est libre, alors la case devient impassable
                            map[a[i].coord_x][a[i].coord_y].typeCase = 1;   //si la case o� va le d�placement est libre, alors la case devient animal
                            map[a[i].coord_x][a[i].coord_y].pdv = 100;  //useless
                        }
                        break;
                }
            }
        check++;
        a[i].hunger--;
        a[i].thirst--;
        }

        for(m=0; m<*l; m++) //checking pour verifier que si une ressource est d�c�d� alors elle devient sol
        {
            for(n=0; n<*c; n++)
            {
                if(map[m][n].pdv == 0)
                {
                    map[m][n].statutCase = 0;
                    map[m][n].typeCase = 0;
                    map[m][n].pdv = 0;
                }
            }
        }
    }
}

void test(int *l, int *c, sCaseMap map[LMAX][CMAX], int *animal_alive)
{
    int i, j;

    for(i=0; i<*l; i++)
    {
        for(j=0; j<*c; j++)
        {
            if(map[i][j].typeCase == 1)
            {
                animal_alive++;
            }
        }
    }
}

