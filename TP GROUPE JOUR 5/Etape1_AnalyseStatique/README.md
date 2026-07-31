# Étape 1 : Analyse Statique du Ransomware

**Objectif :** Effectuer une reconnaissance initiale du ransomware pour identifier ses caractéristiques principales.


---

## 🎯 Objectifs de l'Étape

À la fin de cette étape, vous devez :
- ✅ Calculer les hashes du binaire (MD5, SHA256)
- ✅ Extraire les strings et IOCs (IPs, URLs, domaines)
- ✅ Identifier les fonctions malveillantes dans le code source
- ✅ Comprendre le type de chiffrement utilisé
- ✅ Documenter vos découvertes dans `IOCs.txt`

---

## 📁 Fichiers Fournis

```
Etape1_AnalyseStatique/
├── ransomware.c        # Code source du ransomware
├── Makefile            # Pour compiler le ransomware
├── README.md           # Ce fichier
└── SOLUTIONS.md        # Solutions (ne regarder qu'en dernier recours)
```

---

## 🚀 Instructions

### 1. Compilation du Ransomware

```bash
cd Etape1_AnalyseStatique/

# Compiler le ransomware
make

# Ou manuellement
gcc -g -o ransomware ransomware.c
```

**Vérification :** Vous devez voir le fichier `ransomware` créé.

---

### 2. Analyse du Binaire

#### 2.1 Calcul des Hashes

Les hashes permettent d'identifier de manière unique un malware.

MD5
SHA256

**📝 Question 1 :** Notez les 2 hashes dans votre fichier `IOCs.txt`.

---

#### 2.2 Identification du Type de Fichier


**📝 Question 2 :** Quel type de fichier est-ce ?

---

#### 2.3 Extraction des Strings

La commande `strings` extrait toutes les chaînes de caractères lisibles du binaire.

**📝 Question 3 :** Quelles URLs/IPs avez-vous trouvées ?

**📝 Question 4 :** Quels endpoints d'API sont mentionnés ?

**Astuce :** Cherchez des patterns comme :
- `http://` ou `https://`
- `/api/...`
- Adresses IP (format `xxx.xxx.xxx.xxx`)
- Noms de domaines

---

### 3. Analyse du Code Source

Le code source C est fourni pour faciliter votre analyse. Ouvrez `ransomware.c`.

#### 3.1 Identifier les Fonctions Principales

```bash
# Lister toutes les fonctions
grep -n "^void\|^int\|^char\*" ransomware.c

# Trouver la fonction main

# Trouver les fonctions de chiffrement
```

**📝 Question 5 :** Listez les 5 fonctions les plus importantes et leur rôle :

---

#### 3.2 Identifier la Clé de Chiffrement

Cherchez la clé de chiffrement dans le code :

```bash
grep -n "key\[\]" ransomware.c
grep -n "encryption_key" ransomware.c
```

**📝 Question 6 :** Où est définie la clé de chiffrement dans le code ?
- Ligne : ?
- Variable : ?
- Valeur visible : Oui / Non ?

**⚠️ Important :** Ne révélez PAS encore la clé complète. Vous devrez l'extraire avec GDB à l'Étape 2.

---

#### 3.3 Identifier l'Algorithme de Chiffrement

Lisez la fonction `encrypt()` dans le code source.

**📝 Question 7 :** Quel algorithme de chiffrement est utilisé ?
- [ ] AES
- [ ] RSA
- [ ] XOR
- [ ] RC4

**📝 Question 8 :** Pourquoi cet algorithme est-il réversible (chiffrement = déchiffrement) ?

---

#### 3.4 Identifier les Communications C2

Cherchez les informations sur le serveur Command & Control :

```bash
grep -n ? 
```

**📝 Question 9 :** Quelle est l'adresse du serveur C2 ?

**📝 Question 10 :** Quels sont les endpoints utilisés ? (ex: `/api/?`)

**📝 Question 11 :** Quelles informations sont envoyées au C2 ?


---

#### 3.5 Identifier les Extensions Cibles

```bash
grep -n ?
```

**📝 Question 12 :** Quels types de fichiers sont ciblés par le ransomware ?

---

### 4. Analyse du Comportement

#### 4.1 Flow d'Exécution

En lisant la fonction `main()`, identifiez l'ordre des opérations :

**📝 Question 13 :** Numérotez les étapes dans l'ordre :
- [ ] Chiffrement des fichiers
- [ ] Génération de l'ID victime
- [ ] Affichage de la note de rançon
- [ ] Envoi de la clé au C2
- [ ] Enregistrement de la victime

---

#### 4.2 Indicateurs de Compromission (IOCs)

Les IOCs permettent de détecter ce malware sur d'autres systèmes.

**Créez le fichier `IOCs.txt` avec :**

```
# IOCs - Ransomware Pédagogique
# Date : [VOTRE DATE]
# Équipe : [VOTRE ÉQUIPE]

## Hashes
MD5:    [hash MD5]
SHA1:   [hash SHA1]
SHA256: [hash SHA256]

## Network Indicators
C2 Server: [adresse IP ou domaine]
C2 Port:   [port]
Endpoints:


## File Indicators
Extension ajoutée: .encrypted
Note de rançon: README_RANSOM.txt

## Behavior Indicators
- Chiffrement ...
- Communication HTTP POST
- Exfiltration de clé
- Génération victim_id

## Détection
Strings caractéristiques:
  - "....."
  - "......"
  - "......."
```

---

## 📊 Livrables de l'Étape 1

À remettre :
- ✅ Fichier `IOCs.txt` complet
- ✅ Réponses aux 13 questions ci-dessus (dans un fichier texte ou markdown)
- ✅ Notes pour le rapport final

---

**Bon courage ! 🔍**
