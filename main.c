#include <stdio.h>
#include <stdlib.h>

/// structures ///

typedef struct action {
    int idAction;
    int impSocial;
    int impEco;
    int impEnviro;
    int madness;
    char description[100];
    char impDescription[50];

}ACTION;

typedef struct player {
    int idPlayer;
    char firstName[30];
    char lastName[7];

    int gaugeSocial;
    int gaugeEco;
    int gaugeEnviro;
    int gaugeMadness;

    int temporality;

    char password[15];
}PLAYER;


/// Prototypes ///

void initializePlayer(PLAYER *player);
void saveProgression(PLAYER *player, FILE *historique);

void displayPlayerStatus(PLAYER *player);

int initializeActions(ACTION *action1, ACTION *action2);
void vizualiseAction(ACTION *action,int *idAction1, int *idAction2);
int chooseAction(ACTION *action,int *idAction1, int *idAction2);
void realizeAction(ACTION *action ,int choosenAction);
void vizualiseConsequence(ACTION *action, int choosenAction);

int checkGameOver(PLAYER *player);
void displayEnding(PLAYER *player, int endingType);


int main(void) {

    ACTION *list ;
    PLAYER *Trump;

    int choice;
    int end = 0;

    while (end != -1){
        printf("#### TRUMP SIMULATOR ####\n");
        printf("#########################\n");
        printf(" 1 - Nouvelle Partie ----\n");
        printf(" 2 - Charger ------------\n");
        printf(" 3 - Quitter ------------\n");
        printf("#########################\n");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            default:
                break;
        }
    }

    return 0;
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
    printf("\n=== ETAT ACTUEL ===\n");
    printf("Joueur: %s %s\n", player->firstName, player->lastName);
    printf("Social:       [%d/100]\n", player->gaugeSocial);
    printf("Economie:     [%d/100]\n", player->gaugeEco);
    printf("Environement: [%d/100]\n", player->gaugeEnviro);
    printf("Folie:        [%d/100]\n\n", player->gaugeMadness);
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
        //FIN 5: si il finit son mandat = u won!
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
