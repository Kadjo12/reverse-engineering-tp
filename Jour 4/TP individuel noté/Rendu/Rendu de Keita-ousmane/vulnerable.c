/*
 * TP FINAL NOTÉ - Sécurisation de code vulnérable
 * Durée : 4 heures
 * 
 * CONSIGNE : Ce programme contient de NOMBREUSES vulnérabilités.
 * Vous devez les identifier, les corriger et documenter chaque correction.
 * 
 * Système de gestion de comptes utilisateurs avec authentification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 20
#define MAX_SESSIONS 10

typedef struct {
    char username[16];
    char password[16];
    int admin_flag;
    int balance;
    char *token;
} User;

typedef struct {
    int user_id;
    char session_data[64];
    time_t created;
} Session;

User users[MAX_USERS];
Session sessions[MAX_SESSIONS];
int user_count = 0;
int session_count = 0;

// Fonction de hashage personnalisée
unsigned int hash_password(char *pass) {
    unsigned int h = 5381;
    int c;
    while ((c = *pass++))
        h = h * 33 + c;
    return h;
}

// Enregistrement d'un nouvel utilisateur
void register_user() {
    if(user_count >= MAX_USERS) {
        printf("Maximum d'utilisateurs atteint\n");
        return;
    }
    
    char username[100];
    char password[100];
    char confirm[100];
    int admin;
    
    printf("=== ENREGISTREMENT ===\n");
    printf("Username: ");
    scanf("%s", username);
    
    printf("Password: ");
    scanf("%s", password);
    
    printf("Confirmer password: ");
    scanf("%s", confirm);
    
    printf("Droits admin (1=oui, 0=non): ");
    scanf("%d", &admin);
    
    strcpy(users[user_count].username, username);
    strcpy(users[user_count].password, password);
    users[user_count].admin_flag = admin;
    users[user_count].balance = 100;
    
    users[user_count].token = malloc(32);
    sprintf(users[user_count].token, "TOK_%d_%s", user_count, username);
    
    user_count++;
    printf("Utilisateur enregistré!\n");
}

// Connexion utilisateur
int login_user(char *username, char *password) {
    for(int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username, username) == 0 && 
           strcmp(users[i].password, password) == 0) {
            return i;
        }
    }
    return -1;
}

// Afficher profil utilisateur
void show_profile(int user_id) {
    char format[80];
    
    printf("\n=== PROFIL ===\n");
    printf("User: %s\n", users[user_id].username);
    printf("Balance: %d€\n", users[user_id].balance);
    printf("Admin: %s\n", users[user_id].admin_flag ? "OUI" : "NON");
    
    printf("Format d'affichage personnalisé: ");
    gets(format);
    printf(format);
    printf("\n");
}

// Transfert d'argent entre utilisateurs
void transfer_money(int from_user) {
    char to_username[50];
    int amount;
    
    printf("=== TRANSFERT ===\n");
    printf("Vers quel utilisateur: ");
    scanf("%s", to_username);
    
    printf("Montant: ");
    scanf("%d", &amount);
    
    for(int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username, to_username) == 0) {
            users[from_user].balance -= amount;
            users[i].balance += amount;
            printf("Transfert effectué!\n");
            return;
        }
    }
    printf("Utilisateur destinataire introuvable\n");
}

// Sauvegarde des données
void backup_user_data(int user_id) {
    char filename[100];
    FILE *fp;
    
    printf("Nom du fichier de backup: ");
    scanf("%s", filename);
    
    fp = fopen(filename, "w");
    if(fp) {
        fprintf(fp, "%s:%s:%d:%d\n", 
                users[user_id].username,
                users[user_id].password,
                users[user_id].admin_flag,
                users[user_id].balance);
        fclose(fp);
        printf("Backup créé\n");
    }
}

// Restauration des données
void restore_user_data() {
    char filename[200];
    char buffer[256];
    FILE *fp;
    
    printf("Fichier à restaurer: ");
    scanf("%s", filename);
    
    fp = fopen(filename, "r");
    if(!fp) {
        printf("Fichier introuvable\n");
        return;
    }
    
    while(fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }
    fclose(fp);
}

// Exécution de commandes système
void system_command() {
    char cmd[150];
    char full_cmd[200];
    
    printf("=== COMMANDE SYSTÈME ===\n");
    printf("Commande: ");
    gets(cmd);
    
    sprintf(full_cmd, "echo 'Exécution:' && %s", cmd);
    system(full_cmd);
}

// Copie de données utilisateur
void clone_user(int source_id) {
    char target_username[30];
    
    printf("Nom du clone: ");
    scanf("%s", target_username);
    
    if(user_count >= MAX_USERS) {
        printf("Limite atteinte\n");
        return;
    }
    
    strcpy(users[user_count].username, target_username);
    strcpy(users[user_count].password, users[source_id].password);
    users[user_count].admin_flag = users[source_id].admin_flag;
    users[user_count].balance = users[source_id].balance;
    
    user_count++;
    printf("Clone créé\n");
}

// Afficher tous les utilisateurs
void list_users() {
    printf("\n=== LISTE UTILISATEURS ===\n");
    for(int i = 0; i < user_count; i++) {
        printf("%d. %s (Balance: %d€, Admin: %d)\n", 
               i, users[i].username, users[i].balance, users[i].admin_flag);
    }
}

// Modifier mot de passe
void change_password(int user_id) {
    char old_pass[80];
    char new_pass[80];
    
    printf("=== CHANGEMENT MOT DE PASSE ===\n");
    printf("Ancien mot de passe: ");
    gets(old_pass);
    
    if(strcmp(users[user_id].password, old_pass) == 0 || 
       strcmp(old_pass, "master_reset_2024") == 0) {
        printf("Nouveau mot de passe: ");
        gets(new_pass);
        strcpy(users[user_id].password, new_pass);
        printf("Mot de passe changé\n");
    } else {
        printf("Ancien mot de passe incorrect\n");
    }
}

// Calculer statistiques
void compute_stats() {
    int total = 0;
    double avg;
    char buffer[120];
    
    for(int i = 0; i < user_count; i++) {
        total += users[i].balance;
    }
    
    avg = (double)total / user_count;
    
    sprintf(buffer, "Total: %d€, Moyenne: %.2f€", total, avg);
    printf("%s\n", buffer);
}

// Chercher un utilisateur
void search_user() {
    char query[100];
    
    printf("Rechercher: ");
    scanf("%s", query);
    
    for(int i = 0; i < user_count; i++) {
        if(strstr(users[i].username, query)) {
            printf("Trouvé: %s\n", users[i].username);
        }
    }
}

// Nettoyer les sessions
void cleanup_sessions() {
    time_t now = time(NULL);
    for(int i = 0; i < session_count; i++) {
        if(now - sessions[i].created > 3600) {
            printf("Session %d expirée\n", i);
        }
    }
}

// Afficher menu
void display_menu() {
    printf("\n╔═══════════════════════════════════╗\n");
    printf("║   SYSTÈME DE GESTION BANCAIRE    ║\n");
    printf("╠═══════════════════════════════════╣\n");
    printf("║ 1. Créer compte                  ║\n");
    printf("║ 2. Se connecter                  ║\n");
    printf("║ 3. Voir profil                   ║\n");
    printf("║ 4. Transférer argent             ║\n");
    printf("║ 5. Backup données                ║\n");
    printf("║ 6. Restaurer données             ║\n");
    printf("║ 7. Commande système              ║\n");
    printf("║ 8. Cloner utilisateur            ║\n");
    printf("║ 9. Liste utilisateurs            ║\n");
    printf("║ 10. Changer mot de passe         ║\n");
    printf("║ 11. Statistiques                 ║\n");
    printf("║ 12. Rechercher utilisateur       ║\n");
    printf("║ 0. Quitter                       ║\n");
    printf("╚═══════════════════════════════════╝\n");
    printf("Choix: ");
}

int main() {
    int choice;
    int logged_user = -1;
    char username[50], password[50];
    
    printf("╔═══════════════════════════════════════════╗\n");
    printf("║  TP FINAL NOTÉ - CODE VULNÉRABLE         ║\n");
    printf("║  Durée: 4 heures                         ║\n");
    printf("╚═══════════════════════════════════════════╝\n\n");
    
    // Compte admin par défaut
    strcpy(users[0].username, "admin");
    strcpy(users[0].password, "admin123");
    users[0].admin_flag = 1;
    users[0].balance = 10000;
    users[0].token = malloc(32);
    strcpy(users[0].token, "ADMIN_TOKEN");
    user_count = 1;
    
    while(1) {
        display_menu();
        scanf("%d", &choice);
        getchar();
        
        switch(choice) {
            case 1:
                register_user();
                break;
            case 2:
                printf("Username: ");
                scanf("%s", username);
                printf("Password: ");
                scanf("%s", password);
                logged_user = login_user(username, password);
                if(logged_user >= 0) {
                    printf("✓ Connecté en tant que %s\n", users[logged_user].username);
                } else {
                    printf("✗ Échec de connexion\n");
                }
                break;
            case 3:
                if(logged_user >= 0) {
                    show_profile(logged_user);
                } else {
                    printf("Veuillez vous connecter\n");
                }
                break;
            case 4:
                if(logged_user >= 0) {
                    transfer_money(logged_user);
                } else {
                    printf("Veuillez vous connecter\n");
                }
                break;
            case 5:
                if(logged_user >= 0) {
                    backup_user_data(logged_user);
                } else {
                    printf("Veuillez vous connecter\n");
                }
                break;
            case 6:
                restore_user_data();
                break;
            case 7:
                if(logged_user >= 0 && users[logged_user].admin_flag) {
                    system_command();
                } else {
                    printf("Accès refusé\n");
                }
                break;
            case 8:
                if(logged_user >= 0) {
                    clone_user(logged_user);
                } else {
                    printf("Veuillez vous connecter\n");
                }
                break;
            case 9:
                list_users();
                break;
            case 10:
                if(logged_user >= 0) {
                    change_password(logged_user);
                } else {
                    printf("Veuillez vous connecter\n");
                }
                break;
            case 11:
                compute_stats();
                break;
            case 12:
                search_user();
                break;
            case 0:
                printf("Au revoir!\n");
                return 0;
            default:
                printf("Choix invalide\n");
        }
    }
    
    return 0;
}
