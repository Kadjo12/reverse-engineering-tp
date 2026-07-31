# Challenge 1 — Analyse d'un Binaire Strippé
# TP Jour 5 — Étape 6 Bonus | Djimy DONKENG | 2026-07-31

## Objectif

Extraire la clé de chiffrement d'un ransomware dont les symboles de debug ont été supprimés
(`strip`), sans pouvoir utiliser `break encrypt_file` ou `x/s encryption_key` directement.

---

## Étape 1 — Vérification du Binaire

```bash
file ransomware_stripped
```

Résultat :
```
ELF 64-bit LSB pie executable, x86-64, dynamically linked, stripped
```

Le mot clé **stripped** confirme : pas de table de symboles, pas de noms de fonctions.

```bash
md5sum ransomware_stripped
# 3d9a8073adaa4b5dcb5db2bb361fb2ad
```

Différent du binaire de debug (MD5: 851d19efb2eb77f27ae6aa4fee78dc83) car compilé avec `-O2`.

---

## Étape 2 — Méthode 1 : strings (la plus rapide)

Même strippé, les chaînes en clair restent dans le binaire :

```bash
strings ransomware_stripped | grep -E "Sup3r|api|http|VICTIM"
```

Résultat immédiat :
```
.encrypted
VICTIM_%s_%ld
/api/register
/api/key
Sup3rS3cr3tK3y2024!
```

**Clé trouvée : `Sup3rS3cr3tK3y2024!`**

La clé XOR est stockée en clair dans la section `.rodata` du binaire.
Même `strip` ne la supprime pas car elle est une donnée (pas un symbole de debug).

---

## Étape 3 — Méthode 2 : Analyse de la section .rodata

```bash
readelf -S ransomware_stripped | grep rodata
# [18] .rodata  PROGBITS  0x2000
```

```bash
objdump -s -j .rodata ransomware_stripped
```

Extrait pertinent (offset 0x2500) :
```
2500  53757033 72533363 7233744b 33793230   Sup3rS3cr3tK3y20
2510  32342100                              24!.
```

**Adresse mémoire de la clé : 0x555555556500** (base 0x555555554000 + offset 0x2500)

---

## Étape 4 — Méthode 3 : GDB sans symboles

```bash
gdb ./ransomware_stripped
```

Sans symboles, on ne peut pas faire `break main` directement.
On utilise le point d'entrée :

```bash
readelf -h ransomware_stripped | grep "Entry point"
# Entry point address: 0x14b0
```

Dans GDB :
```gdb
(gdb) break *0x14b0
(gdb) run ./test_files
(gdb) x/100s 0x555555556000    # Scanner la section .rodata
```

La clé apparaît à une adresse dans `.rodata` :
```
0x555555556500: "Sup3rS3cr3tK3y2024!"
```

---

## Comparaison : Binaire avec symboles vs Strippé

| Critère               | Avec symboles (-g)        | Strippé (strip)           |
|-----------------------|---------------------------|---------------------------|
| Taille                | 24 Ko                     | ~16 Ko                    |
| Noms de fonctions     | Visibles (encrypt_file…)  | Absents                   |
| Variables globales    | Accessibles (encryption_key) | Absentes              |
| Strings .rodata       | Présentes ✅              | Présentes ✅              |
| Clé visible via strings | OUI ✅                  | OUI ✅                    |
| GDB facile            | OUI (break encrypt_file)  | NON (adresses brutes)     |

---

## Conclusion

Même avec `strip`, la clé XOR reste visible car :
1. Elle est stockée comme donnée statique dans `.rodata`
2. `strip` supprime uniquement la **table de symboles** (noms), pas les données
3. `strings` l'extrait en 1 seconde

Pour vraiment protéger une clé, il faudrait la chiffrer ou la générer dynamiquement
(ce qui est fait dans les vrais ransomwares avec AES + RSA).

**Clé extraite : `Sup3rS3cr3tK3y2024!`**
**Adresse .rodata : offset 0x2500 dans le binaire**
