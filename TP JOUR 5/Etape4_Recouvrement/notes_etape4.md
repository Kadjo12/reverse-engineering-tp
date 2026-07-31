# Notes Étape 4 — Recouvrement de Données
# TP Jour 5 | Djimy DONKENG | 2026-07-31

---

## Q1 — Pourquoi XOR permet-il de chiffrer ET déchiffrer avec la même opération ?

Parce que XOR est une opération **symétrique** et **son propre inverse** :

```
A XOR K = B        (chiffrement)
B XOR K = A        (déchiffrement)
```

Preuve :
```
B XOR K = (A XOR K) XOR K = A XOR (K XOR K) = A XOR 0 = A
```

K XOR K = 0, donc appliquer deux fois la même clé annule l'effet.
On utilise donc exactement le même code pour chiffrer et déchiffrer.

---

## Q2 — Que fait `i % key_len` ? Pourquoi est-ce nécessaire ?

`i % key_len` calcule le **reste de la division** de `i` par la longueur de la clé.

Cela permet de **répéter cycliquement** la clé sur toute la longueur du fichier,
même si le fichier est plus long que la clé.

Exemple avec clé de 3 octets et fichier de 7 octets :
```
index fichier :  0  1  2  3  4  5  6
index clé     :  0  1  2  0  1  2  0   ← i % 3
```

Sans `% key_len`, on dépasserait les bornes du tableau de la clé (erreur IndexError).

---

## Q3 — Pourquoi utilise-t-on `'rb'` et `'wb'` (mode binaire) ?

Les fichiers chiffrés sont des **données binaires brutes**, pas du texte.

- `'rb'` (read binary)  : lit les octets exacts du fichier sans aucune conversion
- `'wb'` (write binary) : écrit les octets exacts sans aucune conversion

En mode texte (`'r'` ou `'w'`), Python convertit automatiquement certains
caractères (ex: `\n` ↔ `\r\n` sur Windows), ce qui **corrompt les données binaires**.
Le mode binaire est indispensable pour manipuler des fichiers chiffrés.

---

## Q4 — Le fichier est-il correctement déchiffré ?

OUI. Après exécution du script sur `document.txt.encrypted` :

```bash
cat encrypted_files/document.txt
```

Résultat lisible et cohérent — le fichier original a été restauré correctement.

---

## Q5 — Tous les fichiers sont-ils lisibles ?

OUI. Les 3 fichiers ont été déchiffrés avec succès :

```
✅ Succès: encrypted_files/document.txt
✅ Succès: encrypted_files/rapport.txt
✅ Succès: encrypted_files/clients.txt
```

Contenu de chaque fichier vérifié avec `cat` — tous lisibles en clair.

---

## Q6 — Le script gère-t-il correctement les erreurs ?

OUI. Le script inclut une gestion des erreurs basique :

- **Fichier inexistant** : affiche un message d'erreur sans crasher
- **Répertoire vide** : affiche "0 fichier(s) trouvé(s)" et s'arrête proprement
- **Fichier non chiffré** : traité mais le résultat sera du "bruit" binaire
  (limitation connue : pas de vérification de format `.encrypted`)

---

## Explication de la Méthode Utilisée

J'ai utilisé l'algorithme **XOR** avec la clé **`Sup3rS3cr3tK3y2024!`** (19 octets),
extraite dynamiquement avec GDB à l'Étape 2.

Le script Python lit chaque fichier `.encrypted` en mode binaire, applique l'opération
XOR octet par octet avec la clé en mode cyclique (`key[i % 19]`), puis sauvegarde
le résultat avec le nom d'origine (sans l'extension `.encrypted`).

Résultat : **3/3 fichiers déchiffrés avec succès**.
