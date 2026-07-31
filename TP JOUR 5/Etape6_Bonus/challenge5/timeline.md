# Timeline de l'Attaque Ransomware — Forensics
# TP Jour 5 — Challenge 5 | Djimy DONKENG | 2026-07-31

## Informations Générales

| Champ           | Valeur                              |
|-----------------|-------------------------------------|
| Système victime | Ubuntu (Vagrant VM - re-lab)        |
| Hostname        | student-vm                          |
| Date d'analyse  | 2026-07-31                          |
| Analyste        | Djimy DONKENG                       |
| Outils utilisés | stat, ps, netstat, strings, find, cat |

---

## Timeline Complète de l'Attaque

### T+0:00 — Infection Initiale

```
[2024-12-17 14:23:44] EXÉCUTION
  Processus : ./ransomware ./test_files
  PID       : (variable)
  Utilisateur: vagrant
  Répertoire : ~/labs/A_re-malware/Etape1_AnalyseStatique/
```

Artéfact : entrée dans `~/.bash_history`
```bash
./ransomware ./test_files
```

---

### T+0:01 — Génération du Victim ID

```
[2024-12-17 14:23:44] VICTIM_ID GÉNÉRÉ
  Format    : VICTIM_<hostname>_<timestamp_unix>
  Exemple   : VICTIM_student-vm_1734444225
  Timestamp : 1734444225 → Tue Dec 17 14:23:45 2024
```

Méthode de reconstruction :
```bash
# Convertir le timestamp Unix
date -d @1734444225
# → Tue Dec 17 14:23:45 UTC 2024
```

---

### T+0:02 — Connexion au Serveur C2

```
[2024-12-17 14:23:45] CONNEXION C2
  Destination : 127.0.0.1:5000
  Méthode    : HTTP POST via curl
  Endpoint   : /api/register
  Payload    : {"victim_id": "VICTIM_student-vm_1734444225",
                "hostname": "student-vm",
                "timestamp": 1734444225}
```

Détectable via :
```bash
sudo netstat -antp | grep 5000
# tcp  127.0.0.1:PORT  127.0.0.1:5000  ESTABLISHED  PID/curl
```

---

### T+0:03 — Début du Chiffrement

```
[2024-12-17 14:23:45] CHIFFREMENT DÉMARRÉ
  Algorithme : XOR (clé "Sup3rS3cr3tK3y2024!")
  Cibles     : .txt .pdf .docx .xlsx .jpg .png .zip
  Limite     : 15 fichiers
```

Fichiers chiffrés (dans l'ordre de traitement) :
```
14:23:45.127  document.txt     → document.txt.encrypted
14:23:45.128  rapport.txt      → rapport.txt.encrypted
14:23:45.129  clients.txt      → clients.txt.encrypted
```

Artéfacts détectables :
```bash
# Fichiers créés récemment
find ~/labs -name "*.encrypted" -mmin -60

# Timestamps de modification
stat encrypted_files/*.encrypted
```

---

### T+0:04 — Exfiltration de la Clé

```
[2024-12-17 14:23:45] CLÉ EXFILTRÉE
  Destination : 127.0.0.1:5000/api/key
  Payload    : {"victim_id": "VICTIM_student-vm_1734444225",
                "key": "Sup3rS3cr3tK3y2024!"}
```

Visible dans les logs du serveur C2 et interceptable avec tcpdump.

---

### T+0:05 — Dépôt de la Note de Rançon

```
[2024-12-17 14:23:45] NOTE DE RANÇON CRÉÉE
  Fichier  : README_RANSOM.txt
  Dossier  : ./test_files/README_RANSOM.txt
```

```bash
stat test_files/README_RANSOM.txt
# Modify: 2024-12-17 14:23:45
```

---

### T+0:06 — Fin de l'Attaque

```
[2024-12-17 14:23:45] ATTAQUE TERMINÉE
  Durée totale    : < 1 seconde
  Fichiers touchés: 3 (dans notre TP)
  Processus       : terminé normalement
```

---

## Artéfacts Collectés

### Fichiers Créés par le Ransomware

| Fichier | Timestamp | Description |
|---------|-----------|-------------|
| document.txt.encrypted | 2024-12-17 14:23:45 | Fichier chiffré XOR |
| rapport.txt.encrypted  | 2024-12-17 14:23:45 | Fichier chiffré XOR |
| clients.txt.encrypted  | 2024-12-17 14:23:45 | Fichier chiffré XOR |
| README_RANSOM.txt      | 2024-12-17 14:23:45 | Note de rançon |

### Fichiers Supprimés (Récupérables)

Les fichiers originaux ont été supprimés avec `remove()` (pas de suppression sécurisée).
Ils pourraient être partiellement récupérés avec `foremost` ou `testdisk`.

```bash
sudo apt-get install foremost
sudo foremost -i /dev/sda1 -o ./recovered/ -t txt,pdf
```

### Logs Réseau

Voir `Etape3_AnalyseC2/network_capture.log` pour les requêtes HTTP capturées.

---

## IOCs Forensiques

```
# Fichiers
Pattern : *.encrypted (extension ajoutée)
Fichier : README_RANSOM.txt (note de rançon)
Hash binaire MD5 : 851d19efb2eb77f27ae6aa4fee78dc83

# Réseau
Connexion HTTP POST vers :5000/api/register
Connexion HTTP POST vers :5000/api/key
User-Agent : curl/7.x

# Comportement
Création massive de fichiers .encrypted en < 1 seconde
Processus curl lancé en sous-processus (system())
Connexion locale vers port 5000

# Strings caractéristiques
"VICTIM_" (préfixe victim_id)
"Sup3rS3cr3tK3y2024!" (clé XOR en clair)
"/api/register" et "/api/key" (endpoints C2)
```

---

## Commandes d'Analyse Forensique

```bash
# 1. Trouver tous les fichiers chiffrés
find / -name "*.encrypted" -mtime -1 2>/dev/null

# 2. Trouver la note de rançon
find / -name "README_RANSOM.txt" 2>/dev/null

# 3. Analyser les timestamps
stat encrypted_files/*.encrypted

# 4. Vérifier l'historique bash
cat ~/.bash_history | grep ransomware

# 5. Rechercher dans les logs système
journalctl --since "2024-12-17 14:00" --until "2024-12-17 15:00"

# 6. Capturer le trafic en temps réel
sudo tcpdump -i lo -A port 5000 2>/dev/null
```
