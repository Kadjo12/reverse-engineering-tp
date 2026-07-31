/*
 * Programme Vulnérable pour TP Dump Mémoire
 * ATTENTION: Contient volontairement des failles de sécurité à des fins éducatives
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX_USERS 10
#define BUFFER_SIZE 256

// Structure pour stocker des credentials
typedef struct {
    char username[50];
    char password[50];
    char email[100];
    int is_admin;
} User;

// Structure pour les données sensibles
typedef struct {
    char api_key[100];
    char secret_token[100];
    char database_url[200];
} SensitiveData;

// Données globales (vulnérabilité: stockage en clair)
User users[MAX_USERS];
SensitiveData sensitive;
char session_cookie[256];
int user_count = 0;

// Fonction pour initialiser des données "sensibles"
void init_sensitive_data() {
    strcpy(sensitive.api_key, "API_KEY_12345_SUPER_SECRET_ABCDEF");
    strcpy(sensitive.secret_token, "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.secret");
    strcpy(sensitive.database_url, "mysql://admin:P@ssw0rd123@192.168.1.100:3306/production_db");
    strcpy(session_cookie, "SESSION_ID=a1b2c3d4e5f6; token=secret_session_token_xyz");
}

// Fonction pour ajouter des utilisateurs
void add_user(const char* username, const char* password, const char* email, int is_admin) {
    if (user_count < MAX_USERS) {
        strcpy(users[user_count].username, username);
        strcpy(users[user_count].password, password);
        strcpy(users[user_count].email, email);
        users[user_count].is_admin = is_admin;
        user_count++;
        printf("[+] User added: %s\n", username);
    }
}

// Fonction pour afficher les utilisateurs (sans les passwords, mais ils sont en mémoire!)
void list_users() {
    printf("\n=== Liste des utilisateurs ===\n");
    for (int i = 0; i < user_count; i++) {
        printf("User %d: %s (%s) - Admin: %s\n", 
               i+1, 
               users[i].username, 
               users[i].email,
               users[i].is_admin ? "Yes" : "No");
    }
    printf("==============================\n\n");
}

// Fonction vulnérable avec buffer sur la stack
void vulnerable_input() {
    char buffer[64];  // Buffer limité
    char credit_card[20] = "4532-1234-5678-9010";
    
    printf("\n[INPUT] Entrez votre message (vulnérable): ");
    fflush(stdout);
    
    // Vulnérabilité: pas de limite de taille!
    gets(buffer);  // DANGEREUX: deprecated et vulnérable
    
    printf("[OUTPUT] Vous avez dit: %s\n", buffer);
    printf("[DEBUG] Numéro de carte en mémoire locale: %s\n", credit_card);
}

// Fonction pour simuler une connexion réseau
void simulate_network_activity() {
    char request[512];
    char response[512];
    
    sprintf(request, "POST /api/login HTTP/1.1\r\n"
                     "Host: secure-banking.com\r\n"
                     "Authorization: %s\r\n"
                     "Content-Type: application/json\r\n"
                     "\r\n"
                     "{\"username\":\"admin\",\"password\":\"SuperSecret123!\"}",
                     sensitive.secret_token);
    
    sprintf(response, "HTTP/1.1 200 OK\r\n"
                      "Set-Cookie: %s\r\n"
                      "Content-Type: application/json\r\n"
                      "\r\n"
                      "{\"status\":\"success\",\"session\":\"active\"}",
                      session_cookie);
    
    printf("[NETWORK] Requête envoyée (simulée)\n");
    printf("[NETWORK] Réponse reçue (simulée)\n");
}

// Fonction pour créer des données intéressantes en mémoire
void create_interesting_data() {
    // Allouer de la mémoire dynamique avec des données sensibles
    char* heap_data = (char*)malloc(512);
    sprintf(heap_data, 
            "SECRET_KEY=my_super_secret_key_456\n"
            "AWS_ACCESS_KEY=AKIAIOSFODNN7EXAMPLE\n"
            "AWS_SECRET_KEY=wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY\n"
            "PRIVATE_KEY_PATH=/home/user/.ssh/id_rsa\n"
            "CREDIT_CARD=5555-5555-5555-4444\n"
            "CVV=123\n");
    
    printf("[DEBUG] Données sensibles créées en mémoire heap\n");
    
    // On ne libère PAS la mémoire volontairement (memory leak)
    // free(heap_data);
}

// Menu interactif
void display_menu() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║   Application Vulnérable - Menu       ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("  [PID: %d]\n", getpid());
    printf("1. Lister les utilisateurs\n");
    printf("2. Ajouter un utilisateur\n");
    printf("3. Test d'entrée vulnérable\n");
    printf("4. Simuler activité réseau\n");
    printf("5. Créer des données sensibles\n");
    printf("6. Afficher les données sensibles (DEBUG)\n");
    printf("7. Attendre (pour faire le dump)\n");
    printf("0. Quitter\n");
    printf("Choix: ");
    fflush(stdout);
}

void main_loop() {
    int choice;
    char username[50], password[50], email[100];
    int is_admin;
    
    while (1) {
        display_menu();
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); // Nettoyer le buffer
            continue;
        }
        getchar(); // Consommer le \n
        
        switch(choice) {
            case 1:
                list_users();
                break;
                
            case 2:
                printf("Username: ");
                fgets(username, sizeof(username), stdin);
                username[strcspn(username, "\n")] = 0;
                
                printf("Password: ");
                fgets(password, sizeof(password), stdin);
                password[strcspn(password, "\n")] = 0;
                
                printf("Email: ");
                fgets(email, sizeof(email), stdin);
                email[strcspn(email, "\n")] = 0;
                
                printf("Admin? (1/0): ");
                scanf("%d", &is_admin);
                getchar();
                
                add_user(username, password, email, is_admin);
                break;
                
            case 3:
                vulnerable_input();
                break;
                
            case 4:
                simulate_network_activity();
                break;
                
            case 5:
                create_interesting_data();
                break;
                
            case 6:
                printf("\n=== DONNÉES SENSIBLES (DEBUG) ===\n");
                printf("PID du processus: %d\n", getpid());
                printf("─────────────────────────────────\n");
                printf("API Key: %s\n", sensitive.api_key);
                printf("Token: %s\n", sensitive.secret_token);
                printf("Database: %s\n", sensitive.database_url);
                printf("Cookie: %s\n", session_cookie);
                printf("=================================\n");
                printf("\n💡 Pour dumper ce processus:\n");
                printf("   sudo gcore -o dump %d\n", getpid());
                break;
                
            case 7:
                printf("\n[*] Programme en attente...\n");
                printf("[*] PID: %d\n", getpid());
                printf("[*] C'est le moment de faire le dump!\n");
                printf("[*] Commande: sudo gcore -o dump %d\n", getpid());
                printf("[*] Appuyez sur Entrée pour continuer...\n");
                getchar();
                break;
                
            case 0:
                printf("Au revoir!\n");
                return;
                
            default:
                printf("Choix invalide!\n");
        }
    }
}

int main() {
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║  Application Vulnérable pour TP Dump Mémoire  ║\n");
    printf("║  ATTENTION: À des fins éducatives uniquement  ║\n");
    printf("╚════════════════════════════════════════════════╝\n\n");
    
    printf("[*] PID du processus: %d\n\n", getpid());
    
    // Initialiser les données
    init_sensitive_data();
    
    // Ajouter quelques utilisateurs de test
    add_user("admin", "AdminP@ssw0rd!", "admin@company.com", 1);
    add_user("john.doe", "JohnSecret123", "john.doe@company.com", 0);
    add_user("alice", "AliceSecure456!", "alice@company.com", 0);
    add_user("bob", "BobPassword789", "bob@company.com", 0);
    
    // Créer des données intéressantes
    create_interesting_data();
    
    printf("\n[*] Données sensibles chargées en mémoire\n");
    printf("[*] Prêt pour le TP!\n");
    
    // Lancer le menu
    main_loop();
    
    return 0;
}
