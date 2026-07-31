# Recovery Tool — Mode d'Emploi
# TP Jour 5 — Étape 4 | Djimy DONKENG

## Description

`recovery_tool.py` est un outil Python de déchiffrement des fichiers chiffrés
par le ransomware analysé dans ce TP. Il utilise l'algorithme XOR avec la clé
extraite à l'Étape 2 via GDB.

## Prérequis

- Python 3.6 ou supérieur
- Aucune dépendance externe (bibliothèque standard uniquement)

## Installation

```bash
# Aucune installation requise
python3 --version  # Vérifier que Python 3 est installé
```

## Utilisation

### Déchiffrer un seul fichier

```bash
python3 recovery_tool.py encrypted_files/document.txt.encrypted
```

Résultat : crée `encrypted_files/document.txt` dans le même dossier.

### Déchiffrer un répertoire entier

```bash
python3 recovery_tool.py encrypted_files/
```

Résultat : déchiffre tous les fichiers `.encrypted` trouvés récursivement.

## Exemple de Sortie

```
✅ Clé configurée: 19 bytes
📋 3 fichier(s) trouvé(s):
   - encrypted_files/document.txt.encrypted
   - encrypted_files/rapport.txt.encrypted
   - encrypted_files/clients.txt.encrypted

🔓 Déchiffrement en cours...
✅ Succès: encrypted_files/document.txt
✅ Succès: encrypted_files/rapport.txt
✅ Succès: encrypted_files/clients.txt

✅ Fichiers déchiffrés avec succès: 3/3
```

## Comment Ça Marche ?

Le ransomware chiffre chaque fichier avec un XOR cyclique :

```
octet_chiffré[i] = octet_original[i] XOR clé[i % longueur_clé]
```

Comme XOR est une opération symétrique (A XOR K XOR K = A), la même
opération appliquée sur le fichier chiffré avec la même clé redonne le
fichier original. C'est pourquoi encrypt = decrypt.

## Algorithme

```python
def xor_decrypt(data: bytes, key: bytes) -> bytes:
    decrypted = bytearray()
    key_len = len(key)
    for i, byte in enumerate(data):
        decrypted.append(byte ^ key[i % key_len])
    return bytes(decrypted)
```

## Clé Utilisée

```
Sup3rS3cr3tK3y2024!  (19 octets)
```

Extraite avec GDB (voir Étape 2 — gdb_commands.txt).

## Limitations

- Fonctionne uniquement avec la clé spécifique à ce ransomware
- Pas de vérification d'intégrité (hash) après déchiffrement
- Ne gère pas les fichiers de plus de quelques centaines de Mo en RAM
- Ne supprime pas les fichiers `.encrypted` après déchiffrement (sécurité)
