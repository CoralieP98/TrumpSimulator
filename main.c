#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/// define ///

#define MAX_DESCRP 100
#define MAX_NOM 20

#define LINE_SIZE 1000
#define NOM_FICHIER_TXT_ETAPES "STRUCT.txt"
#define NOM_FICHIER_TXT_PLAYERS "PLAYERS.txt"

#define NB_MAX_ETAPES 49

/// structures ///

typedef struct action {
    int idAction;
    int impSocial;
    int impEco;
    int impEnviro;
    int madness;
    char description[MAX_DESCRP];
    char impDescription[MAX_DESCRP];
    struct action *next;
}ACTION;

typedef struct player {
    int idPlayer;
    char firstName[];
    char lastName[7];

    int gaugeSocial;
    int gaugeEco;
    int gaugeEnviro;
    int gaugeMadness;

    int progression[49];

    int temporality;

    char password[15];
    struct player *next;

}PLAYER;


/// Prototypes ///
void printMenu(void);
void displayAction(ACTION *action);
void afficheListe(ACTION *list);

void initializePlayer(PLAYER *player);
void saveProgression(PLAYER *player, FILE *historique);
ACTION *chargerTxt(char *nomFichier);
ACTION *recupererLigne(char *line);
ACTION *insertionAlphaActions(ACTION *list, ACTION *newAction);

void displayPlayerStatus(PLAYER *player);
PLAYER *chargerJoueurTxt(char *nomFichier);
PLAYER *recupererLigneJoueur(char *line);
PLAYER *insertionAlphaJoueur(PLAYER *list, PLAYER *newPlayer);

int initializeActions(ACTION *action1, ACTION *action2);
void vizualiseAction(ACTION *action,int *idAction1, int *idAction2);
int chooseAction(ACTION *action,int *idAction1, int *idAction2);
void realizeAction(ACTION *action ,int choosenAction);
void vizualiseConsequence(ACTION *action, int choosenAction);

int checkGameOver(PLAYER *player);
void displayEnding(PLAYER *player, int endingType);


int main(void) {

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
                break;
            case 2:
                break;
            case 3:
                afficheListe(list);
                break;
            case 4:

                break;
            case 0:
                end = 1;
                break;
            default:
                break;
        }
    }

    return 0;
}

void printMenu(void)
{
    printf("#### TRUMP SIMULATOR ####\n");
    printf("\n\n\nThis is the Fake News MENU\n");
    printf("#########################\n");
    printf("1 - PLAY\n");
    printf("2 - New Game\n");
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

        /* on avance vers l'STEPent suivant */
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
            player = insertionAlphaJoueur(player, new);
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
    char *token = strtok(line, separator); // eco (no id field)
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


//Insertion des strucutures joueur par ordre alphabétique
PLAYER *insertionAlphaJoueur(PLAYER *list, PLAYER *newPlayer)
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
        if (courant->idPlayer, newPlayer->idPlayer)
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








void initializePlayer(PLAYER *player) {
    printf("=== CREATION DE JOUEUR ===\n");

    printf("Entrez un prenom : ");
    scanf("%s", player->firstName);

    *player->lastName ="Trump";
    player->temporality = 0;
    // printf("Entrez votre nom : ");
    // scanf("%s", player->lastName);

    printf("Entrez un mot de passe : ");
    scanf("%s", player->password);

    player->gaugeSocial = 50;
    player->gaugeEco = 50;
    player->gaugeEnviro = 50;
    player->gaugeMadness = 15;

    // ID aléatoire pour le joueur
    player->idPlayer = rand() % 10000;
    printf("\nJoueur cree avec succes ! ID: %d\n\n", player->idPlayer);
}

// Sauvegarde la progression du joueur dans un fichier
void saveProgression(PLAYER *player, FILE *historique) {
    fprintf(historique, "%d,%s,%s,", player->idPlayer, player->firstName, player->lastName);
    fprintf(historique, "%d,%d,%d,%d\n",
            player->gaugeSocial,
            player->gaugeEco,
            player->gaugeEnviro,
            player->gaugeMadness);

    printf("Progression sauvegardee !\n");
}

// Initialise deux actions aléatoires et retourne leurs IDs
// Pour l'instant, on retourne juste deux IDs aléatoires
int initializeActions(ACTION *action1, ACTION *action2) {
    // Génère deux IDs différents entre 1 et 20
    int id1 = (rand() % 20) + 1;
    int id2 = (rand() % 20) + 1;

    // S'assure que les deux actions sont différentes
    while (id2 == id1) {
        id2 = (rand() % 20) + 1;
    }

    action1->idAction = id1;
    action2->idAction = id2;

    return 0; // Retourne 0 si succès
}

// Affiche les deux actions au joueur
void vizualiseAction(ACTION *action1, ACTION *action2) {
    printf("\n=== CHOIX D'ACTIONS ===\n\n");

    printf("ACTION 1 (ID: %d)\n", action1->idAction);
    printf("Description: %s\n", action1->description);


    printf("ACTION 2 (ID: %d)\n", action2->idAction);
    printf("Description: %s\n", action2->description);

}

// Demande au joueur de choisir une action et retourne l'ID choisi
int chooseAction(ACTION *action1, ACTION *action2) {
    int choix;

    printf("Quelle action choisissez-vous ? (1 ou 2) : ");
    scanf("%d", &choix);

    // Validation du choix
    while (choix != 1 && choix != 2) {
        printf("Choix invalide ! Entrez 1 ou 2 : ");
        scanf("%d", &choix);
    }

    // Retourne l'ID de l'action choisie
    if (choix == 1) {
        return action1->idAction;
    } else {
        return action2->idAction;
    }
}

// Applique les impacts de l'action choisie sur les jauges du joueur
void realizeAction(PLAYER *player, ACTION *action) {
    printf("\n=== APPLICATION DE L'ACTION ===\n");

    // Applique les modificateurs
    player->gaugeSocial += action->impSocial;
    player->gaugeEco += action->impEco;
    player->gaugeEnviro += action->impEnviro;
    player->gaugeMadness + action->madness;
    player->temporality += 1;

    // Limite les jauges entre 0 et 100
    if (player->gaugeSocial < 0) player->gaugeSocial = 0;
    if (player->gaugeSocial > 100) player->gaugeSocial = 100;

    if (player->gaugeEco < 0) player->gaugeEco = 0;
    if (player->gaugeEco > 100) player->gaugeEco = 100;

    if (player->gaugeEnviro < 0) player->gaugeEnviro = 0;
    if (player->gaugeEnviro > 100) player->gaugeEnviro = 100;

    if (player->gaugeMadness < 0) player->gaugeMadness = 0;
    if (player->gaugeMadness > 100) player->gaugeMadness = 100;

    printf("Jauges mises a jour !\n");
}

// Affiche les conséquences de l'action choisie
void vizualiseConsequence(ACTION *action) {
    printf("\n=== CONSEQUENCES ===\n");
    printf("%s\n", action->impDescription);

    printf("\nModifications appliquees:\n");
    if (action->impSocial > 0) printf("  Social: +%d\n", action->impSocial);
    else if (action->impSocial < 0) printf("  Social: %d\n", action->impSocial);

    if (action->impEco > 0) printf("  Economie: +%d\n", action->impEco);
    else if (action->impEco < 0) printf("  Economie: %d\n", action->impEco);

    if (action->impEnviro > 0) printf("  Environnement: +%d\n", action->impEnviro);
    else if (action->impEnviro < 0) printf("  Environnement: %d\n", action->impEnviro);

    if (action->madness > 0) printf("  Folie: +%d\n", action->madness);
    else if (action->madness < 0) printf("  Folie: %d\n", action->madness);
}

// Fonction utile pour afficher l'état actuel du joueur
void displayPlayerStatus(PLAYER *player) {

    PLAYER *current = player;
    if (player == NULL) {
        printf("affiche statut joueur impossible sur liste vide\n");
        return;
    }
    while (current != NULL) {
        printf("\n=== ETAT ACTUEL ===\n");
        printf("Joueur: %s %s\n", player->firstName, player->lastName);
        printf("Social:       [%d/100]\n", player->gaugeSocial);
        printf("Economie:     [%d/100]\n", player->gaugeEco);
        printf("Environement: [%d/100]\n", player->gaugeEnviro);
        printf("Folie:        [%d/100]\n\n", player->gaugeMadness);
        for(int i = 0; i < 49; i++) {
            printf("avancement[%d]=%d ",i,current->progression[i]);
        }
        current = current->next;
    }
}


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

//


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
