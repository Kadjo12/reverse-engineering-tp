# Rapport Final - Analyse de Ransomware

## Informations Générales

- **Nom / Prénom :** 
- **Groupe :** 
- **Date :** 01/01/2024
- **Durée du TP :** 09h00 - 17h00

---

## 📝 Résumé Exécutif

*Rédigez un résumé de 200-300 mots décrivant :*
- Le contexte de l'analyse (ransomware XOR)
- Les principales découvertes
- Les résultats obtenus (clé extraite, fichiers récupérés, vulnérabilités trouvées)
- Votre conclusion sur la sécurité du ransomware et du serveur C2

```
[VOTRE RÉSUMÉ ICI]




```

---

## Étape 1 : Analyse Statique

### 1.1 Méthodologie

*Outils utilisés (cocher) :*
- [ ] `strings`
- [ ] `grep`
- [ ] `hexdump`
- [ ] `readelf`
- [ ] `nm`
- [ ] Lecture du code source C
- [ ] Autres : ________________

### 1.2 Découvertes Principales

**Clé de chiffrement identifiée :**
```
[INDIQUER LA CLÉ ICI]
```

**Longueur de la clé :** ________ octets

**Algorithme de chiffrement :**
```
[XOR / AES / RSA / Autre : ________________]
```

**Serveur C2 :**
```
Adresse : [URL]
Port : [PORT]
```

**Endpoints API contactés :**
1. ______________________________
2. ______________________________

### 1.3 Analyse du Code

*Décrivez brièvement le fonctionnement du ransomware (10-15 lignes) :*

```
[VOTRE ANALYSE ICI]











```

---

## Étape 2 : Debug Dynamique avec GDB

### 2.1 Breakpoints Placés

*Listez les breakpoints utilisés et leur objectif :*

| Fonction | Ligne | Objectif |
|----------|-------|----------|
| | | |
| | | |
| | | |
| | | |

### 2.2 Extraction de la Clé

**Commande GDB utilisée :**
```gdb
(gdb) [VOTRE COMMANDE]
```

**Valeur de la clé en mémoire :**
```
[CONTENU MÉMOIRE]
```

**victim_id généré lors de votre exécution :**
```
[VOTRE VICTIM_ID]
```

### 2.3 Observations

*Décrivez ce que vous avez observé pendant le debug (5-10 lignes) :*

```
[VOS OBSERVATIONS]






```

---

## Étape 3 : Analyse du Serveur C2

### 3.1 Protocole Réseau

**Protocole utilisé :** [HTTP / HTTPS / TCP / UDP]

**Méthode HTTP :** [GET / POST / PUT / DELETE]

**Port :** __________

### 3.2 Données Transmises

**Requête 1 : Enregistrement**

Endpoint : `/api/_______________`

Données envoyées :
```
[COPIER LA REQUÊTE ICI]


```

**Requête 2 : Envoi de la clé**

Endpoint : `/api/_______________`

Données envoyées :
```
[COPIER LA REQUÊTE ICI]


```

### 3.3 Analyse de Sécurité

**La clé est-elle transmise en clair ?** [ Oui / Non ]

**Le protocole est-il chiffré (HTTPS) ?** [ Oui / Non ]

**Risques identifiés :**
1. _______________________________________________
2. _______________________________________________
3. _______________________________________________

---

## Étape 4 : Recouvrement de Données

### 4.1 Script Développé

**Langage utilisé :** [Python / Bash / C / Autre]

**Nom du fichier :** ___________________________

**Nombre de lignes de code :** ________

### 4.2 Résultats

**Nombre de fichiers déchiffrés :** ________ / 3

**Liste des fichiers récupérés :**
- [ ] document.txt
- [ ] rapport.txt
- [ ] clients.txt

**Fichiers correctement déchiffrés ?**
```bash
# Commande de vérification
diff document.txt encrypted_files/document.txt

Résultat : [Identique / Différent]
```

### 4.3 Extraits de Code

*Collez la fonction de déchiffrement XOR que vous avez implémentée :*

```python
def xor_decrypt(data, key):
    # VOTRE CODE ICI





```

---

## Étape 5 : Pentest Web du Serveur C2

### 5.1 Vulnérabilités Identifiées

*Listez au minimum 3 vulnérabilités avec leur niveau de sévérité :*

| # | Vulnérabilité | Sévérité | Endpoint affecté |
|---|---------------|----------|------------------|
| 1 | | [ Critique / Élevé / Moyen / Faible ] | |
| 2 | | [ Critique / Élevé / Moyen / Faible ] | |
| 3 | | [ Critique / Élevé / Moyen / Faible ] | |
| 4 | | [ Critique / Élevé / Moyen / Faible ] | |
| 5 | | [ Critique / Élevé / Moyen / Faible ] | |

### 5.2 Exploitation SQL Injection

**Payload utilisé :**
```sql
[VOTRE PAYLOAD]
```

**Résultat obtenu :**
```json
[COPIER LA RÉPONSE JSON]


```

**Données extraites :**
- Nombre de victimes : ________
- Nombre de clés : ________
- Comptes admin trouvés : ________

### 5.3 Exploitation IDOR

**Endpoint testé :** `/api/victim/___________`

**IDs testés :** De ________ à ________

**Résultat :**
```
[DÉCRIRE CE QUE VOUS AVEZ PU ACCÉDER]


```

### 5.4 Scripts POC

*Listez les scripts Proof of Concept que vous avez créés :*

1. **Fichier :** ___________________________
   - **Objectif :** ___________________________
   - **Vulnérabilité exploitée :** ___________________________

2. **Fichier :** ___________________________
   - **Objectif :** ___________________________
   - **Vulnérabilité exploitée :** ___________________________

3. **Fichier :** ___________________________
   - **Objectif :** ___________________________
   - **Vulnérabilité exploitée :** ___________________________

---

## 📊 Tableau de Synthèse

### Informations Extraites

| Donnée | Valeur |
|--------|--------|
| **Clé de chiffrement** | |
| **Algorithme** | |
| **Serveur C2** | |
| **victim_id** | |
| **Hostname** | |
| **IP Address** | |
| **Nombre de fichiers récupérés** | / 3 |
| **Vulnérabilités web trouvées** | |

---

## 🔐 Analyse de Sécurité

### Points Faibles du Ransomware

*Listez et expliquez les faiblesses de sécurité du ransomware (10-15 lignes) :*

```
1. [FAIBLESSE 1]


2. [FAIBLESSE 2]


3. [FAIBLESSE 3]


4. [AUTRES]


```

### Points Faibles du Serveur C2

*Listez et expliquez les faiblesses du serveur C2 (10-15 lignes) :*

```
1. [FAIBLESSE 1]


2. [FAIBLESSE 2]


3. [FAIBLESSE 3]


4. [AUTRES]


```

---

## 💡 Recommandations

### Pour Améliorer le Ransomware (du point de vue de l'attaquant)

*Si vous étiez l'attaquant, comment amélioreriez-vous la sécurité du ransomware ?*

```
1. [AMÉLIORATION 1]


2. [AMÉLIORATION 2]


3. [AMÉLIORATION 3]


4. [AMÉLIORATION 4]


```

### Pour se Protéger (du point de vue de l'entreprise)

*Quelles mesures de protection recommandez-vous pour éviter ce type d'attaque ?*

```
1. [MESURE 1]


2. [MESURE 2]


3. [MESURE 3]


4. [MESURE 4]


```

---

## 🧪 Difficultés Rencontrées

*Décrivez les principales difficultés et comment vous les avez surmontées :*

| Étape | Difficulté | Solution Adoptée |
|-------|------------|------------------|
| | | |
| | | |
| | | |

---

## 📚 Connaissances Acquises

*Listez 5-10 compétences/connaissances que vous avez acquises pendant ce TP :*

1. _______________________________________________
2. _______________________________________________
3. _______________________________________________
4. _______________________________________________
5. _______________________________________________
6. _______________________________________________
7. _______________________________________________
8. _______________________________________________
9. _______________________________________________
10. _______________________________________________

---

## 📎 Annexes

### Annexe A : Commandes Utilisées

*Listez les commandes importantes que vous avez utilisées :*

```bash
# Étape 1 : Analyse statique




# Étape 2 : GDB




# Étape 3 : Analyse réseau




# Étape 4 : Déchiffrement




# Étape 5 : Pentest




```

### Annexe B : Captures d'Écran

*Indiquez les captures d'écran que vous joignez (optionnel mais recommandé) :*

- [ ] `screenshot_1.png` - [Description]
- [ ] `screenshot_2.png` - [Description]
- [ ] `screenshot_3.png` - [Description]

---

## 🎯 Auto-Évaluation

*Évaluez votre performance sur chaque étape (1-5 étoiles) :*

| Étape | Note (/5) | Commentaire |
|-------|-----------|-------------|
| Étape 1 : Analyse Statique | ⭐⭐⭐⭐⭐ | |
| Étape 2 : Debug GDB | ⭐⭐⭐⭐⭐ | |
| Étape 3 : Analyse C2 | ⭐⭐⭐⭐⭐ | |
| Étape 4 : Recouvrement | ⭐⭐⭐⭐⭐ | |
| Étape 5 : Pentest Web | ⭐⭐⭐⭐⭐ | |

**Note globale que vous vous donnez :** ________ / 20

---

## 📝 Conclusion Personnelle

*Rédigez une conclusion personnelle de 150-200 mots sur ce TP :*

```
[VOTRE CONCLUSION]









```

---

## ✅ Checklist de Remise

Avant de remettre votre rapport, vérifiez :

- [ ] Toutes les sections sont complétées
- [ ] Les commandes sont copiées correctement
- [ ] Les codes sources sont inclus
- [ ] Le rapport est au format Markdown (.md) ou PDF
- [ ] Le nom du fichier : `Rapport_Final_[NOM]_[PRENOM].md`
- [ ] Tous les scripts développés sont joints
- [ ] Les fichiers déchiffrés sont inclus
- [ ] L'orthographe a été vérifiée

---

**Date de remise :** ______________________

**Signature :** ______________________

---

## 📧 Informations de Contact

En cas de question :
- **Email formateur :** [email@example.com]
- **Discord :** [Lien serveur]

---

**Bon courage et félicitations pour votre travail ! 🎉**
