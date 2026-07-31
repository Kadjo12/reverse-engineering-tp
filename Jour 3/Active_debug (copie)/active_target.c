/*
 * Active Debug Target - Programme pour exercice de debug actif
 * 
 * Ce programme simule un système d'authentification qui vérifie
 * périodiquement des credentials. Il est conçu pour être débugué
 * pendant son exécution avec GDB.
 * 
 * Compilation: gcc -g -fno-pie -no-pie -o active_target active_target.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Variables globales pour l'authentification
char password[64] = "WrongPassword123";
char correct_password[64] = "SecretPass123!";
int attempt = 0;
int authenticated = 0;

// Fonction secrète non appelée normalement
void secret_admin_mode() {
    printf("\n");
    printf("🔓 ================================ 🔓\n");
    printf("   SECRET ADMIN MODE ACTIVATED!\n");
    printf("   Congratulations, you found me!\n");
    printf("   Flag: DEBUG{H1dd3n_Func_F0und}\n");
    printf("🔓 ================================ 🔓\n");
    printf("\n");
}

// Fonction qui vérifie le mot de passe
int check_password() {
    attempt++;
    
    // C'est ici que la vérification se fait
    int result = strcmp(password, correct_password);
    
    if (result == 0) {
        authenticated = 1;
        printf("✅ Login successful! (attempt #%d)\n", attempt);
        printf("   Welcome, Administrator!\n");
        
        // Easter egg si appelé après 100 tentatives
        if (attempt > 100) {
            printf("🎉 Persistence bonus unlocked!\n");
            printf("   Flag: DEBUG{P3rs1st3nt_D3bugg3r}\n");
        }
        
        return 1;
    } else {
        printf("❌ Login failed! (attempt #%d)\n", attempt);
        printf("   Password provided: '%s'\n", password);
        printf("   Access denied.\n");
        return 0;
    }
}

// Fonction qui simule l'activité du programme
void simulate_activity() {
    printf("\n");
    printf("📡 System activity...\n");
    printf("   Checking authentication status...\n");
    
    // Vérifier le password
    check_password();
    
    if (authenticated) {
        printf("   System status: AUTHENTICATED\n");
        printf("   Running privileged operations...\n");
    } else {
        printf("   System status: UNAUTHORIZED\n");
        printf("   Retrying in 5 seconds...\n");
    }
}

// Fonction principale
int main() {
    pid_t pid = getpid();
    
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║   Active Debug Target - Auth Service      ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("📋 Process Information:\n");
    printf("   PID: %d\n", pid);
    printf("   To attach GDB: sudo gdb -p %d\n", pid);
    printf("\n");
    
    printf("🔐 Authentication System:\n");
    printf("   Current password: '%s'\n", password);
    printf("   Correct password: [HIDDEN]\n");
    printf("   Status: Not authenticated\n");
    printf("\n");
    
    printf("🎯 Debug Objectives:\n");
    printf("   1. Attach GDB to this process\n");
    printf("   2. Set breakpoint on check_password()\n");
    printf("   3. Inspect variables when breakpoint hits\n");
    printf("   4. Modify 'result' variable to bypass check\n");
    printf("   5. Find and call the secret function\n");
    printf("\n");
    
    printf("⏱️  Starting authentication loop...\n");
    printf("   (Checking every 5 seconds)\n");
    printf("\n");
    printf("═══════════════════════════════════════════════\n");
    
    // Boucle principale
    while (1) {
        sleep(5);
        simulate_activity();
        
        // Si authentifié, afficher un message différent
        if (authenticated) {
            printf("\n✅ System is now authenticated!\n");
            printf("   You successfully bypassed the check!\n");
            printf("\n💡 Bonus challenge:\n");
            printf("   Can you find and call secret_admin_mode()?\n");
            printf("   Hint: Use 'info functions secret' in GDB\n");
            printf("\n");
            
            // Continuer à tourner pour permettre d'autres expérimentations
            printf("🔄 Continuing to run for further debugging...\n");
            printf("   (Press Ctrl+C to stop)\n");
            sleep(5);
        }
    }
    
    return 0;
}
