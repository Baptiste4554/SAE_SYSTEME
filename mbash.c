// on veut faire un terminal bash qui comprend la focntion cd, pwd, exit pour quitter.
// on veut aussi rajouter historique,
// on veut aussi essayer de faire complétion 

#include <stdio.h>   
#include <stdlib.h> 
#include <unistd.h>    
#include <string.h>    
#include <sys/wait.h>  

#define TAILLE_BUFFER 1024 // Taille maximale pour les buffers de chaînes

// Fonction pour exécuter une commande
void executerCommande(char **args, int arrierePlan) {
    pid_t pid = fork(); // Création d'un processus enfant
    if (pid == 0) { // Si on est dans le processus enfant
        if (execvp(args[0], args) == -1) { // Exécute la commande
            perror("mbash"); // Affiche une erreur si la commande échoue
        }
        exit(EXIT_FAILURE); // Termine le processus enfant en cas d'échec
    } else if (pid < 0) { // Si fork échoue
        perror("mbash"); // Affiche une erreur
    } else { // Si on est dans le processus parent
        if (!arrierePlan) { // Si la commande n'est pas en arrière-plan
            wait(NULL); // Attend que le processus enfant se termine
        }
    }
}

// Fonction pour changer de répertoire (commande "cd")
void changerRepertoire(char **args) {
    if (args[1] == NULL) { // Si aucun argument n'est fourni à "cd"
        fprintf(stderr, "mbash: argument attendu pour \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) { // Change le répertoire courant
            perror("mbash"); // Affiche une erreur si "cd" échoue
        }
    }
}

// Fonction pour afficher le répertoire courant (commande "pwd")
void afficherRepertoireCourant() {
    char cwd[TAILLE_BUFFER]; // Buffer pour stocker le chemin du répertoire courant
    if (getcwd(cwd, sizeof(cwd)) != NULL) { // Récupère le chemin du répertoire courant
        printf("%s\n", cwd); // Affiche le chemin
    } else {
        perror("mbash"); // Affiche une erreur si getcwd échoue
    }
}

// Fonction pour traiter l'entrée utilisateur
void traiterEntree(char *entree, char **args, int *arrierePlan) {
    char *token; // Pointeur pour extraire les arguments
    *arrierePlan = 0; // Initialise le drapeau pour l'exécution en arrière-plan
    int i = 0; // Index pour remplir le tableau des arguments

    token = strtok(entree, " "); // Divise la chaîne d'entrée en tokens (arguments)
    while (token != NULL) { // Tant qu'il y a des tokens
        args[i++] = token; // Ajoute le token au tableau des arguments
        token = strtok(NULL, " "); // Passe au token suivant
    }
    args[i] = NULL; // Termine le tableau des arguments par NULL

    if (i > 0 && strcmp(args[i - 1], "&") == 0) { // Vérifie si le dernier argument est "&"
        *arrierePlan = 1; // Active le mode arrière-plan
        args[i - 1] = NULL; // Supprime "&" du tableau des arguments
    }
}

int main() {
    char buffer[TAILLE_BUFFER]; // Buffer pour stocker l'entrée utilisateur
    char *args[TAILLE_BUFFER / 2 + 1]; // Tableau pour stocker les arguments
    int arrierePlan; // Drapeau pour indiquer si une commande est en arrière-plan

    while (1) { // Boucle principale du shell
        char cwd[TAILLE_BUFFER]; // Buffer pour le chemin du répertoire courant
        if (getcwd(cwd, sizeof(cwd)) != NULL) { // Récupère le chemin du répertoire courant
            printf("%s mbash> ", cwd); // Affiche le prompt avec le répertoire courant
        } else {
            printf("mbash> "); // Affiche un prompt générique si getcwd échoue
        }
        fflush(stdout); // Vide le buffer de sortie pour afficher immédiatement le prompt

        if (!fgets(buffer, TAILLE_BUFFER, stdin)) { // Lit l'entrée utilisateur
            break; // Quitte la boucle si fgets retourne NULL 
        }

        buffer[strcspn(buffer, "\n")] = '\0'; // Remplace le caractère de nouvelle ligne par '\0'
        traiterEntree(buffer, args, &arrierePlan); // Traite l'entrée utilisateur

        if (args[0] == NULL) { // Si aucune commande n'est entrée
            continue; // Recommence la boucle
        }

        if (strcmp(args[0], "exit") == 0) { // Si l'utilisateur entre "exit"
            printf("Au revoir !\n");
            break; // Quitte la boucle et termine le programme
        } else if (strcmp(args[0], "cd") == 0) { // Si l'utilisateur entre "cd"
            changerRepertoire(args); // Change de répertoire
        } else if (strcmp(args[0], "pwd") == 0) { // Si l'utilisateur entre "pwd"
            afficherRepertoireCourant(); // Affiche le répertoire courant
        } else { // Pour toutes les autres commandes
            executerCommande(args, arrierePlan); // Exécute la commande
        }
    }

    return 0; // Termine le programme
}
