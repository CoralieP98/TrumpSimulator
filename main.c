#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/// define ///

#define MAX_DESCRP 150
#define MAX_NOM 20

#define LINE_SIZE 1000
#define NOM_FICHIER_TXT_ETAPES "STRUCT.txt"
#define NOM_FICHIER_TXT_PLAYERS "PLAYERS.txt"

#define NB_MAX_ETAPES 49

#define START_GAUGE 75
#define START_MADNESS 5

/// structures ///

typedef struct action {
    int idAction;
    int impEco;
    int impSocial;
    int impEnviro;
    int madness;
    char description[MAX_DESCRP];
    char impDescription[MAX_DESCRP];
    struct action *next;
}ACTION;

typedef struct player {
    int idPlayer;
    char firstName[MAX_DESCRP];
    char lastName[7];

    int gaugeSocial;
    int gaugeEco;
    int gaugeEnviro;
    int gaugeMadness;

    int progression[NB_MAX_ETAPES];

    int temporality;

    char password[15];
    struct player *next;

}PLAYER;


/// Prototypes ///
void printMenu(void);
void displayAction(ACTION *action);
void afficheListe(ACTION *list);

ACTION *chargerTxt(char *nomFichier);
ACTION *recupererLigne(char *line);
ACTION *insertionAlphaActions(ACTION *list, ACTION *newAction);

void displayPlayerStatus(PLAYER *player);
PLAYER *chargerJoueurTxt(char *nomFichier);
PLAYER *recupererLigneJoueur(char *line);
PLAYER *insertionJoueurById(PLAYER *list, PLAYER *newPlayer);
int genererIdUnique(char *nomFichier);
PLAYER *initializePlayer(char *nomFichier);
int enregistrerJoueur(PLAYER *player, char *nomFichier);
PLAYER *creerEtEnregistrerJoueur(char *nomFichier);

PLAYER *chargePlayerById(PLAYER *list, int id);
PLAYER *findPlayerById(PLAYER *list, int id);

void saveProgression(PLAYER *list, char *nomFichier);

void initializeActions(ACTION *list,ACTION **action1, ACTION **action2);
void vizualiseAction(ACTION *action);
void realizeAction(PLAYER *player, ACTION *action);

int checkGameOver(PLAYER *player);
void displayEnding(PLAYER *player, int endingType);

//fonction qui gere tout :
void gamePLay(PLAYER *player, ACTION *list,PLAYER *listPlayers, char *fichierSave);


int main(void) {

    //le fichier struct marche mais j'ai demandé a claude de le faire pour pas me faire chié (du coup les blagues et les modificateurs sont nuls)
    //je le referai mieux lundi
    //ça et le coté "joli" des menus si on a le temps

    srand(time(NULL));
    ACTION *list = NULL;
    PLAYER *Trump = NULL;


    printf("...chargement des fichiers...\n");
    list = chargerTxt(NOM_FICHIER_TXT_ETAPES);
    Trump = chargerJoueurTxt(NOM_FICHIER_TXT_PLAYERS);

    int choice;
    int end = 0;

    while (end == 0){
        printMenu();


        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("\n### NOUVELLE PARTIE ###\n");
                PLAYER *newPlayer = creerEtEnregistrerJoueur(NOM_FICHIER_TXT_PLAYERS);
                if (newPlayer != NULL) {
                    //Ajoute le nouveau à la liste
                    Trump = insertionJoueurById(Trump, newPlayer);

                    //go inGame avec ce joueur direct
                    gamePLay(newPlayer, list, Trump,NOM_FICHIER_TXT_PLAYERS);
                }
                break;

            case 2:
                //charger une partie existante donc rentrer l'id du player qu'on veut jouer et recup sa progression et enchainer sur le jeux

                if (Trump == NULL) {
                    printf("\n Aucun joueur enregistre. Creez d'abord votre president. (option 1)\n");
                    break;
                }

                printf("\n### CHARGER UNE PARTIE ###\n");
                printf("Entrez l'ID du joueur: ");
                int id;
                scanf(" %d", &id);
                getchar();

                PLAYER *player = chargePlayerById(Trump, id);

                if (player!= NULL) {
                    gamePLay(player, list, Trump, NOM_FICHIER_TXT_PLAYERS);
                }

                break;
            case 3:
                printf("\n### TOUTES LES ACTIONS POSSIBLES ###\n\n");
                afficheListe(list);
                break;
            case 4:
                //afficher tout les players (notamment pour voir l'id qui nous interesse pour charger derriere)
                printf("\n### TOUS LES JOUEURS ###\n");
                displayPlayerStatus(Trump);
                break;
            case 0:
                printf("\nAu revoir! Make America Great Again!\n");
                end = 1;
                break;
            default:
                printf("\n Choix invalide.\n\n");
                break;
        }
    }

    return 0;
}

void printMenu(void)
{
    printf("#### TRUMP SIMULATOR ####\n");
    printf("\nThis is the Fake News MENU\n");
    printf("#########################\n");
    printf("1 - New Game\n");
    printf("2 - PLAY\n");
    printf("3 - See the backstages \n"); //afficher toutes les étapes
    printf("4 - See all players\n");
    printf("0 - Exit ----\n");
    printf("#########################\n");
}


//Affiche le contenu d'une liste chainée
void afficheListe(ACTION *list)
{
    ACTION *courant = list;
    if (list == NULL)
    {
        printf("afficheListe impossible sur liste vide\n");
        return;
    }
    while (courant != NULL) /* tant qu'il reste des actions */
    {
        displayAction(courant);

        /* on avance vers l'action suivant */
        courant = courant->next;
    }
}

//Affiche le contenue d'une structure étape
void displayAction(ACTION *action)
{
    if (action == NULL)
    {
        printf("display action impossible sur action vide\n");
        return;
    }
    printf("|----------------------------------------------------------------------------------|\n");
    printf("|id : %d|\n",action->idAction);
    printf("|impact Eco : %d|\n",action->impEco);
    printf("|impact Social : %d|\n",action->impSocial);
    printf("|impact Environnement : %d|\n",action->impEnviro);
    printf("|impact Folie : %d|\n",action->madness);
    printf("|description action  : %s|\n", action->description);
    printf("|description impact : %s|\n", action->impDescription);
}

ACTION *chargerTxt(char *nomFichier)
{
    printf("chargement du fichier %s\n", nomFichier);
    ACTION *list = NULL;
    ACTION *new = NULL;
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
        list = insertionAlphaActions(list, new);
    }

    fclose(f);
    return list;
}

/* Converti la ligne du fichier stockant les étapes ecrit en TXT en une structure que le programme peut comprendre */
ACTION *recupererLigne(char *line)
{
    ACTION *new = malloc(sizeof(ACTION));
    if (new == NULL)
    {
        printf("Erreur allocation memoire dans recupererLigne()\n");
        return NULL;
    }
    memset(new,0,sizeof *new);

    /* trim newline */
    size_t len = strlen(line);
    if (len > 0 && line[len-1] == '\n')
        line[len-1] = '\0';
    if (line[0] == '\0') {
        free(new);
        return NULL;
    }

    char separator[2] = ";";
    char *token = strtok(line, separator);
    if (!token) goto fail;
    new->idAction = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->impEco = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->impSocial = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->impEnviro = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->madness = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    strncpy(new->description, token, MAX_DESCRP-1);
    new->description[MAX_DESCRP-1] = '\0';

    token = strtok(NULL, separator);
    if (!token) goto fail;
    strncpy(new->impDescription, token, MAX_DESCRP-1);
    new->impDescription[MAX_DESCRP-1] = '\0';

    new->next = NULL;
    return new;

    //goto : sorte de saut qui ramene a fail (plus robuste & evite de repeter fail 36 fois)
    fail:
        free(new);
    return NULL;
}

//Insertion des structures des étapes par id croissant
ACTION *insertionAlphaActions(ACTION *list, ACTION *newAction)
{
    if (newAction == NULL)
    {
        printf("Rien a insert\n");
        return list;
    }

    if (list == NULL)
    { /* cas d'une liste vide */
        newAction->next = NULL;
        list = newAction;
        return list;
    }

    ACTION *courant = list;
    ACTION *precedent = NULL;
    while (courant != NULL)
    {
        /* parcour de la liste pour trouver le bon endroit ou insert */
        if (courant->idAction>newAction->idAction)
        { /* on a trouver le bon endroit */
            if (courant == list)
            { /* ajout debut */
                newAction->next = list;
                list = newAction;
                return list;
            }
            else
            {
                precedent->next = newAction;
                newAction->next = courant;
                return list;
            }
        }
        else
        { /* on continue d'avancer */
            precedent = courant;
            courant = courant->next;
        }
    }
    /* ici on est sortie de la boucle => fin de la liste courant == NULL */
    precedent->next = newAction;
    newAction->next = NULL;

    return list;
}








/*Chargement du fichier joueur dans une liste chainée*/
PLAYER *chargerJoueurTxt(char *nomFichier){

    printf("chargement du fichier %s\n", nomFichier);
    PLAYER *player = NULL;
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
            player = insertionJoueurById(player, new);
    }

    fclose(f);
    return player;
}

/* Converti la ligne du fichier stockant les joueurs ecrit en TXT en une structure que le programme peut comprendre */
PLAYER *recupererLigneJoueur(char *line)
{
    PLAYER *new = malloc(sizeof(PLAYER));
    if (new == NULL)
    {
        printf("Erreur allocation memoire dans recupererLigneJoueur()\n");
        return NULL;
    }
    memset(new, 0, sizeof *new);

    /* trim newline */
    size_t len = strlen(line);
    if (len > 0 && line[len-1] == '\n')
        line[len-1] = '\0';

    if (line[0] == '\0') { /* ligne vide */
        free(new);
        return NULL;
    }

    char separator[2] = ";";
    char *token = strtok(line, separator);
    if (!token) goto fail;
    new->idPlayer = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->gaugeEco = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->gaugeSocial = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->gaugeEnviro = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->gaugeMadness = atoi(token);

    token = strtok(NULL, separator);
    if (!token) goto fail;
    strncpy(new->firstName, token, MAX_NOM-1);
    new->firstName[MAX_NOM-1] = '\0';

    token = strtok(NULL, separator);
    if (!token) goto fail;
    strncpy(new->lastName, token, 6);
    new->lastName[6] = '\0';

    // 8. Temporality (nouveau)
    token = strtok(NULL, separator);
    if (!token) goto fail;
    new->temporality = atoi(token);

    // 9. Password (nouveau)
    token = strtok(NULL, separator);
    if (!token) goto fail;
    strncpy(new->password, token, 14);
    new->password[14] = '\0';

    /* read progression fields if present, otherwise leave at 0 */
    for(int i = 0; i < NB_MAX_ETAPES; ++i) {
        token = strtok(NULL, separator);
        if (token)
            new->progression[i] = atoi(token);
        else
            new->progression[i] = 0;
    }

    new->next = NULL;

    return new;

    fail:
        free(new);
    return NULL;
}

//Insertion des strucutures joueur par  id croissant
PLAYER *insertionJoueurById(PLAYER *list, PLAYER *newPlayer)
{
    if (newPlayer == NULL)
    {
        printf("Rien a inserer\n");
        return list;
    }

    if (list == NULL)
    { // cas d'une lsite vide
        newPlayer->next = NULL;
        list = newPlayer;
        return list;
    }

    PLAYER *courant = list;
    PLAYER *precedent = NULL;
    while (courant != NULL)
    {
        // parcour de la liste pour trouver le bon endroit ou inserer
        if (courant->idPlayer && newPlayer->idPlayer)
        { // on a trouver le bon endroit
            if (courant == list)
            { // ajout debut
                newPlayer->next = list;
                list = newPlayer;
                return list;
            }
            else
            {
                precedent->next = newPlayer;
                newPlayer->next = courant;
                return list;
            }
        }
        else
        { //on continue d'avancer
            precedent = courant;
            courant = courant->next;
        }
    }
    //ici on est sortie de la boucle => fin de la liste courant == NULL
    precedent->next = newPlayer;
    newPlayer->next = NULL;

    return list;
}

// Génère un ID unique en lisant le fichier existant
int genererIdUnique(char *nomFichier)
{
    FILE *f = fopen(nomFichier, "r");
    int maxId = 0;

    if (f == NULL) {
        return 1; // Premier joueur
    }

    char line[2048]; // Grosse ligne à cause de progression
    while (fgets(line, 2048, f) != NULL) {
        int id;
        if (sscanf(line, "%d;", &id) == 1) {
            if (id > maxId) {
                maxId = id;
            }
        }
    }

    fclose(f);
    return maxId + 1;
}


PLAYER *initializePlayer(char *nomFichier) {
    PLAYER *new = malloc(sizeof(PLAYER));
    if (new == NULL) {
        printf("Erreur allocation memoire pour le joueur\n");
        return NULL;
    }

    memset(new, 0, sizeof(PLAYER));

    printf("\n=== CREATION DE VOTRE PRESIDENT ===\n\n");

    // Génère un ID
    new->idPlayer = genererIdUnique(nomFichier);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Entrez votre prenom: ");
    fgets(new->firstName, MAX_DESCRP, stdin);
    size_t len = strlen(new->firstName);
    if (len > 0 && new->firstName[len-1] == '\n') {
        new->firstName[len-1] = '\0';
    }

    // Nom = Trump pour tout le monde
    strcpy(new->lastName, "Trump");

    // Initialise les jauges
    new->gaugeSocial = START_GAUGE;
    new->gaugeEco = START_GAUGE;
    new->gaugeEnviro = START_GAUGE;
    new->gaugeMadness = START_MADNESS;

    // Commence au mois 1
    new->temporality = 1;

    // Initialise progression à 0
    for (int i = 0; i < NB_MAX_ETAPES; i++) {
        new->progression[i] = 0;
    }

    new->next = NULL;

    printf("\n>>> Joueur cree: %s %s (ID: %d) <<<\n\n",
           new->firstName, new->lastName, new->idPlayer);

    return new;
}

// Enregistre un joueur dans le fichier (format compatible)
int enregistrerJoueur(PLAYER *player, char *nomFichier)
{
    if (player == NULL) {
        printf("Erreur: joueur NULL\n");
        return 0;
    }

    FILE *f = fopen(nomFichier, "a");
    if (f == NULL) {
        printf("Erreur: impossible d'ouvrir %s\n", nomFichier);
        return 0;
    }

    // Format: id;eco;social;enviro;madness;firstName;lastName;temporality;password;progression...
    fprintf(f, "%d;%d;%d;%d;%d;%s;%s;%d;%s",
            player->idPlayer,
            player->gaugeEco,
            player->gaugeSocial,
            player->gaugeEnviro,
            player->gaugeMadness,
            player->firstName,
            player->lastName,
            player->temporality,
            player->password);

    // Ajoute le tableau progression
    for (int i = 0; i < NB_MAX_ETAPES; i++) {
        fprintf(f, ";%d", player->progression[i]);
    }

    fprintf(f, "\n");

    fclose(f);

    printf(">>> Joueur enregistre dans %s <<<\n\n", nomFichier);
    return 1;
}


PLAYER *creerEtEnregistrerJoueur(char *nomFichier)
{
    PLAYER *nouveauJoueur = initializePlayer(nomFichier);

    if (nouveauJoueur == NULL) {
        return NULL;
    }

    if (!enregistrerJoueur(nouveauJoueur, nomFichier)) {
        printf("Attention: joueur cree mais pas enregistre!\n");
    }

    return nouveauJoueur;
}











// Initialise deux actions aléatoires et retourne leurs IDs
// Pour l'instant, on retourne juste deux IDs aléatoires
void initializeActions(ACTION *list,ACTION **action1, ACTION **action2) {
    // regarde si assez d'actions
    int count =0;
    ACTION *current = list;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    if (count < 2) {
        printf("Pas assez d'actions dans la liste!\n");
        *action1 = NULL;
        *action2 = NULL;
    }
    int index1 = rand() % count;
    int index2 = rand() % count;

    while (index2 == index1) {
        index2 = rand() % count;
    }

    current = list;
    int i = 0;

    while (current != NULL) {
        if (i == index1) {
            *action1 = current;
        }
        if (i == index2) {
            *action2 = current;
        }
        i++;
        current = current->next;
    }
}

// Affiche la desc d'une action betement
void vizualiseAction(ACTION *action) {

    if (action == NULL) {
        printf("Action NULL\n");
        return;
    }

    printf("- - - - - - - - - - - - - - - - - - - - -\n");
    printf("%s\n", action->description);
    printf("- - - - - - - - - - - - - - - - - - - - -\n");


}


// Applique les impacts de l'action choisie sur les jauges du joueur
void realizeAction(PLAYER *player, ACTION *action) {
    if (player == NULL || action == NULL) {
        return;
    }

    // Applique les modificateurs
    player->gaugeSocial += action->impSocial;
    player->gaugeEco += action->impEco;
    player->gaugeEnviro += action->impEnviro;
    player->gaugeMadness += action->madness;
    player->temporality++;

    // Limite les jauges entre 0 et 100
    if (player->gaugeSocial < 0) player->gaugeSocial = 0;
    if (player->gaugeSocial > 100) player->gaugeSocial = 100;

    if (player->gaugeEco < 0) player->gaugeEco = 0;
    if (player->gaugeEco > 100) player->gaugeEco = 100;

    if (player->gaugeEnviro < 0) player->gaugeEnviro = 0;
    if (player->gaugeEnviro > 100) player->gaugeEnviro = 100;

    if (player->gaugeMadness < 0) player->gaugeMadness = 0;
    if (player->gaugeMadness > 100) player->gaugeMadness = 100;

    if (action->idAction >= 1 && action->idAction <= NB_MAX_ETAPES) {
        player->progression[action->idAction -1] = 1;
    }

    printf("\n Les terriiibles consequences: %s \n",action->impDescription);
}


// Fonction utile pour afficher l'état actuel du joueur // utilisé dans le menu pour voir tout les joueurs et selectionner l'id
void displayPlayerStatus(PLAYER *player) {

    PLAYER *current = player;
    if (player == NULL) {
        printf("affiche statut joueur impossible sur liste vide\n");
        return;
    }

    int count =1;

    while (current != NULL) {
        printf("\n\n=== ETAT ACTUEL ===\n");
        printf("JOUEUR %d\n",count);
        printf("Nom complet: %s %s\n", current->firstName, current->lastName);
        printf("Id: %d\n", current->idPlayer);
        printf("Mois: %d/48\n",current->temporality);
        printf("Social:       [%d/100]\n", current->gaugeSocial);
        printf("Economie:     [%d/100]\n", current->gaugeEco);
        printf("Environement: [%d/100]\n", current->gaugeEnviro);
        printf("Folie:        [%d/100]\n\n", current->gaugeMadness);
        // for(int i = 0; i < 49; i++) {
        //     printf("avancement[%d]=%d ",i,current->progression[i]);
        // }
        current = current->next;
        count++;
    }
    printf("\n");
}

//deux fonctions pour charger une partie en selectionnant un joueur avec son id
PLAYER *findPlayerById(PLAYER *list, int id) {
    PLAYER *current = list;
    while (current != NULL) {
        if (current->idPlayer == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

PLAYER *chargePlayerById(PLAYER *list, int id) {
    PLAYER *player = findPlayerById(list, id);
    if (player == NULL) {
        printf("\n joueur avec id %d introuvable.\n", id);
        return NULL;
    }

    printf("\nJoueur charge avec succes!\n");
    printf("- - - - - - - - - - - - - - - - - - - - -\n");
    printf("  President: %s %s\n", player->firstName, player->lastName);
    printf("  ID: %d\n", player->idPlayer);
    printf("  Mois actuel: %d/48\n", player->temporality);
    printf("- - - - - - - - - - - - - - - - - - - - -\n");
    printf("  Social:       [%d/100]\n", player->gaugeSocial);
    printf("  Economie:     [%d/100]\n", player->gaugeEco);
    printf("  Environnement:[%d/100]\n", player->gaugeEnviro);
    printf("  Folie:        [%d/100]\n", player->gaugeMadness);
    printf("- - - - - - - - - - - - - - - - - - - - -\n");

    return player;
}


//gestion des fins
int checkGameOver(PLAYER *player) {

    // FIN 1: Folie à 100 - Trump complètement fou
    if (player->gaugeMadness >= 100) {
        return 1;
    }

    // FIN 2: Social à 0 - Révolution populaire
    if (player->gaugeSocial <= 0) {
        return 2;
    }

    // FIN 3: Économie à 0 - Effondrement économique
    if (player->gaugeEco <= 0) {
        return 3;
    }

    // FIN 4: Environnement à 0 - Catastrophe écologique ou autre
    if (player->gaugeEnviro <= 0) {
        return 4;
    }


    if (player->temporality == 48){
        return 5;
    }

    else {
        return 0;
        //no end checked le game play continue
    }
}

// Affiche le texte de fin correspondant au type de fin
void displayEnding(PLAYER *player, int endingType) {
    printf("\n");
    printf("===============================================\n");
    printf("           FIN DU MANDAT                       \n");
    printf("===============================================\n\n");

    switch(endingType) {
        case 1:
            printf("=== GAME OVER: FOLIE TOTALE ===\n\n");
            printf("Votre folie a atteint des sommets inimaginables!\n");
            printf("Apres avoir declare la guerre aux extraterrestres\n");
            printf("et essaye de racheter la Lune et vous appuye sur le bouton rouge interdit\n");
            break;

        case 2:
            printf("=== GAME OVER: REVOLUTION POPULAIRE ===\n\n");
            printf("Le mecontentement social a atteint son paroxysme!\n");
            printf("Des millions d'Americains envahissent la Maison Blanche.\n");
            printf("Vous etes contraint de demissionner et de fuir\n");
            printf("en Russie ou vous ouvrez une chaine de fast-foods.\n");
            break;

        case 3:
            printf("=== GAME OVER: EFFONDREMENT ECONOMIQUE ===\n\n");
            printf("L'economie americaine s'est completement effondree!\n");
            printf("Le dollar ne vaut plus rien, Wall Street et a la rue.'Pun intended'\n");
            printf("Vous etes impeache et l'Amerique met 50 ans\n");
            printf("a se remettre de votre existence.\n");
            break;

        case 4:
            printf("=== GAME OVER: CATASTROPHE ECOLOGIQUE ===\n\n");
            printf("Votre mepris total pour l'environnement a eu\n");
            printf("des consequences desastreuses! La moitie du pays\n");
            printf("est inondee, l'autre moitie brule. Les ours polaires\n");
            printf("ont engage un avocat pour vous poursuivre.\n");
            break;

        case 5:  // Fin normale (4 ans complets)
            printf("=== FIN DU MANDAT: MISSION ACCOMPLIE ===\n\n");
            printf("Vous avez survecu 4 ans a la presidence!\n");
            printf("Statistiques finales:\n");
            printf("  - Social: %d/100\n", player->gaugeSocial);
            printf("  - Economie: %d/100\n", player->gaugeEco);
            printf("  - Environnement: %d/100\n", player->gaugeEnviro);
            printf("  - Folie: %d/100\n\n", player->gaugeMadness);

            // Calcul d'un score basé sur les jauges
            int score = (player->gaugeSocial + player->gaugeEco +
                        player->gaugeEnviro + (100 - player->gaugeMadness)) / 4;

            printf("Score final: %d/100\n\n", score);

            if (score >= 80) {
                printf("Incroyable! Vous etes un president exemplaire!\n");
            } else if (score >= 60) {
                printf("Pas mal! Vous avez fait un mandat honorable.\n");
            } else if (score >= 40) {
                printf("Moyen... Vous survivez mais laissez des degats.\n");
            } else {
                printf("Catastrophique! Le pays est en ruine mais vous avez tenu 4 ans!\n");
            }
            break;

        default:
            printf("Fin inconnue...\n");
            break;
    }

    printf("\n===============================================\n");
    printf("         Merci d'avoir joue!                   \n");
    printf("===============================================\n\n");
}


// Sauvegarde TOUS les joueurs dans le fichier après chaque action
void saveProgression(PLAYER *list, char *nomFichier)
{
    if (list == NULL) {
        printf("Liste de joueurs vide, rien a sauvegarder.\n");
        return;
    }

    FILE *f = fopen(nomFichier, "w"); // Écrase le fichier
    if (f == NULL) {
        printf("Impossible de sauvegarder dans %s\n", nomFichier);
        return;
    }

    PLAYER *current = list;
    int count =0;

    while (current != NULL) {
        fprintf(f, "%d;%d;%d;%d;%d;%s;%s;%d;%s",
                current->idPlayer,
                current->gaugeEco,
                current->gaugeSocial,
                current->gaugeEnviro,
                current->gaugeMadness,
                current->firstName,
                current->lastName,
                current->temporality,
                current->password);

        // Ajoute le tableau progression
        for (int i = 0; i < NB_MAX_ETAPES; i++) {
            fprintf(f, ";%d", current->progression[i]);
        }

        fprintf(f, "\n");

        current = current->next;
        count++;
    }

    fclose(f);

    printf("Progression sauvegardee!\n");
}


//GROSSE fonction qui gere tout le gameplay du jeu
void gamePLay(PLAYER *player, ACTION *list,PLAYER *listPlayers, char *fichierSave) {
    if (player == NULL || list == NULL) {
        printf("joueur ou list d'actions invalide/vide.\n");
        return;
    }
printf("\n");
printf("######################################################################\n");
printf("##                     DEBUT DU MANDAT PRESIDENTIEL                 ##\n");
printf("######################################################################\n\n");
printf("  ⡅⢊⠔⣏⠒⣍⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣻⣟⣿⣻⢿⡿⣿⣻⣟⡿⣿\n");
printf("  ⡜⢠⠚⡬⡑⢬⠲⣹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣞⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣷⣿⣾⣿⣿\n");
printf("  ⡜⢢⠑⣢⢙⢢⠓⣬⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⣿⣿⣿⣽⣯⣷⣷⣾⣷⣿⣽⣿⣷⣾⣶⣯⣿\n");
printf("  ⢮⡑⣊⠴⣫⠬⣙⡮⢧⣹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⢿⣿⡿⢻⣛⢿⣛⠛⣟⢿⠿⡿⠿⡿⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⢦⠓⣌⢺⡕⢪⠵⣛⢦⡑⢾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⠋⠁⠁⠀⠈⠁⠂⠉⠀⡈⠄⡐⢈⢁⢂⠔⡔⠡⠀⠈⠈⠙⠻⣿⣿⣿⣿⣿⣿⣭⣯⣽⣭⣯⣷⣭⣿⣼⣷⣯⣿⣽⣿\n");
printf("  ⣌⠓⣤⡛⡼⢡⡟⡭⢒⡌⢇⡚⣿⣿⣿⣿⣿⣿⠏⠈⠀⠀⡀⠀⠀⠀⠀⠀⠈⠀⣀⠉⠀⠀⢘⡈⡢⠈⢎⠆⠃⢐⠀⠀⠀⠙⠽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣽⣿⣿⣿⣿\n");
printf("  ⣿⡖⠤⢹⠌⡳⡘⡥⢣⠜⣊⠵⡨⢿⣿⣿⠯⠁⠀⠀⣀⡠⢄⠤⠀⠀⠂⠀⠀⡁⠄⢀⠨⠄⡓⡼⠝⡳⡦⠑⡁⡀⠀⡀⠁⠀⠐⢋⢙⢌⣛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣧⢊⢲⡁⠳⣌⢣⠊⡔⢢⠑⢢⢻⣿⠋⠀⡠⣦⡏⠾⣡⢎⠱⡁⠁⠁⡀⢂⠁⠬⢝⠵⢚⠁⠣⢂⣹⠫⢬⠢⡐⢀⠀⠂⠁⠉⠊⠐⢠⠈⢿⣿⣿⣿⡿⣷⣿⣿⣾⢿⣽⡿⣿\n");
printf("  ⣿⣿⣿⣷⡢⢍⡱⢤⢃⠎⠰⡁⢎⠰⡎⠇⠐⡠⣣⣼⣽⢫⡗⠜⣁⠐⠀⠌⠀⠀⠀⠐⠀⠀⠘⢬⢻⠕⢿⡩⡥⠛⢒⠂⠂⡀⠀⡀⡂⠀⠀⠌⢺⣯⣽⣿⣿⣿⣿⣾⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣷⡃⠔⣊⠦⣘⠡⡜⢠⢻⣇⡀⢢⣮⣿⡿⢻⣁⣶⣩⡞⠄⡔⠀⠀⠀⠀⠀⠀⠀⠀⣀⢱⡙⢪⣷⣞⢯⡙⠠⠀⠔⠂⢀⠒⠔⣵⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⡔⣨⠖⣡⠏⡰⢡⣟⡿⠐⠿⣻⣿⣿⣏⣿⡿⢗⣭⠗⡈⠑⡀⠀⠀⠀⠀⠀⠚⠀⢌⣝⠗⡥⠿⣗⠌⠅⡓⠎⢔⡠⡛⡽⣚⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣄⢋⣆⢣⣱⣛⣾⡇⣴⣿⣛⣷⠯⣾⣷⣿⣿⡞⢽⠝⠊⠄⠀⠀⠀⠀⠀⠀⠁⠈⠀⢇⠰⠢⠁⠿⢚⣼⢟⣇⢟⡳⣯⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣆⠜⣢⠷⡝⣾⠏⡨⢢⣊⣭⣛⣟⣿⣾⣏⡻⢚⡁⠃⠀⠀⠄⠀⠀⠀⠀⠀⠀⢀⠈⠘⠀⠉⠁⠉⠠⣩⣾⠜⢻⣳⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⡜⡟⣱⣾⢹⢕⡷⣦⡏⢬⠋⡽⣞⣮⠡⣡⠐⡁⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠑⣒⡞⠨⡚⢯⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⡳⣼⠫⣟⣞⣿⡻⣑⠇⣄⢿⡼⣮⡙⠴⠼⠐⡂⠌⠩⠈⠉⠀⠀⠀⠀⠀⠈⠀⠀⠀⠀⠐⢀⢭⢛⢡⡸⣸⡏⣟⣻⡙⣏⡝⣫⠽⣩⢏⡽⢭⠯⡽⣹\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡹⡇⢡⡯⣿⡋⣣⢶⡽⡾⣽⣿⢳⣭⣀⣑⠛⢵⢀⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠊⠉⠔⡿⣼⢷⡿⣿⣾⣿⣿⣷⣿⣯⣿⣳⣯⣾⣟⣾⣷⣯\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡿⢰⣿⠯⡔⣿⢂⢿⣵⣿⣿⡞⠣⠥⠔⠫⡡⠠⠀⡀⢀⢀⠀⠀⠀⠠⠔⣤⢶⣂⡐⠊⣰⢿⡽⣸⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣏⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⢣⡇⠀⣿⡿⢃⣿⣿⣾⣟⣷⣱⣶⣶⣖⣖⣴⣉⢔⣦⣷⠀⢀⢷⣦⣾⠿⠿⠟⠘⣳⣰⢸⣷⣏⢸⣿⣿⠟⣿⣿⢏⢻⣿⡟⡙⢿⣿⡋⠹⣿\n");
printf("  ⡞⡜⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⢷⣸⣯⢹⡎⢿⡯⣿⡿⢿⣟⠑⠂⡀⠀⠀⢀⣽⢿⠣⠀⠀⠨⠦⠐⢀⠀⠀⢈⢡⢺⢿⣿⡇⣿⣿⡿⣛⠻⣟⢧⣫⠛⣽⣤⣻⢯⣳⣞⣽\n");
printf("  ⡽⡘⡜⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠿⢉⢻⡯⢾⣿⣿⡿⡽⣝⣲⠰⡀⠐⠂⢻⡷⢑⣪⢀⠀⢀⠈⠆⠐⠂⠀⢁⠎⠇⣿⠁⢱⣿⣿⣿⣿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣷⣎⡿\n");
printf("  ⣧⠱⢎⢏⡻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡴⠛⣿⣿⣿⣽⣿⣾⡮⣟⡓⠀⠡⠪⠹⣿⣿⣿⠄⠠⠈⡄⠀⠀⠀⠀⠠⠠⣌⣿⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣧⢛⠬⢆⠵⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⡗⣿⣿⣿⡿⣟⣿⣒⠆⠪⣁⡐⠱⣟⣽⡧⠊⠀⠀⡻⠠⠀⠀⠀⠀⢰⢡⢼⣿⣿⣿⣟⣯⣽⣿⣿⣿⣿⣳⣾⢿⣿⣿⣿⣿⣿\n");
printf("  ⣛⣎⠧⣋⢎⡳⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⠈⣧⣹⣿⣿⢷⡿⣾⣷⡲⠁⠀⢠⠠⣧⣽⡌⠠⢀⠠⡆⠁⡄⠀⠀⠠⠍⡤⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣧⣿⣿⣿⣿⣿⣿\n");
printf("  ⣟⡼⠳⣌⢞⡽⣻⢜⣿⣿⣿⣿⣿⣿⣿⡛⣽⡇⠀⠹⣿⣏⡿⣷⣿⣿⣿⠟⢂⣵⠯⣀⡛⠒⡛⠉⠀⠀⠂⡁⠐⡄⠀⢀⢾⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⢧⣛⠷⣨⢞⣽⢣⢞⡽⣿⣿⣿⢿⣳⣷⣽⣿⡇⠀⠀⠙⣷⣿⢿⣿⣿⣷⢬⠿⡁⣥⡈⠡⠯⠁⠀⠀⠀⠐⠈⠀⠀⠀⠈⣌⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⢧⣏⡳⢥⢾⣭⣳⣾⣻⣽⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⢿⣿⣿⣻⣯⠀⣤⣾⢫⠌⢉⠉⢉⠁⠉⠡⠀⠁⠂⠀⠀⢀⣾⣿⣿⣳⣿⣟⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⡳⣮⢷⣛⣽⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠻⣧⣿⢡⣼⡿⡸⣟⣶⣮⢖⢲⠒⣢⠠⡀⠀⠀⢠⢀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣷⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡃⠀⠀⠀⠀⠀⠈⠺⡙⠴⢿⣸⣿⡟⣷⡢⠐⠀⠅⣩⠐⣤⡶⡺⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠙⠻⣿⣦⡅⡙⠣⠠⢄⣄⠶⡿⠛⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠻⢿⣿⣿⢏⡊⠀⠀⠀⠀⠀⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡰⣟⡻⣟⣿⣿⣶⣄⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⣀⢺⣿⣽⣿⣜⣻⣿⡿⢁⠲⣂⠀⠀⠀⣘⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⡀⢦⢠⠻⣞⣿⣿⣷⣩⣿⠃⡬⢑⡈⡑⢇⡔⠰⣫⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣗⠀⠀⠠⠀⡐⡀⢆⣻⢳⣿⣿⣿⣼⣿⠡⠘⢆⠠⢁⠋⢶⢳⡒⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡶⢔⡁⠀⠀⢱⢪⡜⣯⣿⣿⣟⣿⣿⣧⡈⠌⠣⢌⡘⢢⠓⡜⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠈⡔⢃⠲⡄⢧⡙⣾⣿⣿⣿⣿⣿⣿⣷⡠⠁⠂⡔⠡⢣⠘⣸⣿⣿⣿⣿⣿⣿⣿⣟⠟⡝⣻⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠙⠦⡑⠜⡡⠃⣿⣿⣿⣿⣿⡿⣿⣿⣷⡀⠡⠌⣁⠪⡔⢨⣿⣿⣿⣿⣿⣿⣿⣿⣧⣜⣽⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣉⢮⡱⢌⡐⠡⣿⣿⣿⣿⣿⣿⣿⣷⡿⡇⠄⠡⢀⠒⡸⡀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡜⣦⠳⡰⢈⡐⣿⣿⣿⣿⣿⣿⣿⣾⣿⣿⡄⢁⠂⢌⡰⡑⢾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡖⢯⡑⢢⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡂⢁⢆⠲⣙⢾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣽⢧⡙⢦⣋⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣈⠬⡳⢬⣛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
printf("  ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣛⠶⣭⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⢚⡵⣫⣞⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n\n");


    //boucle de jeu : 48 mois
    while (player->temporality < 48) {

        printf("######################################################################\n");
        printf("                             MOIS %d / 48                             \n", player->temporality);
        printf("######################################################################\n");

        //verifie conditions de fin
        int endingType = checkGameOver(player);
        if (endingType != 0) {
            displayEnding(player, endingType);
            break;
        }

        // Affiche l'état actuel
        printf("Etat actuel:\n");
        printf("  Social: %d | Eco: %d | Enviro: %d | Folie: %d\n\n",
               player->gaugeSocial, player->gaugeEco,
               player->gaugeEnviro, player->gaugeMadness);

        ACTION *action1 = NULL;
        ACTION *action2 = NULL;
        initializeActions(list, &action1, &action2);

        if (action1 == NULL || action2 == NULL) {
            printf("erreur lors de la sélection des actions.\n");
            break;
        }

        //afficher les actions ( je ferai ptetre un truc plus sympa au niveau graph plus tard)
        printf("Choisissez quelle lois ou actions effectuer: \n\n");
        printf("\n [ 1 ]\n");
        vizualiseAction(action1);
        printf("\n [ 2 ]\n");
        vizualiseAction(action2);

        //choice joueur
        int choicePlayer = 0;
        printf("\n Quel sera votre choix ? (1 ou 2)");
        scanf("%d", &choicePlayer);

        while ((choicePlayer != 1) && (choicePlayer != 2)) {
            printf("Un effort s'il te plait! 1 OU 2 pas 34! \n");
            printf("Choisis entre 1 et 2 (tu sais compter jusqu'à deux ?) : ");
            getchar();
            scanf(" %d", &choicePlayer);
            
        }

        //application impact action choisi
        ACTION *actionChoosen;

        if (choicePlayer == 1) {
            actionChoosen = action1;
        } else {
            actionChoosen = action2;
        }
        realizeAction(player, actionChoosen);

        //sauvegarde de tout apres chaque action ! au moins c'est fait !
        saveProgression(listPlayers, fichierSave);

        printf("\nAppuyez sur Entree pour continuer...");
        getchar(); // Consomme le \n du scanf
        getchar(); // Attend l'entrée
    }

    if (player->temporality >= 48) {
        displayEnding(player, 5); // Fin reussi
    }

    printf("######################################################################\n");
    printf("      FIN DE LA PARTIE\n");
    printf("######################################################################\n");
}

