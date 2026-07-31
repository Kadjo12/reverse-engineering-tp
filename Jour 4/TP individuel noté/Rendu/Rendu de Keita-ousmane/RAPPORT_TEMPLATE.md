# RAPPORT DE SÉCURITÉ - TP FINAL
**Étudiant** : [Votre nom]  
**Date** : [Date du TP]  
**Durée** : 4 heures

---

## TABLE DES MATIÈRES
1. [Synthèse des vulnérabilités](#synthèse)
2. [Corrections détaillées](#corrections)
3. [Tests de validation](#tests)
4. [Conclusion](#conclusion)

---

## 1. SYNTHÈSE DES VULNÉRABILITÉS <a name="synthèse"></a>

### Tableau récapitulatif

| # | Fonction            | Ligne(s) | Type           | Gravité | Corrigée |
|---|----------           |----------|------          |---------|----------|
| 1 |register_user()      |61,73,117 |depassement de  |forte     | ☐ Oui ☐ Non |
     login_user()         139,160,ect   tampon

| 2 |show_profile()       |104,106   |Format string   |Moyen     | ☐ Oui ☐ Non |

| 3 |system_command()     |181,184   |Command injection|forte    | ☐ Oui ☐ Non |

| 4 |main()               |309,315   |Identifiants et |Forte     | ☐ Oui ☐ Non |
                                      token
| 5 |change_password()    |226-230   |Backdoor        |Fort     | ☐ Oui ☐ Non |

| 6 |login_user() / User  |20,21     |Mot de passe    |Forte    | ☐ Oui ☐ Non |
| 7 |                     |          |                |         | ☐ Oui ☐ Non |
| 8 |                     |          |                |         | ☐ Oui ☐ Non |
| 9 |                     |          |                |         | ☐ Oui ☐ Non |
| 10 |                    |          |                |         | ☐ Oui ☐ Non |

Rajouter des lignes si nécessaire.

**Total de vulnérabilités identifiées** : _____

---

## 2. CORRECTIONS DÉTAILLÉES <a name="corrections"></a>

> Chaque correction doit contenir les 4 sections obligatoires.

---

### Correction #1 : [buffer overflow]

#### 🔴 Vulnérabilité originale
**Fonction concernée** : `main()`, register_user(), backup_user_data()
**Ligne(s)** : XX-YY

**Description** :
```
[Décrivez précisément le problème de sécurité]
Le problème est que ces fonctions ne contrôlent pas correctement la quantité de données copiées dans les buffers de destination
```

**Code vulnérable** :
```c
// Collez le code problématique ici
```
strcpy(), gets(), scanf("%s", username); scanf("%s", password); scanf("%s", confirm);


#### 💥 Attaque possible
**Scénario d'exploitation** :
```
[Décrivez comment un attaquant pourrait exploiter cette vulnérabilité]
[Donnez un exemple concret d'entrée malicieuse]
```
Dans le cas de strcpy(), une entrée de grande taille peut dépasser les 16 octets prévus pour username ou password.
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

**Impact** :
- [ ] Exécution de code arbitraire
- [ ] Lecture de mémoire
- [ ] Déni de service (crash)
- [X] Élévation de privilèges
- [ ] Fuite d'informations
- [ ] Autre : _______________

#### ✅ Correction implémentée

**Code corrigé** :
```c
// Collez votre code sécurisé ici
```
char username[16]; printf("Username: ");
if (scanf("%15s", username) != 1) { 
  printf("Entrée invalide\n"); 
  return; 
  }

**Modifications effectuées** :
1. Ajout de limites correspondant à la taille réelle des buffers.
2. 
3. 

#### 🛡️ Justification de sécurité

**Pourquoi cette correction est sûre** :
```
[Expliquez pourquoi votre solution empêche l'exploitation]
[Mentionnez les mécanismes de sécurité utilisés]
```
Car la taille maximale des données acceptées est désormais limitée à la capacité réelle du buffer

**Tests effectués** :
```bash
# Commandes de test utilisées
```
Username:BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

---

### Correction #2 : [Titre de la vulnérabilité]

#### 🔴 Vulnérabilité originale
**Fonction concernée** :show_profile()
**Ligne(s)** :

**Description** :


**Code vulnérable** :
```c
printf("Format d'affichage personnalisé: "); 
gets(format); 
printf(format);

```

#### 💥 Attaque possible
**Scénario d'exploitation** :


**Impact** :
- [ ] 

#### ✅ Correction implémentée

**Code corrigé** :
```c

```

**Modifications effectuées** :
1. 

#### 🛡️ Justification de sécurité

**Pourquoi cette correction est sûre** :


**Tests effectués** :
```bash

```

---

### Correction #3 : [Titre de la vulnérabilité]

#### 🔴 Vulnérabilité originale
**Fonction concernée** :  
**Ligne(s)** :

**Description** :


**Code vulnérable** :
```c

```

#### 💥 Attaque possible
**Scénario d'exploitation** :


**Impact** :
- [ ] 

#### ✅ Correction implémentée

**Code corrigé** :
```c

```

**Modifications effectuées** :
1. 

#### 🛡️ Justification de sécurité

**Pourquoi cette correction est sûre** :


**Tests effectués** :
```bash

```

---

### Correction #4 : [Titre de la vulnérabilité]

_[Répétez le format ci-dessus]_

---

### Correction #5 : [Titre de la vulnérabilité]

_[Répétez le format ci-dessus]_

---

### Correction #6 : [Titre de la vulnérabilité]

_[Répétez le format ci-dessus]_

---

### Correction #7 : [Titre de la vulnérabilité]

_[Répétez le format ci-dessus]_

---

### Correction #8 : [Titre de la vulnérabilité]

_[Répétez le format ci-dessus]_

---

### Correction #9 : [Titre de la vulnérabilité]

_[Répétez le format ci-dessus]_

---

### Correction #10 : [Titre de la vulnérabilité]

_[Répétez le format ci-dessus]_

---

## 3. TESTS DE VALIDATION <a name="tests"></a>

### 3.1 Compilation

**Commande utilisée** :
```bash
gcc -o secure secure.c -lcrypto -Wall -Wextra
```

**Résultat** :
```
[Collez la sortie de compilation]
```

**Nombre de warnings** : _____  
**Nombre d'erreurs** : _____

---

### 3.2 Tests fonctionnels

| Fonctionnalité | Test effectué | Résultat |
|---------------|---------------|----------|
| Création utilisateur | Tentative avec nom très long | ☐ OK ☐ KO |
| Connexion | Login avec mauvais credentials | ☐ OK ☐ KO |
| Transfert | Montant négatif | ☐ OK ☐ KO |
| Backup | Nom de fichier avec `../` | ☐ OK ☐ KO |
| Commande système | Injection avec `;` | ☐ OK ☐ KO |

---

### 3.3 Analyse mémoire (Valgrind)

**Commande** :
```bash
valgrind --leak-check=full --show-leak-kinds=all ./secure
```

**Résultat** :
```
[Collez la sortie de valgrind]
```

**Fuites détectées** : _____ bytes  
**Blocs non libérés** : _____

---

### 3.4 Tests de sécurité

**Test 1 - Buffer Overflow**
```bash
# Entrée testée :
# Résultat attendu :
# Résultat obtenu :
```

**Test 2 - Format String**
```bash
# Entrée testée :
# Résultat attendu :
# Résultat obtenu :
```

**Test 3 - Command Injection**
```bash
# Entrée testée :
# Résultat attendu :
# Résultat obtenu :
```

---

## 4. CONCLUSION <a name="conclusion"></a>

### 4.1 Résumé des corrections

**Statistiques** :
- Vulnérabilités CRITIQUES corrigées : _____
- Vulnérabilités HAUTES corrigées : _____
- Vulnérabilités MOYENNES corrigées : _____
- Total de lignes modifiées : ~_____

**Principaux changements** :
1. 
2. 
3. 

---

### 4.2 Difficultés rencontrées

**Problème 1** :
```
[Description du problème]
[Comment vous l'avez résolu]
```

**Problème 2** :
```
[Description du problème]
[Comment vous l'avez résolu]
```

---

### 4.3 Améliorations possibles

**Sécurité** :
- [ ] Implémenter un système de salage pour les mots de passe
- [ ] Ajouter des logs de sécurité
- [ ] Limiter le nombre de tentatives de connexion
- [ ] Implémenter un système de sessions avec timeout
- [ ] Autre : _______________

**Fonctionnalité** :
- [ ] Chiffrement des données sensibles
- [ ] Audit trail complet
- [ ] Protection contre les timing attacks
- [ ] Autre : _______________

---

### 4.4 Apprentissages clés

**Ce que j'ai appris** :
1. 
2. 
3. 

**Compétences développées** :
- [ ] Identification de vulnérabilités
- [ ] Utilisation de fonctions sécurisées
- [ ] Validation d'entrées
- [ ] Gestion mémoire rigoureuse
- [ ] Cryptographie appliquée

---

### 4.5 Auto-évaluation

| Critère | Note estimée /20 | Justification |
|---------|------------------|---------------|
| Analyse | _____ /6 | |
| Corrections | _____ /10 | |
| Documentation | _____ /4 | |
| **TOTAL** | _____ /20 | |

---

## ANNEXES

### Annexe A : Checklist de vérification finale

- [ ] Tous les `gets()` remplacés par `fgets()`
- [ ] Tous les `scanf("%s")` sécurisés avec taille maximale. Exemp,le : `scanf("%19s", buffer)` pour un buffer de 20 bytes
- [ ] Tous les `strcpy()` remplacés par `strncpy()`
- [ ] Tous les `strcat()` remplacés par `strncat()`
- [ ] Tous les `sprintf()` remplacés par `snprintf()`
- [ ] Cryptographie utilise SHA-256
- [ ] Pas de mots de passe en clair
- [ ] Pas de backdoors
- [ ] Tous les indices de tableaux validés
- [ ] Tous les `malloc()` ont un `free()` correspondant
- [ ] 0 fuites mémoire (valgrind)
- [ ] 0 warnings de compilation
- [ ] Validation stricte des noms de fichiers
- [ ] Protection contre command injection

### Annexe B : Références utilisées

1. 
2. 
3. 

---

**FIN DU RAPPORT**
