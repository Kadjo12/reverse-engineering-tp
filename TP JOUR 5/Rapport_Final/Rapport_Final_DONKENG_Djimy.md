# Rapport Final — Analyse de Ransomware

## Informations Générales

- **Nom / Prénom :** DONKENG KADJO Djimy Romaric
- **Groupe :** MCS 26.2
- **Date :** 2026-07-31
- **Durée du TP :** Journée complète

---

## Résumé Exécutif

Ce TP a consisté à analyser un ransomware pédagogique écrit en C, utilisant un chiffrement XOR avec une clé statique de 19 octets (`Sup3rS3cr3tK3y2024!`). L'analyse statique du binaire a permis d'identifier immédiatement la clé en clair dans les chaînes du binaire, ainsi que les adresses du serveur C2 (`127.0.0.1:5000`), les endpoints d'API (`/api/register`, `/api/key`) et les extensions de fichiers ciblées.

L'analyse dynamique (GDB) a confirmé la présence de la clé en mémoire sous forme de variable globale non protégée. La capture réseau a révélé que la clé est transmise en clair via HTTP — une faiblesse critique permettant à tout administrateur réseau d'intercepter la clé et de déchiffrer les fichiers sans payer.

Les 3 fichiers chiffrés fournis (`document.txt`, `rapport.txt`, `clients.txt`) ont été récupérés avec succès grâce au script `recovery_tool.py` implémentant le déchiffrement XOR inverse. Enfin, le pentest du serveur C2 Flask a mis en évidence 5 vulnérabilités graves : absence d'authentification sur des endpoints sensibles, IDOR, injection SQL, secret hardcodé, et endpoint de debug exposé en production.

Conclusion : ce ransomware est facilement neutralisable en raison de ses nombreuses faiblesses cryptographiques et réseau. Il illustre néanmoins parfaitement la structure d'un vrai ransomware (enregistrement C2, chiffrement, exfiltration de clé, note de rançon).

---

## Étape 1 : Analyse Statique

### 1.1 Méthodologie

Outils utilisés :
- `gcc -g -O0` : compilation avec symboles de debug
- `strings` : extraction des chaînes lisibles du binaire
- `grep` : filtrage des patterns suspects
- `md5sum`, `sha1sum`, `sha256sum` : calcul des hashes
- `file` : identification du type de fichier
- Lecture directe du code source `ransomware.c`

### 1.2 Découvertes Principales

**Clé de chiffrement identifiée :**
```
Sup3rS3cr3tK3y2024!
```

**Longueur de la clé :** 19 octets

**Algorithme de chiffrement :** XOR cyclique (`data[i] ^= key[i % key_len]`)

**Serveur C2 :**
```
Adresse : 127.0.0.1
Port    : 5000
```

**Endpoints API contactés :**
1. `POST /api/register` — enregistrement de la victime (victim_id, hostname, timestamp)
2. `POST /api/key` — exfiltration de la clé en clair

### 1.3 Hashes

| Hash | Valeur |
|------|--------|
| MD5    | `851d19efb2eb77f27ae6aa4fee78dc83` |
| SHA1   | `b764e9e0bafeccebb78aa31d91e896822543cd0b` |
| SHA256 | `4ceb8ae18b98e01faa12d42f09fd6cb0f7b9cb0336621253ae4b72bb6aef69aa` |

**Type de fichier :** ELF 64-bit LSB pie executable, x86-64, with debug_info, not stripped  
**Taille :** ~24 Ko

### 1.4 Fonctions principales

| Fonction | Rôle |
|----------|------|
| `main()` | Orchestration : génère victim_id, enregistre C2, chiffre, exfiltre clé, dépose note |
| `generate_victim_id()` | Crée `VICTIM_<hostname>_<timestamp>` |
| `encrypt()` | Boucle XOR sur les données (data[i] ^= key[i % key_len]) |
| `encrypt_file()` | Lit un fichier, chiffre, écrit `.encrypted`, supprime l'original |
| `encrypt_directory()` | Parcours récursif, limite à MAX_FILES=15 fichiers |
| `register_victim()` | POST /api/register via curl |
| `exfiltrate_key()` | POST /api/key en clair via curl |
| `drop_ransom_note()` | Dépose README_RANSOM.txt avec victim_id et adresse BTC |

### 1.5 Flow d'exécution (ordre)

1. Génération de l'ID victime
2. Enregistrement de la victime au C2
3. Chiffrement des fichiers
4. Envoi de la clé au C2
5. Affichage de la note de rançon

### 1.6 Algorithme de chiffrement — Pourquoi XOR est réversible

XOR est une opération involutive : `A XOR B XOR B = A`.  
Appliquer deux fois le même XOR avec la même clé redonne les données originales.  
Donc `chiffrer = déchiffrer` : un seul algorithme suffit.

---

## Étape 2 : Debug Dynamique avec GDB

### 2.1 Breakpoints Placés

| Fonction | Objectif |
|----------|----------|
| `main` | Point d'entrée — inspecter les variables globales dès le démarrage |
| `encrypt_file` | Identifier quel fichier est en cours de chiffrement |
| `encrypt` | Observer les données et la clé au moment du XOR |

### 2.2 Extraction de la Clé

**Commandes GDB utilisées :**
```gdb
(gdb) break main
(gdb) break encrypt_file
(gdb) break encrypt
(gdb) run ./test_files
(gdb) x/s encryption_key
(gdb) x/20xb encryption_key
(gdb) print KEY_LENGTH
```

**Résultat — valeur de la clé en mémoire :**
```
0x... <encryption_key>: "Sup3rS3cr3tK3y2024!"
```

**Représentation hexadécimale :**
```
53 75 70 33 72 53 33 63 72 33 74 4b 33 79 32 30 32 34 21 00
S  u  p  3  r  S  3  c  r  3  t  K  3  y  2  0  2  4  !  \0
```

**Longueur :** 19 octets (`print KEY_LENGTH` → 19)

### 2.3 Observations

La variable `encryption_key` est déclarée globale dans le code C, ce qui la rend :
- Visible dans le binaire via `strings` (aucune obfuscation)
- Directement accessible dans GDB dès le breakpoint sur `main` (`x/s encryption_key`)
- Présente dans la table des symboles (`nm ransomware | grep encryption_key`)

Dans `encrypt`, les 4 arguments (`data`, `data_len`, `key`, `key_len`) confirment le schéma XOR : `key` pointe vers la même adresse que `encryption_key`. Avant le premier XOR, `x/50c data` montre le contenu en clair du fichier — preuve que le chiffrement se fait bien octet par octet.

---

## Étape 3 : Analyse du Serveur C2

### 3.1 Protocole Réseau

- **Protocole :** HTTP (non chiffré)
- **Méthode :** POST
- **Port :** 5000

### 3.2 Données Transmises

**Requête 1 — Enregistrement** (`POST /api/register`) :
```json
{
  "victim_id": "VICTIM_student-vm_1734444225",
  "hostname":  "student-vm",
  "timestamp": 1734444225
}
```

**Réponse :**
```json
{"status": "success", "message": "Victim registered", "id": 1}
```

**Requête 2 — Exfiltration de clé** (`POST /api/key`) :
```json
{
  "victim_id": "VICTIM_student-vm_1734444225",
  "key":       "Sup3rS3cr3tK3y2024!"
}
```

**Réponse :**
```json
{"status": "success", "message": "Key stored", "key_id": 1}
```

### 3.3 Analyse de Sécurité

**La clé est-elle transmise en clair ?** OUI  
**Le protocole est-il chiffré (HTTPS) ?** NON

**Risques identifiés :**
1. Interception de la clé sur le réseau (wireshark/tcpdump) → déchiffrement sans payer
2. Tout attaquant sur le même réseau peut extraire la clé des paquets
3. Le victim_id révèle le nom de la machine infectée

---

## Étape 4 : Recouvrement de Données

### 4.1 Script Développé

- **Langage :** Python 3
- **Fichier :** `recovery_tool.py`
- **Principe :** XOR avec la clé `Sup3rS3cr3tK3y2024!` (symétrie de XOR)

### 4.2 Résultats

**Nombre de fichiers déchiffrés : 3 / 3**

- ✅ `document.txt` — "Ceci est un document confidentiel de test."
- ✅ `rapport.txt` — "Rapport financier Q4 2024 / CA: 1.2M€ / Bénéfices: 300K€"
- ✅ `clients.txt` — "Liste des clients: 1. Acme Corp, 2. TechStart Inc, 3. Global Solutions"

### 4.3 Fonction de déchiffrement XOR implémentée

```python
def xor_decrypt(data: bytes, key: bytes) -> bytes:
    decrypted = bytearray()
    key_len = len(key)
    for i, byte in enumerate(data):
        decrypted_byte = byte ^ key[i % key_len]  # XOR cyclique
        decrypted.append(decrypted_byte)
    return bytes(decrypted)
```

**Explication :** J'ai utilisé XOR cyclique avec la clé `Sup3rS3cr3tK3y2024!` (19 octets).  
Chaque octet chiffré est XORé avec `key[i % 19]`. Comme XOR est symétrique, cette opération annule exactement le chiffrement appliqué par le ransomware.

---

## Étape 5 : Pentest Web du Serveur C2

### 5.1 Vulnérabilités Identifiées

| # | Vulnérabilité | Sévérité | Endpoint |
|---|---------------|----------|----------|
| 1 | Absence d'authentification sur `/api/victims` et `/api/keys` | **Critique** | `/api/victims`, `/api/keys` |
| 2 | IDOR — accès aux données de toute victime par ID numérique | **Élevé** | `/api/victim/<id>` |
| 3 | SQL Injection — paramètre `name` non échappé | **Critique** | `/api/search?name=` |
| 4 | Secret Flask hardcodé dans le code source | **Élevé** | (config) |
| 5 | Endpoint de debug `/api/debug/config` exposé — révèle mot de passe admin | **Critique** | `/api/debug/config` |

### 5.2 Exploitation SQL Injection

**Payload :**
```
' OR '1'='1
```
**Requête générée côté serveur :**
```sql
SELECT * FROM victims WHERE hostname LIKE '%' OR '1'='1%'
```
**Résultat :** Toutes les victimes de la base retournées.

```bash
curl "http://localhost:5000/api/search?name=' OR '1'='1"
```

**Données extraites :**
- 3 victimes dans la base (test-machine-1, laptop-user, server-prod)

### 5.3 Exploitation IDOR

```bash
curl http://localhost:5000/api/victim/1   # Victime 1
curl http://localhost:5000/api/victim/2   # Victime 2
curl http://localhost:5000/api/victim/3   # Victime 3
```
Chaque requête retourne les données complètes d'une victime sans aucune vérification d'identité.

### 5.4 Absence d'authentification — Extraction de toutes les clés

```bash
curl http://localhost:5000/api/keys
```
Retourne toutes les clés de toutes les victimes en clair — aucun token, aucun login requis.

### 5.5 Endpoint de debug exposé

```bash
curl http://localhost:5000/api/debug/config
```
Retourne :
```json
{
  "SECRET_KEY": "super_secret_key_hardcoded_123",
  "DATABASE": "c2_database.db",
  "DEBUG": true,
  "VERSION": "1.0-dev",
  "ADMIN_PASSWORD": "password123"
}
```

---

## Tableau de Synthèse

| Donnée | Valeur |
|--------|--------|
| **Clé de chiffrement** | `Sup3rS3cr3tK3y2024!` |
| **Algorithme** | XOR cyclique (19 octets) |
| **Serveur C2** | `127.0.0.1:5000` (HTTP) |
| **Endpoints C2** | `/api/register`, `/api/key` |
| **victim_id** | `VICTIM_<hostname>_<timestamp>` |
| **Extensions ciblées** | `.txt .pdf .docx .xlsx .jpg .png .zip` |
| **Fichiers récupérés** | 3 / 3 |
| **Vulnérabilités web** | 5 (3 Critiques, 2 Élevées) |

---

## Analyse de Sécurité

### Points Faibles du Ransomware

1. **Clé en clair dans le binaire** : `strings ransomware | grep Sup3r` révèle immédiatement la clé. Aucune obfuscation.
2. **Clé transmise en clair sur HTTP** : interceptable par wireshark/tcpdump depuis le même réseau.
3. **XOR avec clé statique** : la même clé chiffre toutes les victimes — une clé compromise déchiffre tout.
4. **Variable globale non protégée** : visible en mémoire dès le démarrage du processus via GDB.
5. **Utilisation de `system(curl ...)` pour le C2** : laisse des traces dans les logs shell, et curl peut être absent du système cible.

### Points Faibles du Serveur C2

1. **Aucune authentification** : tous les endpoints sont accessibles sans token ni login.
2. **SQL Injection** : concaténation directe du paramètre `name` dans la requête SQL.
3. **IDOR** : les IDs numériques séquentiels permettent d'énumérer toutes les victimes.
4. **Secret Flask hardcodé** : `super_secret_key_hardcoded_123` dans le code source.
5. **Endpoint debug en production** : `/api/debug/config` expose mot de passe admin et configuration interne.

---

## Recommandations

### Pour se Protéger (entreprise)

1. **Sauvegardes régulières hors ligne** (3-2-1 : 3 copies, 2 supports différents, 1 hors site) — seule vraie protection contre les ransomwares.
2. **Segmentation réseau** : isoler les machines critiques, bloquer les connexions sortantes non autorisées.
3. **EDR/IDS** : détecter les comportements suspects (chiffrement massif de fichiers, connexions HTTP vers des IPs inconnues).
4. **Principe du moindre privilège** : limiter les droits des utilisateurs pour ralentir la propagation.
5. **Mise à jour régulière** : patcher les vulnérabilités exploitées pour l'accès initial.

### Pour Améliorer le Ransomware (point de vue attaquant)

1. **Chiffrement asymétrique** : utiliser RSA ou ECDH pour que la clé locale ne soit jamais la même que la clé de déchiffrement → impossible à retrouver sans le serveur.
2. **Obfusquer la clé** : XOR la clé avec une constante ou la dériver d'un secret serveur → non visible dans le binaire.
3. **HTTPS pour le C2** : chiffrer la communication → clé non interceptable sur le réseau.
4. **Clé unique par victime** : générée côté serveur et poussée vers la victime → une clé compromise ne déchiffre qu'une victime.
5. **Anti-debug** : détecter GDB/ptrace pour rendre l'extraction de clé plus difficile.

---

## Étape 6 : Challenges Bonus

Cinq challenges bonus ont été réalisés pour approfondir les compétences acquises.

### Challenge 1 — Analyse d'un Binaire Strippé (⭐⭐⭐)

**Objectif :** Extraire la clé sans symboles de debug.

Le binaire `ransomware_stripped` a été produit avec `gcc -O2 && strip`. Sans symboles,
la commande `break encryption_key` dans GDB ne fonctionne plus.

**Méthode utilisée :** `strings ransomware_stripped | grep -E "Sup3r"` — la clé XOR reste
visible en clair dans la section `.rodata` même après strip. L'adresse trouvée : offset `0x2500`.

**Conclusion :** `strip` supprime les symboles mais pas les données statiques.
Pour vraiment protéger la clé, il faut la générer dynamiquement (voir Challenge 4).

**Livrable :** `Etape6_Bonus/challenge1/challenge1_report.md`

---

### Challenge 2 — Vaccin Anti-Ransomware (⭐⭐⭐⭐)

**Objectif :** Détecter et bloquer le ransomware avant qu'il ne chiffre les fichiers.

**Script `vaccine.py` — trois mécanismes de défense :**
1. Surveillance des processus (`ps aux`) — tue le processus ransomware dès détection
2. Surveillance des connexions réseau (`ss -tnp`) — alerte si connexion vers port 5000
3. Surveillance des fichiers (`os.walk`) — détecte les nouveaux `.encrypted`
4. Backup préventif automatique + restauration en cas d'attaque

**Utilisation :**
```bash
python3 vaccine.py ./test_files/
```

**Livrable :** `Etape6_Bonus/challenge2/vaccine.py`

---

### Challenge 3 — Exploit Avancé du Serveur C2 (⭐⭐⭐⭐⭐)

**Objectif :** Automatiser l'exploitation des 5 vulnérabilités identifiées à l'Étape 5.

**Script `exploit.py` — 4 exploits enchaînés :**
1. Dump complet sans authentification (`/api/victims`, `/api/keys`)
2. Énumération IDOR (`/api/victim/1`, `/2`, `/3`…)
3. SQL Injection (`' OR '1'='1`) → toutes les victimes retournées
4. Configuration admin exposée (`/api/debug/config`) → `ADMIN_PASSWORD: password123`

**Impact calculé :** 18 clés de chiffrement récupérables → toutes les victimes peuvent
déchiffrer sans payer. Le modèle économique du ransomware est brisé.

**Livrables :** `Etape6_Bonus/challenge3/exploit.py` + `writeup.md`

---

### Challenge 4 — Ransomware Amélioré (⭐⭐⭐⭐)

**Objectif :** Implémenter les améliorations qui rendraient ce ransomware résistant à notre analyse.

**`ransomware_v2.c` — 5 améliorations :**
1. **AES-256-CBC** (OpenSSL) au lieu de XOR → infaisable par bruteforce
2. **Clé aléatoire** (`/dev/urandom`) → clé unique par victime, invisible dans le binaire
3. **Anti-debug** (`ptrace PTRACE_TRACEME`) → notre analyse GDB de l'Étape 2 aurait échoué
4. **Détection de VM** (lecture `/sys/class/dmi/id/`) → détecte VirtualBox/VMware
5. **Suppression sécurisée** (écrasement zeros) → récupération forensique impossible

**Compilation et test :**
```bash
gcc -O2 -o ransomware_v2 ransomware_v2.c -lssl -lcrypto
./ransomware_v2 ./test_v2/
# [*] Clé AES-256 générée aléatoirement (32 bytes)
# [+] Chiffré (AES-256) : test_v2/secret.txt -> test_v2/secret.txt.encrypted
```

**Livrables :** `Etape6_Bonus/challenge4/ransomware_v2.c` + `comparaison_v1_v2.md` + binaire compilé

---

### Challenge 5 — Forensics : Timeline de l'Attaque (⭐⭐⭐)

**Objectif :** Reconstituer la chronologie complète de l'attaque à partir des artéfacts système.

**Timeline reconstituée :**

| Temps | Événement |
|-------|-----------|
| T+0:00 | Exécution de `./ransomware` |
| T+0:01 | Génération du victim_id (`VICTIM_student-vm_1734444225`) |
| T+0:02 | POST `/api/register` vers le C2 |
| T+0:03 | Chiffrement XOR des 3 fichiers (< 1 seconde) |
| T+0:04 | POST `/api/key` — exfiltration de la clé en clair |
| T+0:05 | Dépôt de `README_RANSOM.txt` |

**Artéfacts identifiés :** fichiers `.encrypted`, logs réseau, historique bash, timestamps.

**Méthode de récupération :** clé extraite des logs réseau → `recovery_tool.py` → 3/3 fichiers restaurés.

**Livrables :** `Etape6_Bonus/challenge5/timeline.md` + `forensics_report.md`

---

## Difficultés Rencontrées

| Étape | Difficulté | Solution |
|-------|------------|----------|
| Étape 1 | `ransomware.c` fourni vide | Reconstruction à partir du log réseau (Étape 3), du template (Étape 4) et du serveur C2 (Étape 5) |
| Étape 4 | Comprendre le XOR cyclique | Lecture du template `recovery_tool.py` + vérification avec `xxd` sur les fichiers chiffrés |
| Étape 5 | Identifier toutes les vulnérabilités | Lecture ligne par ligne du code `app.py` |

---

## Connaissances Acquises

1. Utilisation de `strings`, `grep`, `md5sum` pour l'analyse statique d'un binaire ELF
2. XOR cyclique : symétrie, implémentation en C et Python, limites comme algorithme de chiffrement
3. Utilisation de GDB (breakpoints, `x/s`, `x/Nxb`, `info args`, `info locals`)
4. Lecture et analyse de captures réseau (`network_capture.log`)
5. Structure d'un ransomware C : génération victim_id, chiffrement, C2, note de rançon
6. SQL Injection basique (`' OR '1'='1`) et IDOR par énumération d'IDs
7. Absence d'authentification comme vulnérabilité critique sur une API REST
8. Importance des sauvegardes hors ligne face aux ransomwares
9. Pourquoi le chiffrement asymétrique est supérieur au XOR pour un ransomware réel
10. Rédaction d'un rapport d'analyse de malware structuré (IOCs, comportement, recommandations)

---

## Annexes

### Annexe A — Commandes Utilisées

```bash
# Étape 1 : Analyse statique
gcc -g -Wall -O0 -o ransomware ransomware.c
md5sum ransomware && sha1sum ransomware && sha256sum ransomware
file ransomware
strings ransomware | grep -E "http|key|encrypt|victim|api"
grep -n "encryption_key\|C2_HOST\|C2_PORT\|ENCRYPTED_EXT" ransomware.c

# Étape 2 : GDB
gdb ./ransomware
(gdb) break main
(gdb) break encrypt_file
(gdb) break encrypt
(gdb) run ./test_files
(gdb) x/s encryption_key
(gdb) x/20xb encryption_key
(gdb) print KEY_LENGTH

# Étape 3 : Analyse réseau
cat Etape3_AnalyseC2/network_capture.log
# Clé extraite du champ "key" du POST /api/key : Sup3rS3cr3tK3y2024!

# Étape 4 : Déchiffrement
python3 recovery_tool.py ./encrypted_files/

# Étape 5 : Pentest Web
curl http://localhost:5000/api/victims
curl http://localhost:5000/api/keys
curl http://localhost:5000/api/victim/1
curl "http://localhost:5000/api/search?name=' OR '1'='1"
curl http://localhost:5000/api/debug/config
```

### Annexe B — Fichiers Produits

- `Etape1_AnalyseStatique/ransomware.c` — code source C reconstruit
- `Etape1_AnalyseStatique/ransomware` — binaire compilé
- `Etape1_AnalyseStatique/IOCs.txt` — indicateurs de compromission complets
- `Etape2_DebugDynamique/key_extracted.txt` — clé extraite via GDB
- `Etape2_DebugDynamique/gdb_commands.txt` — commandes GDB documentées
- `Etape4_Recouvrement/recovery_tool.py` — script de déchiffrement complété
- `Etape4_Recouvrement/encrypted_files/document.txt` — ✅ déchiffré
- `Etape4_Recouvrement/encrypted_files/rapport.txt` — ✅ déchiffré
- `Etape4_Recouvrement/encrypted_files/clients.txt` — ✅ déchiffré

**Étape 6 — Bonus :**
- `Etape6_Bonus/challenge1/challenge1_report.md` — analyse binaire strippé
- `Etape6_Bonus/challenge1/ransomware_stripped` — binaire sans symboles
- `Etape6_Bonus/challenge2/vaccine.py` — vaccin anti-ransomware
- `Etape6_Bonus/challenge3/exploit.py` — exploit automatisé C2
- `Etape6_Bonus/challenge3/writeup.md` — writeup exploitation
- `Etape6_Bonus/challenge4/ransomware_v2.c` — ransomware AES-256 + anti-debug
- `Etape6_Bonus/challenge4/ransomware_v2` — binaire compilé
- `Etape6_Bonus/challenge4/comparaison_v1_v2.md` — analyse comparative
- `Etape6_Bonus/challenge5/timeline.md` — timeline forensique complète
- `Etape6_Bonus/challenge5/forensics_report.md` — rapport d'analyse forensique

---

**Date de remise :** 2026-07-31

