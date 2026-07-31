# Notes Étape 2 — Debug Dynamique & Extraction de Clé
# TP Jour 5 | Djimy DONKENG | 2026-07-31

---

## Q1 — Nombre de fonctions visibles dans GDB

```gdb
(gdb) info functions
```

Résultat : plusieurs fonctions dont `main`, `encrypt`, `encrypt_file`, `encrypt_directory`,
`generate_victim_id`, `register_victim`, `exfiltrate_key`, `drop_ransom_note`, etc.

---

## Q2 — Adresse mémoire de main

Exemple observé :
```
Breakpoint 1 at 0x555555555b9f : file ransomware.c, line 195.
```

L'adresse varie à chaque exécution (ASLR) mais la fonction est toujours au même offset
dans le binaire.

---

## Q3 & Q4 — Clé de chiffrement extraite

Commande GDB :
```gdb
(gdb) x/s encryption_key
```

Résultat :
```
0x555555556010 <encryption_key>: "Sup3rS3cr3tK3y2024!"
```

**Clé complète : `Sup3rS3cr3tK3y2024!`**

---

## Q5 — 20 premiers bytes en hexadécimal

```gdb
(gdb) x/20xb encryption_key
```

Résultat :
```
53 75 70 33 72 53 33 63 72 33 74 4b 33 79 32 30 32 34 21 00
```

Décodage ASCII : S u p 3 r S 3 c r 3 t K 3 y 2 0 2 4 ! \0

---

## Q6 — Longueur de la clé

```gdb
(gdb) print KEY_LENGTH
```

**19 octets** (sans le caractère nul de fin de chaîne).

---

## Q7 — Fichier en cours de chiffrement (encrypt_file)

```gdb
(gdb) info args
```

Exemple :
```
filepath = 0x... "./test_files/test.txt"
```

Le premier fichier traité était `test.txt`.

---

## Q8 — Arguments de la fonction encrypt

Signature de la fonction :
```c
void encrypt(unsigned char* data, size_t data_len, unsigned char* key, size_t key_len)
```

Arguments vus dans GDB :
- `data`     : pointeur vers les octets du fichier à chiffrer
- `data_len` : taille du fichier en bytes
- `key`      : pointeur vers `encryption_key` ("Sup3rS3cr3tK3y2024!")
- `key_len`  : 19

---

## Q9 — Données avant chiffrement

```gdb
(gdb) x/50c data
```

OUI, le contenu du fichier original est visible en clair avant le chiffrement.
On pouvait lire le texte "Document de test pour debug" (notre fichier test.txt).

---

## Résumé — Clé Extraite

| Champ     | Valeur                    |
|-----------|---------------------------|
| Clé       | `Sup3rS3cr3tK3y2024!`     |
| Longueur  | 19 octets                 |
| Type      | ASCII printable + symbole |
| Variable  | `encryption_key` (globale)|
| Adresse   | 0x555555556010 (exemple)  |
| Hex       | 53 75 70 33 72 53 33 63 72 33 74 4b 33 79 32 30 32 34 21 |
