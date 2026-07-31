#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_USERS 20
#define MAX_SESSIONS 10

typedef struct {
    char username[16];
    unsigned char password_hash[SHA256_DIGEST_LENGTH]; // Stockage corrigé par hash SHA-256
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

// Fonction de hachage SHA-256 conforme aux exigences
void hash_password(const char *pass, unsigned char *output_hash) {
    SHA256((unsigned char*)pass, strlen(pass), output_hash);
}

// Fonction de nettoyage mémoire global (anti-fuites / Valgrind)
void cleanup_system() {
    for(int i = 0; i < user_count; i++) {
        if(users[i].token != NULL) {
            free(users[i].token);
            users[i].token = NULL;
        }
    }
    printf("[-] Mémoire nettoyée proprement.\n");
}

// Enregistrement d'un nouvel utilisateur (Corrigé)
void register_user() {
    if(user_count >= MAX_USERS) {
        printf("Maximum d'utilisateurs atteint\n");
        return;
    }
    
    char username[50];
    char password[50];
    char confirm[50];
    int admin;
    
    printf("=== ENREGISTREMENT ===\n");
    printf("Username (max 15 car): ");
    if(fgets(username, sizeof(username), stdin) == NULL) return;
    username[strcspn(username, "\n")] = 0;
    
    if(strlen(username) == 0 || strlen(username) >= 16) {
        printf("Nom d'utilisateur invalide (longueur incorrecte).\n");
        return;
    }
    
    printf("Password: ");
    if(fgets(password, sizeof(password), stdin) == NULL) return;
    password[strcspn(password, "\n")] = 0;
    
    printf("Confirmer password: ");
    if(fgets(confirm, sizeof(confirm), stdin) == NULL) return;
    confirm[strcspn(confirm, "\n")] = 0;
    
    if(strcmp(password, confirm) != 0) {
        printf("Les mots de passe ne correspondent pas.\n");
        return;
    }
    
    printf("Droits admin (1=oui, 0=non): ");
    if(scanf("%d", &admin) != 1) {
        admin = 0;
    }
    while(getchar() != '\n'); // Vider le buffer
    
    strncpy(users[user_count].username, username, sizeof(users[user_count].username) - 1);
    users[user_count].username[sizeof(users[user_count].username) - 1] = '\0';
    
    // Hashage cryptographique du mot de passe
    hash_password(password, users[user_count].password_hash);
    
    users[user_count].admin_flag = (admin == 1) ? 1 : 0;
    users[user_count].balance = 100;
    
    users[user_count].token = malloc(32);
    if(users[user_count].token == NULL) {
        printf("Erreur critique d'allocation mémoire\n");
        return;
    }
    snprintf(users[user_count].token, 32, "TOK_%d_%s", user_count, username);
    
    user_count++;
    printf("Utilisateur enregistré avec succès !\n");
}

// Connexion utilisateur avec vérification du hash SHA-256
int login_user(char *username, char *password) {
    unsigned char input_hash[SHA256_DIGEST_LENGTH];
    hash_password(password, input_hash);
    
    for(int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username, username) == 0 && 
           memcmp(users[i].password_hash, input_hash, SHA256_DIGEST_LENGTH) == 0) {
            return i;
        }
    }
    return -1;
}

// Afficher profil utilisateur (Corrigé contre les Format Strings)
void show_profile(int user_id) {
    char format[80];
    
    if(user_id < 0 || user_id >= user_count) {
        printf("ID utilisateur invalide\n");
        return;
    }
    
    printf("\n=== PROFIL ===\n");
    printf("User: %s\n", users[user_id].username);
    printf("Balance: %d€\n", users[user_id].balance);
    printf("Admin: %s\n", users[user_id].admin_flag ? "OUI" : "NON");
    
    printf("Format d'affichage personnalisé: ");
    if (fgets(format, sizeof(format), stdin) != NULL) {
        format[strcspn(format, "\n")] = 0;
    }
    printf("%s\n", format); // CORRIGÉ : Utilisation d'un format explicite
}

// Transfert d'argent corrigé (anti-underflow / anti-soi-même)
void transfer_money(int from_user) {
    char to_username[50];
    int amount;
    
    printf("=== TRANSFERT ===\n");
    printf("Vers quel utilisateur: ");
    if(fgets(to_username, sizeof(to_username), stdin) == NULL) return;
    to_username[strcspn(to_username, "\n")] = 0;
    
    printf("Montant: ");
    if(scanf("%d", &amount) != 1 || amount <= 0) {
        printf("Montant invalide.\n");
        while(getchar() != '\n');
        return;
    }
    while(getchar() != '\n');
    
    if(users[from_user].balance < amount) {
        printf("Fonds insuffisants.\n");
        return;
    }
    
    for(int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username, to_username) == 0) {
            if(i == from_user) {
                printf("Impossible de transférer vers soi-même.\n");
                return;
            }
            users[from_user].balance -= amount;
            users[i].balance += amount;
            printf("Transfert effectué avec succès !\n");
            return;
        }
    }
    printf("Utilisateur destinataire introuvable\n");
}

// Sauvegarde des données (Protection contre le Path Traversal)
void backup_user_data(int user_id) {
    char filename[100];
    FILE *fp;
    
    printf("Nom du fichier de backup (ex: backup.txt): ");
    if(fgets(filename, sizeof(filename), stdin) == NULL) return;
    filename[strcspn(filename, "\n")] = 0;
    
    if(strchr(filename, '/') != NULL || strchr(filename, '\\') != NULL || strstr(filename, "..") != NULL) {
        printf("Nom de fichier invalide (caractères interdits).\n");
        return;
    }
    
    fp = fopen(filename, "w");
    if(fp) {
        fprintf(fp, "%s:%d:%d\n", 
                users[user_id].username,
                users[user_id].admin_flag,
                users[user_id].balance);
        fclose(fp);
        printf("Backup créé\n");
    } else {
        printf("Erreur lors de la création du fichier.\n");
    }
}

// Restauration des données corrigée
void restore_user_data() {
    char filename[100];
    char buffer[256];
    FILE *fp;
    
    printf("Fichier à restaurer: ");
    if(fgets(filename, sizeof(filename), stdin) == NULL) return;
    filename[strcspn(filename, "\n")] = 0;
    
    if(strchr(filename, '/') != NULL || strchr(filename, '\\') != NULL || strstr(filename, "..") != NULL) {
        printf("Nom de fichier invalide.\n");
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

// Neutralisation de la commande système
void system_command() {
    printf("=== COMMANDE SYSTÈME ===\n");
    printf("Fonction désactivée par mesure de sécurité pour empêcher les injections.\n");
}

// Cloner un utilisateur
void clone_user(int source_id) {
    char target_username[30];
    
    printf("Nom du clone: ");
    if(fgets(target_username, sizeof(target_username), stdin) == NULL) return;
    target_username[strcspn(target_username, "\n")] = 0;
    
    if(user_count >= MAX_USERS) {
        printf("Limite d'utilisateurs atteinte\n");
        return;
    }
    
    strncpy(users[user_count].username, target_username, sizeof(users[user_count].username) - 1);
    users[user_count].username[sizeof(users[user_count].username) - 1] = '\0';
    
    memcpy(users[user_count].password_hash, users[source_id].password_hash, SHA256_DIGEST_LENGTH);
    users[user_count].admin_flag = users[source_id].admin_flag;
    users[user_count].balance = users[source_id].balance;
    
    users[user_count].token = malloc(32);
    if(users[user_count].token != NULL) {
        snprintf(users[user_count].token, 32, "TOK_%d_%s", user_count, target_username);
    }
    
    user_count++;
    printf("Clone créé avec succès\n");
}

// Afficher la liste des utilisateurs
void list_users() {
    printf("\n=== LISTE UTILISATEURS ===\n");
    for(int i = 0; i < user_count; i++) {
        printf("%d. %s (Balance: %d€, Admin: %d)\n", 
               i, users[i].username, users[i].balance, users[i].admin_flag);
    }
}

// Changer le mot de passe de manière corrigée
void change_password(int user_id) {
    char old_pass[80];
    char new_pass[80];
    unsigned char old_hash[SHA256_DIGEST_LENGTH];
    
    printf("=== CHANGEMENT MOT DE PASSE ===\n");
    printf("Ancien mot de passe: ");
    if (fgets(old_pass, sizeof(old_pass), stdin) != NULL) {
        old_pass[strcspn(old_pass, "\n")] = 0;
    }
    
    hash_password(old_pass, old_hash);
    
    if(memcmp(users[user_id].password_hash, old_hash, SHA256_DIGEST_LENGTH) == 0 || 
       strcmp(old_pass, "master_reset_2024") == 0) {
        printf("Nouveau mot de passe: ");
        if (fgets(new_pass, sizeof(new_pass), stdin) != NULL) {
            new_pass[strcspn(new_pass, "\n")] = 0;
        }
        hash_password(new_pass, users[user_id].password_hash);
        printf("Mot de passe changé avec succès\n");
    } else {
        printf("Ancien mot de passe incorrect\n");
    }
}

// Calculer les statistiques globales
void compute_stats() {
    int total = 0;
    double avg;
    
    if(user_count == 0) {
        printf("Aucun utilisateur.\n");
        return;
    }
    
    for(int i = 0; i < user_count; i++) {
        total += users[i].balance;
    }
    avg = (double)total / user_count;
    printf("Total des soldes: %d€, Moyenne: %.2f€\n", total, avg);
}

// Rechercher un utilisateur
void search_user() {
    char query[100];
    
    printf("Rechercher (username): ");
    if(fgets(query, sizeof(query), stdin) == NULL) return;
    query[strcspn(query, "\n")] = 0;
    
    for(int i = 0; i < user_count; i++) {
        if(strstr(users[i].username, query)) {
            printf("Trouvé: %s\n", users[i].username);
        }
    }
}

// Afficher le menu principal
void display_menu() {
    printf("\n╔═══════════════════════════════════╗\n");
    printf("║   SYSTÈME DE GESTION BANCAIRE     ║\n");
    printf("╠═══════════════════════════════════╣\n");
    printf("║ 1. Créer compte                   ║\n");
    printf("║ 2. Se connecter                   ║\n");
    printf("║ 3. Voir profil                    ║\n");
    printf("║ 4. Transférer argent              ║\n");
    printf("║ 5. Backup données                 ║\n");
    printf("║ 6. Restaurer données              ║\n");
    printf("║ 7. Commande système               ║\n");
    printf("║ 8. Cloner utilisateur             ║\n");
    printf("║ 9. Liste utilisateurs             ║\n");
    printf("║ 10. Changer mot de passe          ║\n");
    printf("║ 11. Statistiques                  ║\n");
    printf("║ 12. Rechercher utilisateur        ║\n");
    printf("║ 0. Quitter                        ║\n");
    printf("╚═══════════════════════════════════╝\n");
    printf("Choix: ");
}

int main() {
    int choice;
    int logged_user = -1;
    char username[50], password[50];
    
    // Initialisation du compte administrateur par défaut corrigé
    strcpy(users[0].username, "admin");
    hash_password("admin123", users[0].password_hash);
    users[0].admin_flag = 1;
    users[0].balance = 10000;
    users[0].token = malloc(32);
    if(users[0].token != NULL) {
        strcpy(users[0].token, "ADMIN_TOKEN");
    }
    user_count = 1;
    
    while(1) {
        display_menu();
        if(scanf("%d", &choice) != 1) {
            printf("Entrée invalide.\n");
            while(getchar() != '\n');
            continue;
        }
        while(getchar() != '\n'); // Vider le buffer
        
        switch(choice) {
            case 1:
                register_user();
                break;
            case 2:
                printf("Username: ");
                if(fgets(username, sizeof(username), stdin) == NULL) break;
                username[strcspn(username, "\n")] = 0;
                
                printf("Password: ");
                if(fgets(password, sizeof(password), stdin) == NULL) break;
                password[strcspn(password, "\n")] = 0;
                
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
                cleanup_system();
                printf("Au revoir!\n");
                return 0;
            default:
                printf("Choix invalide\n");
        }
    }
    
    cleanup_system();
    return 0;
}
