#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//constantes
#define MAX_DESCRP_COURTE 20
#define MAX_DESCRP 100
#define MAX_NOM 20

#define LINE_SIZE 1000
#define NOM_FICHIER_TXT_ETAPES "STRUCT.txt"
#define NOM_FICHIER_TXT_PLAYERS "PLAYERS.txt"

#define NB_MAX_ETAPES 49


//structures
typedef struct etape{

int id;
int impactEco;
int impactSocial;
int impactEnviron;
int impactFolie;

char descriptionCourte[MAX_DESCRP_COURTE];

char descpritionImpact[MAX_DESCRP];
struct etape *suiv;


}STEP;

typedef struct player{
    int eco; //jauge eco
    int social; // jauge sociale    
    int environ; // jauge environnementale
    int folie; //jauge de folie
    char nom[MAX_NOM]; //pseudo du joeur
    int avancement[49]; // pour savoir quelles étapes le joueur à déja parcouru
    struct player *suiv;


}PLAYER;



//def fonctions
STEP *chargerTxt(char *nomFichier);
STEP *recupererLigne(char *ligne);
PLAYER *recupererLigneJoueur(char *ligne);
STEP *insertionAlpha(STEP *liste, STEP *nouvelSTEP);
PLAYER *insertionAlphaJoueur(PLAYER *liste, PLAYER *nouvelSTEP);
void afficheSTEPent(STEP *STEP);
void afficheListe(STEP *liste);
void printMenu(void);
void afficheJoueur(PLAYER *joueur);
PLAYER *chargerJoueurTxt(char *nomFichier);
void insererTete(STEP** liste, STEP* nouv);
void insererAleatoire(STEP** liste, STEP* etape);
void appliquerImpact(PLAYER* joueur, STEP* e);
int verifierDefaite(PLAYER* joueur);
void afficherJauges(PLAYER* joueur);
void jeu(PLAYER* joueur, STEP* liste);
STEP* retirerTete(STEP** liste);


int main(void){
    srand(time(NULL));
    STEP *liste = NULL;
    PLAYER *joueur=NULL;
    
    printf("...chargement des fichiers...\n");
	liste = chargerTxt(NOM_FICHIER_TXT_ETAPES);
    joueur=chargerJoueurTxt(NOM_FICHIER_TXT_PLAYERS);
    int exit = 0;
	char choix;
    while (exit == 0)
	{
        printMenu();
        scanf(" %c", &choix); 
        switch (choix)
		{
		case '0':
		case 'o':
		case 'O':
			exit = 1;
            break;
        case '1':
             afficheListe(liste);
            break;
        case '2':
            afficheJoueur(joueur);
            break;
        case '3':
            jeu(joueur,liste);
            break;
        default:
            printf("Commande inconnue\n");
            break;
        }
    }
    return 0;
}




void printMenu(void)
{
	printf("\n\n\nThis is the Fake News MENU\n");
	printf("0 - exit \n");
    printf("1 - affichier toutes les étapes \n");
    printf("2 - afficher les joeurs\n");
	printf("3 - Jeu Dev Version\n");
	printf("4 - TBD\n");
	printf("5 - TBD\n");
}
/*Chargement du fichier joueur dans une liste chainée*/
PLAYER *chargerJoueurTxt(char *nomFichier){

    printf("chargement du fichier %s\n", nomFichier);
	PLAYER *joueur = NULL;
	PLAYER *new = NULL;
	FILE *f;
    f = fopen(nomFichier, "r");
    if (f == NULL)
	{
		printf("erreur lecture de fichier %s chargement annulee\n", nomFichier);
		return NULL;
	}

	char lineBuffer[LINE_SIZE];
    while (fgets(lineBuffer, LINE_SIZE, f) != NULL)
	{
		new = recupererLigneJoueur(lineBuffer);
		if (new != NULL)
			joueur = insertionAlphaJoueur(joueur, new);
	}

	fclose(f);
	return joueur;
}

/* Converti la ligne du fichier stockant les joueurs ecrit en TXT en une structure que le programme peut comprendre */
PLAYER *recupererLigneJoueur(char *ligne)
{
	PLAYER *new = malloc(sizeof(PLAYER));
	if (new == NULL)
	{
		printf("Erreur allocation memoire dans recupererLigneJoueur()\n");
		return NULL;
	}
	memset(new, 0, sizeof *new);

	/* trim newline */
	size_t len = strlen(ligne);
	if (len > 0 && ligne[len-1] == '\n')
		ligne[len-1] = '\0';
	
	if (ligne[0] == '\0') { /* ligne vide */
		free(new);
		return NULL;
	}

	char separator[2] = ";";
	char *token = strtok(ligne, separator); // eco (no id field)
	if (!token) goto fail;
	new->eco = atoi(token);

	token = strtok(NULL, separator);
	if (!token) goto fail;
	new->social = atoi(token);

	token = strtok(NULL, separator);
	if (!token) goto fail;
	new->environ = atoi(token);

	token = strtok(NULL, separator);
	if (!token) goto fail;
	new->folie = atoi(token);

	token = strtok(NULL, separator);
	if (!token) goto fail;
	strncpy(new->nom, token, MAX_NOM-1);
	new->nom[MAX_NOM-1] = '\0';

	/* read avancement fields if present, otherwise leave at 0 */
	for(int i = 0; i < NB_MAX_ETAPES; ++i) {
		token = strtok(NULL, separator);
		if (token)
			new->avancement[i] = atoi(token);
		else
			new->avancement[i] = 0;
	}

	new->suiv = NULL;

	return new;

fail:
	free(new);
	return NULL;
}

STEP *chargerTxt(char *nomFichier)
{
	printf("chargement du fichier %s\n", nomFichier);
	STEP *liste = NULL;
	STEP *new = NULL;
	FILE *f;

	f = fopen(nomFichier, "r");
	if (f == NULL)
	{
		printf("erreur lecture de fichier %s chargement annulee\n", nomFichier);
		return NULL;
	}

	char lineBuffer[LINE_SIZE];

	while (fgets(lineBuffer, LINE_SIZE, f) != NULL)
	{
		new = recupererLigne(lineBuffer);
		liste = insertionAlpha(liste, new);
	}

	fclose(f);
	return liste;
}

/* Converti la ligne du fichier stockant les étapes ecrit en TXT en une structure que le programme peut comprendre */
STEP *recupererLigne(char *ligne)
{
	STEP *new = malloc(sizeof(STEP));
	if (new == NULL)
	{
		printf("Erreur allocation memoire dans recupererLigne()\n");
		return NULL;
	}
	memset(new,0,sizeof *new);

	/* trim newline */
	size_t len = strlen(ligne);
	if (len > 0 && ligne[len-1] == '\n')
		ligne[len-1] = '\0';
	if (ligne[0] == '\0') {
		free(new);
		return NULL;
	}

	char separator[2] = ";";
	char *token = strtok(ligne, separator);
	if (!token) goto fail;
	char *fin;
	new->id = strtol(token, &fin, 10);
//strol pas necessaire.

	token = strtok(NULL, separator);
	if (!token) goto fail;
	new->impactEco = strtol(token, &fin, 10);

	token = strtok(NULL, separator);
	if (!token) goto fail;
	new->impactSocial = atoi(token);

	token = strtok(NULL, separator);
	if (!token) goto fail;
	new->impactEnviron = atoi(token);

	token = strtok(NULL, separator);
	if (!token) goto fail;
	new->impactFolie = atoi(token);

	token = strtok(NULL, separator);
	if (!token) goto fail;
	strncpy(new->descriptionCourte, token, MAX_DESCRP_COURTE-1);
	new->descriptionCourte[MAX_DESCRP_COURTE-1] = '\0';

	token = strtok(NULL, separator);
	if (!token) goto fail;
	strncpy(new->descpritionImpact, token, MAX_DESCRP-1);
	new->descpritionImpact[MAX_DESCRP-1] = '\0';

	new->suiv = NULL;
	return new;

fail:
	free(new);
	return NULL;
}
//Insertion des strucutures joueur par ordre alphabétique
PLAYER *insertionAlphaJoueur(PLAYER *liste, PLAYER *nouvelSTEP)
{
	if (nouvelSTEP == NULL)
	{
		printf("Rien a inserer\n");
		return liste;
	}

	if (liste == NULL)
	{ // cas d'une lsite vide
		nouvelSTEP->suiv = NULL;
		liste = nouvelSTEP;
		return liste;
	}

	PLAYER *courant = liste;
	PLAYER *precedent = NULL;
	while (courant != NULL)
	{
		// parcour de la liste pour trouver le bon endroit ou inserer
		if (strcmp(courant->nom, nouvelSTEP->nom) > 0)
		{ // on a trouver le bon endroit
			if (courant == liste)
			{ // ajout debut
				nouvelSTEP->suiv = liste;
				liste = nouvelSTEP;
				return liste;
			}
			else
			{
				precedent->suiv = nouvelSTEP;
				nouvelSTEP->suiv = courant;
				return liste;
			}
		}
		else
		{ //on continue d'avancer
			precedent = courant;
			courant = courant->suiv;
		}
	}
	 //ici on est sortie de la boucle => fin de la liste courant == NULL
	precedent->suiv = nouvelSTEP;
	nouvelSTEP->suiv = NULL;

	return liste;
}


//Insertion des structures des étapes par id croissant
STEP *insertionAlpha(STEP *liste, STEP *nouvelSTEP)
{
	if (nouvelSTEP == NULL)
	{
		printf("Rien a insert\n");
		return liste;
	}

	if (liste == NULL)
	{ /* cas d'une lsite vide */
		nouvelSTEP->suiv = NULL;
		liste = nouvelSTEP;
		return liste;
	}

	STEP *courant = liste;
	STEP *precedent = NULL;
	while (courant != NULL)
	{
		/* parcour de la liste pour trouver le bon endroit ou insert */
		if (courant->id>nouvelSTEP->id)
		{ /* on a trouver le bon endroit */
			if (courant == liste)
			{ /* ajout debut */
				nouvelSTEP->suiv = liste;
				liste = nouvelSTEP;
				return liste;
			}
			else
			{
				precedent->suiv = nouvelSTEP;
				nouvelSTEP->suiv = courant;
				return liste;
			}
		}
		else
		{ /* on continue d'avancer */
			precedent = courant;
			courant = courant->suiv;
		}
	}
	/* ici on est sortie de la boucle => fin de la liste courant == NULL */
	precedent->suiv = nouvelSTEP;
	nouvelSTEP->suiv = NULL;

	return liste;
}


//Affiche le contenue d'une structure étape
void afficheSTEPent(STEP *STEP)
{
	if (STEP == NULL)
	{
		printf("afficheSTEPent impossible sur STEPent vide\n");
		return;
	}
	printf("|----------------------------------------------------------------------------------|\n");
    printf("|id : %d|\n",STEP->id);
    printf("|impact Eco : %d|\n",STEP->impactEco);
    printf("|impact Social : %d|\n",STEP->impactSocial);
    printf("|impact Environnement : %d|\n",STEP->impactEnviron);
    printf("|impact Folie : %d|\n",STEP->impactFolie);
	printf("|description courte  : %s|\n", STEP->descriptionCourte);
	printf("|description impact : %s|\n", STEP->descpritionImpact);
}
//Affiche le contenu d'une liste chainée
void afficheListe(STEP *liste)
{
	STEP *courant = liste;
	if (liste == NULL)
	{
		printf("afficheListe impossible sur liste vide\n");
		return;
	}
	while (courant != NULL) /* tant qu'il reste des STEPents */
	{
		afficheSTEPent(courant);

		/* on avance vers l'STEPent suivant */
		courant = courant->suiv;
	}
}



//Affiche les champs d'une structure PLAYER
void afficheJoueur(PLAYER *joueur)
{
	PLAYER *courant = joueur;
	if (joueur == NULL)
	{
		printf("afficheJoueur impossible sur liste vide\n");
		return;
	}
	while (courant != NULL)
	{
		printf("|----------------------------------------------------------------------------------|\n");
	    printf("|jauge Eco : %d|\n",courant->eco);
	    printf("|jauge Social : %d|\n",courant->social);
	    printf("|jauge Environnement : %d|\n",courant->environ);
	    printf("|jauge folie : %d|\n",courant->folie);
		printf("|nom : %s|\n", courant->nom);
         for(int i = 0; i < 49; i++) {
            printf("avancement[%d]=%d ",i,courant->avancement[i]);

         }
		courant = courant->suiv;
	}
}

void insererAleatoire(STEP** liste, STEP* etape) {
    if (*liste == NULL) {
        *liste = etape;
        etape->suiv = NULL;
        return;
    }

    int position = rand() % NB_MAX_ETAPES;

    STEP* courant = *liste;
    STEP* precedent = NULL;
    int i = 0;

    while (courant != NULL && i < position) {
        precedent = courant;
        courant = courant->suiv;
        i++;
    }

    if (precedent == NULL) {
        etape->suiv = *liste;
        *liste = etape;
    } else {
        precedent->suiv = etape;
        etape->suiv = courant;
    }
}

void insererTete(STEP** liste, STEP* nouv) {
    nouv->suiv = *liste;
    *liste = nouv;
}

//================= LOGIQUE JEU =================

void appliquerImpact(PLAYER* joueur, STEP* e) {
    joueur->eco += e->impactEco;
    joueur->social += e->impactSocial;
    joueur->environ += e->impactEnviron;
    joueur->folie += e->impactFolie;

    if (joueur->eco < 0) joueur->eco = 0;
    if (joueur->social < 0) joueur->social = 0;
    if (joueur->environ < 0) joueur->environ = 0;
}

int verifierDefaite(PLAYER* joueur) {
    if (joueur->eco <= 0) return 1;
    if (joueur->social <= 0) return 1;
    if (joueur->environ <= 0) return 1;
    if (joueur->folie >= 100) return 1;
    return 0;
}

void afficherJauges(PLAYER* joueur) {
    printf("\n=== Jauges ===\n");
    printf("Eco : %d\n", joueur->eco);
    printf("Social : %d\n", joueur->social);
    printf("Environnement : %d\n", joueur->environ);
    printf("Folie : %d\n", joueur->folie);
}
STEP* retirerTete(STEP** liste) {
    if (*liste == NULL) return NULL;

    STEP* tmp = *liste;
    *liste = (*liste)->suiv;
    tmp->suiv = NULL;
    return tmp;
}


void jeu(PLAYER* joueur, STEP* liste){


     while (!verifierDefaite(joueur)) {

        if (liste == NULL || liste->suiv == NULL) {
            printf("Plus assez d'étapes.\n");
            break;
        }

        STEP* choix1 = retirerTete(&liste);
        STEP* choix2 = retirerTete(&liste);

        printf("\nChoisissez une étape :\n");
        printf("1 - %s\n", choix1->descriptionCourte);
        printf("2 - %s\n", choix2->descriptionCourte);

        int choix;
        scanf("%d", &choix);

        STEP* choisie;
        STEP* nonChoisie;

        if (choix == 1) {
            choisie = choix1;
            nonChoisie = choix2;
        } else {
            choisie = choix2;
            nonChoisie = choix1;
        }

        printf("\n%s\n", choisie->descpritionImpact);

        appliquerImpact(joueur, choisie);

        joueur->avancement[choisie->id - 1] = 1;

        afficherJauges(joueur);

        insererAleatoire(&liste, nonChoisie);

        free(choisie);
    }

    printf("\nYou're Fired  %s !\n", joueur->nom);

    
}
