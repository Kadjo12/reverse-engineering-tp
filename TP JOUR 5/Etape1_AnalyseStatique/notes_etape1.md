# Notes Étape 1 — Analyse Statique
# TP Jour 5 | Djimy DONKENG | 2026-07-31

---

## Q1 — Hashes du binaire

MD5    : 851d19efb2eb77f27ae6aa4fee78dc83
SHA256 : 4ceb8ae18b98e01faa12d42f09fd6cb0f7b9cb0336621253ae4b72bb6aef69aa

Commandes utilisées :
```bash
md5sum ransomware
sha256sum ransomware
```

---

## Q2 — Type de fichier

```
ELF 64-bit LSB pie executable, x86-64, dynamically linked, not stripped
```

C'est un **exécutable Linux natif** (ELF), compilé depuis du code C avec symboles de debug (`-g`).
Ce n'est pas du .NET ni un script Python.

Commande : `file ransomware`

---

## Q3 — URLs/IPs trouvées avec strings

```bash
strings ransomware | grep "http"
```

Résultat :
- `http://127.0.0.1:5000/api/register`
- `http://127.0.0.1:5000/api/key`

IP du C2 : **127.0.0.1** (localhost), port **5000**.

---

## Q4 — Endpoints d'API

- `/api/register` → enregistrement de la victime
- `/api/key`      → exfiltration de la clé de chiffrement

---

## Q5 — Fonctions principales (5 les plus importantes)

| Fonction              | Rôle                                                        |
|-----------------------|-------------------------------------------------------------|
| `main()`              | Orchestration globale : appelle toutes les autres fonctions |
| `encrypt()`           | Boucle XOR sur les données (cœur du chiffrement)           |
| `encrypt_file()`      | Lit un fichier, le chiffre, le réécrit, supprime l'original|
| `register_victim()`   | Envoie POST /api/register via curl                          |
| `exfiltrate_key()`    | Envoie POST /api/key (la clé en clair) via curl             |

Commande utilisée :
```bash
grep -n "^void\|^int\|^char\*" ransomware.c
```

---

## Q6 — Clé de chiffrement dans le code

- Ligne : ~30 dans ransomware.c
- Variable : `static const char encryption_key[]`
- Valeur visible dans le binaire : **OUI** (via `strings ransomware | grep "Sup3r"`)
- La valeur complète sera extraite avec GDB à l'Étape 2

---

## Q7 — Algorithme de chiffrement

**XOR**

Repéré dans la fonction `encrypt()` :
```c
data[i] ^= key[i % key_len];
```

---

## Q8 — Pourquoi le XOR est réversible

Parce que XOR est une opération symétrique :
- `A XOR K = B`  (chiffrement)
- `B XOR K = A`  (déchiffrement)

Appliquer la même clé une deuxième fois restaure les données originales.
Donc **encrypt = decrypt** avec le même algorithme et la même clé.

---

## Q9 — Adresse du serveur C2

`http://127.0.0.1:5000`

---

## Q10 — Endpoints utilisés

- `POST /api/register`
- `POST /api/key`

---

## Q11 — Informations envoyées au C2

**Vers /api/register :**
- `victim_id` (format : VICTIM_hostname_timestamp)
- `hostname` (nom de la machine)
- `timestamp` (horodatage Unix)

**Vers /api/key :**
- `victim_id`
- `key` (la clé XOR en clair)

---

## Q12 — Extensions ciblées

`.txt`, `.pdf`, `.docx`, `.xlsx`, `.jpg`, `.png`, `.zip`

Commande :
```bash
grep -n "\.txt\|\.pdf\|\.docx" ransomware.c
```

---

## Q13 — Flow d'exécution (ordre chronologique)

1. Génération de l'ID victime
2. Enregistrement de la victime (POST /api/register)
3. Chiffrement des fichiers (XOR, fichier par fichier)
4. Envoi de la clé au C2 (POST /api/key)
5. Affichage de la note de rançon (README_RANSOM.txt)
