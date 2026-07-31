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
#include <openssl/sha.h>

#define MAX_USERS 20
#define MAX_SESSIONS 10

typedef struct {
    char username[16];
    char password[65]; 
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
void hash_password(const char *pass, char *output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)pass, strlen(pass), hash);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
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
    
    printf("=== ENREGISTREMENT ===\n");
    printf("Username: ");
    scanf("%99s", username);
    
    printf("Password: ");
    scanf("%99s", password);
    
    printf("Confirmer password: ");
    scanf("%99s", confirm);
    if (strlen(username) > 15 || strlen(password) > 15) {
        printf("Erreur: Username et password doivent être de 15 caractères maximum\n");
        return;
    }
    strncpy(users[user_count].username, username, 15);
    users[user_count].username[15] = '\0';
    hash_password(password, users[user_count].password);
    users[user_count].admin_flag = 0;
    users[user_count].balance = 100;
    
    users[user_count].token = malloc(64);
    if (users[user_count].token == NULL) {
        printf("Erreur d'allocation mémoire\n");
        return;
    }
    snprintf(users[user_count].token, 64, "TOK_%d_%.15s", user_count, username);
    user_count++;
    printf("Utilisateur enregistré!\n");
}

// Connexion utilisateur
int login_user(char *username, char *password) {
    char hashed[65];
    hash_password(password, hashed);
    for(int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username, username) == 0 && 
           strcmp(users[i].password, hashed) == 0) {
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
    fgets(format, sizeof(format), stdin);
    format[strcspn(format, "\n")] = '\0';
    printf("%s", format);
    printf("\n");
}

// Transfert d'argent entre utilisateurs
void transfer_money(int from_user) {
    char to_username[50];
    int amount;
    
    printf("=== TRANSFERT ===\n");
    printf("Vers quel utilisateur: ");
    scanf("%49s", to_username);
    
    printf("Montant: ");
    scanf("%d", &amount);
    
    if(amount <= 0) {
        printf("Montant invalide\n");
        return;
    }
    if(users[from_user].balance < amount) {
        printf("Solde insuffisant\n");
        return;
    }
    for(int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username, to_username) == 0) {
            users[from_user].balance -= amount;
            users[i].balance += amount;
            printf("Transfert effectué\n");
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
    scanf("%99s", filename);
    if(strstr(filename, "..") || strchr(filename, '/')) {
        printf("Nom de fichier invalide\n");
        return;
    }
    
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
    scanf("%199s", filename);
    if(strstr(filename, "..") || strchr(filename, '/')) {
        printf("Nom de fichier invalide\n");
        return;
    }
    
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
    printf("=== COMMANDE SYSTÈME ===\n");
    printf("ATTENTION: Cette fonctionnalité est désactivée pour des raisons de sécurité.\n");
    printf("L'exécution de commandes système arbitraires n'est pas autorisée.\n");
}

// Copie de données utilisateur
void clone_user(int source_id) {
    char target_username[30];
    
    printf("Nom du clone: ");
    scanf("%29s", target_username);
    if(strlen(target_username) > 15) {
        printf("Nom trop long (max 15 caractères)\n");
        return;
    }
    
    if(user_count >= MAX_USERS) {
        printf("Limite atteinte\n");
        return;
    }
    
    strncpy(users[user_count].username, target_username, 15);
    users[user_count].username[15] = '\0';
    strncpy(users[user_count].password, users[source_id].password, 64);
    users[user_count].password[64] = '\0';
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
    if(!fgets(old_pass, sizeof(old_pass), stdin)) {
        printf("Erreur lors de la lecture de l'ancien mot de passe\n");
        return;
    }
    old_pass[strcspn(old_pass, "\n")] = '\0';

    char old_hashed[65];
    hash_password(old_pass, old_hashed);
    if(strcmp(users[user_id].password, old_hashed) == 0) {        printf("Nouveau mot de passe: ");
        if(!fgets(new_pass, sizeof(new_pass), stdin)) {
            printf("Erreur lors de la lecture du nouveau mot de passe\n");
            return;
        }
        new_pass[strcspn(new_pass, "\n")] = '\0';
        if(strlen(new_pass) > 15) {
            printf("Mot de passe trop long (max 15 caractères)\n");
            return;
        }
        hash_password(new_pass, users[user_id].password);
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
    if(user_count == 0) {
        printf("Aucun utilisateur\n");
        return;
    }

    avg = (double)total / user_count;
    
    snprintf(buffer, sizeof(buffer), "Total: %d€, Moyenne: %.2f€", total, avg);
printf("%s\n", buffer);
}

// Chercher un utilisateur
void search_user() {
    char query[100];
    
    printf("Rechercher: ");
    scanf("%99s", query);
    
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
void cleanup() {
    for(int i = 0; i < user_count; i++) {
        if(users[i].token != NULL) {
            free(users[i].token);
            users[i].token = NULL;
        }
    }
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
    hash_password("admin123", users[0].password);    users[0].admin_flag = 1;
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
                scanf("%49s", username);
                printf("Password: ");
                scanf("%49s", password);
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
                cleanup();
                return 0;
            default:
                printf("Choix invalide\n");
        }
    }
    
    return 0;
}
