# Comparaison Ransomware v1 vs v2
# TP Jour 5 — Challenge 4 | Djimy DONKENG | 2026-07-31

## Tableau Comparatif

| Critère                    | Version 1 (original)          | Version 2 (améliorée)              |
|----------------------------|-------------------------------|------------------------------------|
| Algorithme de chiffrement  | XOR simple                    | AES-256-CBC (OpenSSL)              |
| Clé                        | Hardcodée `Sup3rS3cr3tK3y2024!` | Générée aléatoirement (/dev/urandom) |
| Unicité de la clé          | NON — tous partagent la même  | OUI — clé unique par victime       |
| Résistance au bruteforce   | Très faible (19 bytes XOR)    | Pratiquement infaisable (256 bits) |
| IV                         | Aucun                         | Oui (16 bytes, stocké en tête)     |
| Détection debugger         | NON                           | OUI (ptrace)                       |
| Détection VM               | NON                           | OUI (DMI/SMBIOS)                   |
| Suppression originaux      | Simple `remove()`             | Écrasement sécurisé (zeros) puis remove |
| Clé visible via strings    | OUI (en clair dans .rodata)   | NON (clé générée dynamiquement)    |
| Récupération sans payer    | POSSIBLE (clé dans network_capture.log) | IMPOSSIBLE sans la clé C2 |

## Analyse des Améliorations

### 1. AES-256-CBC vs XOR

**XOR (v1) :**
- Symétrique — chiffrer = déchiffrer avec la même opération
- Clé visible en clair dans le binaire (`strings`)
- Cassable si on connaît des séquences de plaintext connues (known-plaintext attack)

**AES-256-CBC (v2) :**
- 2^256 combinaisons de clés possibles → infaisable par bruteforce
- Mode CBC : chaque bloc dépend du précédent (confusion et diffusion)
- Pas de pattern reconnaissable dans le ciphertext

### 2. Clé Aléatoire

**v1 :** La même clé `Sup3rS3cr3tK3y2024!` pour toutes les victimes.
→ Si une victime la révèle, toutes les victimes peuvent se déchiffrer.

**v2 :** `RAND_bytes()` génère 32 octets depuis `/dev/urandom`.
→ Chaque victime a une clé unique, les compromis ne se propagent pas.

### 3. Anti-Debug (ptrace)

```c
if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
    return 1;  // Arrêt si GDB est attaché
}
```

Notre analyse GDB de l'Étape 2 **n'aurait pas fonctionné** sur la v2.
Il faudrait contourner cet anti-debug (ex: patcher l'instruction de comparaison).

### 4. Suppression Sécurisée

**v1 :** `remove(filepath)` — le fichier reste sur disque, récupérable avec `foremost`.

**v2 :** Écrasement avec des zéros AVANT suppression → la récupération forensique échoue.

## Compilation et Test

```bash
# Installer OpenSSL
sudo apt-get install libssl-dev

# Compiler
gcc -O2 -o ransomware_v2 ransomware_v2.c -lssl -lcrypto

# Créer des fichiers de test
mkdir test_v2
echo "Fichier confidentiel" > test_v2/secret.txt

# Lancer (dans un environnement isolé !)
./ransomware_v2 test_v2/
```

## Conclusion

La v2 est significativement plus robuste. Les techniques d'analyse utilisées dans ce TP
(strings, GDB x/s encryption_key, récupération de clé depuis les logs réseau)
ne fonctionneraient plus.

C'est pourquoi les vrais ransomwares modernes (LockBit, REvil, etc.) utilisent :
- AES-256 pour le chiffrement des fichiers
- RSA-2048/4096 pour chiffrer la clé AES (asymétrique)
- Communication HTTPS ou Tor pour masquer le C2
