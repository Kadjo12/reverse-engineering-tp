# Rapport d'Analyse Forensique
# TP Jour 5 — Challenge 5 | Djimy DONKENG | 2026-07-31

## Objectif

Reconstituer la timeline complète de l'attaque ransomware à partir des artéfacts
laissés sur le système après l'infection.

## Méthodologie

Analyse en trois phases :
1. **Collecte** — identifier les artéfacts (fichiers, logs, réseau)
2. **Corrélation** — relier les événements par timestamps
3. **Reconstruction** — établir la chronologie précise de l'attaque

## Phase 1 — Collecte des Artéfacts

### Artéfacts Fichiers
```bash
find ~/labs -name "*.encrypted" -o -name "README_RANSOM.txt" 2>/dev/null
```

Trouvés :
- `encrypted_files/document.txt.encrypted`
- `encrypted_files/rapport.txt.encrypted`
- `encrypted_files/clients.txt.encrypted`
- `encrypted_files/README_RANSOM.txt`

### Artéfacts Réseau
```bash
cat Etape3_AnalyseC2/network_capture.log
```

Deux requêtes HTTP POST vers localhost:5000 identifiées.

### Artéfacts Processus
```bash
cat ~/.bash_history | grep -E "ransomware|./ransom"
```

### Artéfacts Binaire
```bash
file Etape1_AnalyseStatique/ransomware
strings Etape1_AnalyseStatique/ransomware | grep -E "http|api|VICTIM|key"
md5sum Etape1_AnalyseStatique/ransomware
```

## Phase 2 — Corrélation Temporelle

Tous les fichiers `.encrypted` ont le même timestamp de modification :
→ L'attaque a duré **moins d'une seconde**.

Le timestamp du victim_id `VICTIM_student-vm_1734444225` correspond à :
```bash
date -d @1734444225
# Tue Dec 17 14:23:45 UTC 2024
```

## Phase 3 — Reconstruction

Voir `timeline.md` pour la timeline complète.

## Conclusions Forensiques

### Ce qu'on a pu identifier
- Binaire exact (hash MD5)
- Timestamp de l'infection (à la seconde)
- Fichiers touchés (par extension .encrypted)
- Communications C2 (network_capture.log)
- Clé de chiffrement (visible via strings et dans les logs)

### Ce qu'on n'a pas pu identifier
- Comment le ransomware est arrivé sur la machine (vecteur d'infection initial)
- Si d'autres systèmes ont été touchés sur le réseau
- Identité de l'attaquant (le C2 est sur localhost dans ce TP)

### Récupération des Fichiers

Méthode 1 — Clé depuis les logs réseau :
```bash
grep '"key"' Etape3_AnalyseC2/network_capture.log
# "key": "Sup3rS3cr3tK3y2024!"
python3 Etape4_Recouvrement/recovery_tool.py encrypted_files/
```

Méthode 2 — Extraction GDB (si binaire avec symboles) :
```bash
gdb ./ransomware
(gdb) x/s encryption_key
```

Méthode 3 — Strings (même sans symboles) :
```bash
strings ransomware | grep -E "[A-Z][a-z0-9!@#]{10,}"
```

## Recommandations Préventives

1. **Backup 3-2-1** : 3 copies, 2 supports différents, 1 hors site
2. **Surveillance réseau** : Alertes IDS sur POST vers ports non standard
3. **Principe du moindre privilège** : L'utilisateur ne devrait pas pouvoir
   exécuter des binaires inconnus
4. **EDR (Endpoint Detection and Response)** : Détection comportementale
   (création massive de fichiers chiffrés en < 1 seconde)
5. **Journalisation** : Activer auditd pour tracer toutes les exécutions
