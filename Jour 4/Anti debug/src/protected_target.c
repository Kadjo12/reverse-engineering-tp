/*
 * Protected Target - Programme avec protections anti-debug
 * 
 * Ce programme intègre plusieurs techniques anti-debugging courantes.
 * Il est conçu pour être analysé et contourné lors d'exercices de reverse.
 * 
 * Compilation: voir Makefile
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>

// Variables globales
int debug_detected = 0;
int check_count = 0;
char secret_flag[128] = "FLAG{N0_D3bugg3r_D3t3ct3d}";

// Fonction secrète qui affiche le flag final
void show_secret() {
    printf("\n");
    printf("🎉 ═══════════════════════════════════════ 🎉\n");
    printf("         CONGRATULATIONS!\n");
    printf("   You successfully bypassed all protections!\n");
    printf("\n");
    printf("   🚩 SECRET FLAG: %s\n", secret_flag);
    printf("\n");
    printf("🎉 ═══════════════════════════════════════ 🎉\n");
    printf("\n");
}

// Protection 1 : Détection de ptrace
int check_ptrace() {
    check_count++;
    printf("[Check %d] Vérification ptrace...\n", check_count);
    
    // ptrace retourne -1 si déjà tracé
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        printf("❌ DEBUGGER DÉTECTÉ (ptrace) !\n");
        printf("   Un debugger est attaché au processus.\n");
        return 1;
    }
    
    printf("✅ Ptrace check OK\n");
    return 0;
}

// Protection 2 : Vérification du fichier /proc/self/status
int check_proc_status() {
    check_count++;
    printf("[Check %d] Vérification /proc/self/status...\n", check_count);
    
    FILE *status = fopen("/proc/self/status", "r");
    if (!status) {
        printf("⚠️  Impossible de lire /proc/self/status\n");
        return 0;
    }
    
    char line[256];
    int tracer_pid = 0;
    while (fgets(line, sizeof(line), status)) {
        // Rechercher la ligne "TracerPid:"
        if (strncmp(line, "TracerPid:", 10) == 0) {
            tracer_pid = atoi(line + 10);
            break;
        }
    }
    fclose(status);
    
    if (tracer_pid != 0) {
        // Vérifier si c'est vraiment GDB en lisant /proc/TracerPid/comm
        char comm_path[256];
        snprintf(comm_path, sizeof(comm_path), "/proc/%d/comm", tracer_pid);
        FILE *comm = fopen(comm_path, "r");
        if (comm) {
            char tracer_name[256];
            if (fgets(tracer_name, sizeof(tracer_name), comm)) {
                fclose(comm);
                // Vérifier si c'est gdb, strace, ou un vrai debugger
                if (strstr(tracer_name, "gdb") || strstr(tracer_name, "strace") || 
                    strstr(tracer_name, "lldb") || strstr(tracer_name, "radare")) {
                    printf("❌ DEBUGGER DÉTECTÉ (TracerPid=%d) !\n", tracer_pid);
                    printf("   Le processus est tracé par %s", tracer_name);
                    return 1;
                }
            } else {
                fclose(comm);
            }
        }
    }
    
    printf("✅ TracerPid check OK\n");
    return 0;
}

// Protection 3 : Détection par timing (timing attack)
int check_timing() {
    check_count++;
    printf("[Check %d] Vérification timing...\n", check_count);
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Opération simple qui devrait être rapide
    volatile int sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculer le temps écoulé en microsecondes
    long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + 
                   (end.tv_nsec - start.tv_nsec) / 1000;
    
    printf("   Temps écoulé: %ld µs\n", elapsed);
    
    // Si trop lent, probablement un debugger (single-stepping)
    if (elapsed > 50000) { // 50ms threshold
        printf("❌ DEBUGGER DÉTECTÉ (timing) !\n");
        printf("   Exécution trop lente (single-stepping détecté)\n");
        return 1;
    }
    
    printf("✅ Timing check OK\n");
    return 0;
}

// Protection 4 : Vérification de breakpoints (0xCC)
int check_breakpoints() {
    check_count++;
    printf("[Check %d] Vérification breakpoints...\n", check_count);
    
    // Vérifier si un breakpoint (INT3 = 0xCC) est posé sur cette fonction
    // int3, utilisé par GDB pour les breakpoints)
    unsigned char *func_ptr = (unsigned char *)check_breakpoints;
    
    // Que fait cette boucle for ? 
    for (int i = 0; i < 32; i++) {
        if (func_ptr[i] == 0xCC) {
            printf("❌ BREAKPOINT DÉTECTÉ à l'offset +%d !\n", i);
            printf("   Instruction INT3 (0xCC) trouvée dans le code\n");
            return 1;
        }
    }
    
    printf("✅ Breakpoint check OK\n");
    return 0;
}

// Handler pour les signaux
void signal_handler(int sig) {
    if (sig == SIGTRAP) {
        printf("\n❌ SIGNAL SIGTRAP DÉTECTÉ !\n");
        printf("   Un breakpoint ou single-step a été déclenché\n");
        debug_detected = 1;
    }
}

// Fonction qui affiche le contenu normal du programme
void normal_execution() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════╗\n");
    printf("║     Bienvenue dans l'application sécurisée    ║\n");
    printf("╚═══════════════════════════════════════════════╝\n");
    printf("\n");
    printf("🔐 Fonctionnalité principale activée\n");
    printf("📊 Traitement des données en cours...\n");
    printf("✨ Opération terminée avec succès!\n");
    printf("\n");
    printf("💡 Pour débloquer le flag secret, vous devez:\n");
    printf("   - Contourner TOUTES les protections anti-debug\n");
    printf("   - Exécuter le programme sous debugger sans détection\n");
    printf("\n");
}

// Fonction d'exit si debugger détecté
void exit_on_debug() {
    printf("\n");
    printf("🚫 ═══════════════════════════════════════ 🚫\n");
    printf("     DÉTECTION DE DEBUGGER\n");
    printf("     Arrêt du programme par mesure de sécurité\n");
    printf("🚫 ═══════════════════════════════════════ 🚫\n");
    printf("\n");
    exit(1);
}

int main() {
    pid_t pid = getpid();
    
    // Installer le handler de signal
    signal(SIGTRAP, signal_handler);
    
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║    Protected Application v1.0              ║\n");
    printf("║    Anti-Debug Protection: ACTIVE           ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("📋 Informations du processus:\n");
    printf("   PID: %d\n", pid);
    printf("   Architecture: x86_64\n");
    printf("\n");
    
    printf("🛡️  Protections actives:\n");
    printf("   [1] Détection ptrace\n");
    printf("   [2] Vérification TracerPid\n");
    printf("   [3] Analyse de timing\n");
    printf("   [4] Détection de breakpoints\n");
    printf("\n");
    
    printf("🔍 Lancement des vérifications de sécurité...\n");
    printf("════════════════════════════════════════════════\n");
    printf("\n");
    
    // Exécuter toutes les vérifications
    if (check_ptrace()) {
        debug_detected = 1;
        exit_on_debug();
    }
    
    sleep(1);
    
    if (check_proc_status()) {
        debug_detected = 1;
        exit_on_debug();
    }
    
    sleep(1);
    
    if (check_timing()) {
        debug_detected = 1;
        exit_on_debug();
    }
    
    sleep(1);
    
    if (check_breakpoints()) {
        debug_detected = 1;
        exit_on_debug();
    }
    
    printf("\n");
    printf("════════════════════════════════════════════════\n");
    
    // Si aucun debugger détecté
    if (!debug_detected) {
        printf("✅ Toutes les vérifications passées!\n");
        printf("   Aucun debugger détecté.\n");
        normal_execution();
        
        // NE PAS afficher le secret en exécution normale
        printf("⚠️  Exécution normale: flag secret non accessible\n");
        printf("   Utilisez un debugger pour obtenir le flag!\n");
    } else {
        exit_on_debug();
    }
    
    printf("\n");
    return 0;
}
