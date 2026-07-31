#!/usr/bin/env python3
"""
Vaccin Anti-Ransomware — Challenge 2
TP Jour 5 | Djimy DONKENG | 2026-07-31

Détecte et bloque l'exécution du ransomware pédagogique en surveillant :
  - Les processus suspects (nom "ransomware")
  - Les connexions réseau vers le C2 (port 5000)
  - La création de fichiers .encrypted
  - La création de README_RANSOM.txt
"""

import os
import sys
import time
import signal
import threading
import subprocess
from pathlib import Path
from datetime import datetime

# ─── Configuration ───────────────────────────────────────────────────────────

WATCH_DIRECTORY   = "./test_files"          # Dossier à surveiller
CHECK_INTERVAL    = 0.5                     # Secondes entre chaque vérification
LOG_FILE          = "vaccine.log"

# IOCs du ransomware
RANSOMWARE_PROCESS_NAMES = ["ransomware", "ransomware_stripped"]
RANSOMWARE_C2_PORT       = 5000
RANSOMWARE_FILE_EXT      = ".encrypted"
RANSOMWARE_NOTE          = "README_RANSOM.txt"

# ─── Couleurs console ─────────────────────────────────────────────────────────

GREEN  = "\033[92m"
RED    = "\033[91m"
YELLOW = "\033[93m"
BLUE   = "\033[94m"
RESET  = "\033[0m"

# ─── Logging ─────────────────────────────────────────────────────────────────

def log(msg, level="INFO"):
    timestamp = datetime.now().strftime("%H:%M:%S")
    colors = {"INFO": BLUE, "ALERT": RED, "OK": GREEN, "WARN": YELLOW}
    color = colors.get(level, RESET)
    line = f"[{timestamp}] [{level}] {msg}"
    print(f"{color}{line}{RESET}")
    with open(LOG_FILE, "a") as f:
        f.write(line + "\n")

# ─── Détection des processus ─────────────────────────────────────────────────

def get_running_processes():
    """Retourne la liste des processus actifs (pid, name)."""
    try:
        result = subprocess.run(
            ["ps", "aux"], capture_output=True, text=True
        )
        processes = []
        for line in result.stdout.splitlines()[1:]:
            parts = line.split()
            if len(parts) >= 11:
                pid  = int(parts[1])
                name = os.path.basename(parts[10])
                processes.append((pid, name, " ".join(parts[10:])))
        return processes
    except Exception:
        return []

def detect_ransomware_process():
    """Détecte si un processus ransomware est en cours d'exécution."""
    for pid, name, cmdline in get_running_processes():
        for ioc in RANSOMWARE_PROCESS_NAMES:
            if ioc in name or ioc in cmdline:
                return pid, name
    return None, None

def kill_process(pid):
    """Tue le processus ransomware par son PID."""
    try:
        os.kill(pid, signal.SIGKILL)
        log(f"Processus {pid} tué avec SIGKILL", "OK")
        return True
    except ProcessLookupError:
        log(f"Processus {pid} déjà terminé", "WARN")
        return False
    except PermissionError:
        log(f"Permission refusée pour tuer {pid} — essayez avec sudo", "ALERT")
        return False

# ─── Détection des fichiers ───────────────────────────────────────────────────

def scan_encrypted_files(directory):
    """Retourne la liste des fichiers .encrypted dans le dossier."""
    encrypted = []
    try:
        for root, _, files in os.walk(directory):
            for f in files:
                if f.endswith(RANSOMWARE_FILE_EXT) or f == RANSOMWARE_NOTE:
                    encrypted.append(os.path.join(root, f))
    except FileNotFoundError:
        pass
    return encrypted

# ─── Détection des connexions réseau ─────────────────────────────────────────

def detect_c2_connection():
    """Détecte une connexion active vers le port C2 (5000)."""
    try:
        result = subprocess.run(
            ["ss", "-tnp"], capture_output=True, text=True
        )
        for line in result.stdout.splitlines():
            if f":{RANSOMWARE_C2_PORT}" in line or f",{RANSOMWARE_C2_PORT} " in line:
                return True
    except Exception:
        pass
    return False

# ─── Backup préventif ─────────────────────────────────────────────────────────

def backup_directory(directory):
    """Crée une sauvegarde du dossier surveillé avant toute attaque."""
    backup_path = f"{directory}_backup_{int(time.time())}"
    try:
        import shutil
        shutil.copytree(directory, backup_path)
        log(f"Backup créé : {backup_path}", "OK")
        return backup_path
    except Exception as e:
        log(f"Backup échoué : {e}", "WARN")
        return None

def restore_from_backup(backup_path, target_path):
    """Restaure le dossier depuis le backup."""
    import shutil
    try:
        if os.path.exists(target_path):
            shutil.rmtree(target_path)
        shutil.copytree(backup_path, target_path)
        log(f"Restauration depuis {backup_path} réussie", "OK")
        return True
    except Exception as e:
        log(f"Restauration échouée : {e}", "ALERT")
        return False

# ─── Boucle principale de surveillance ───────────────────────────────────────

class RansomwareVaccine:
    def __init__(self, watch_dir):
        self.watch_dir   = watch_dir
        self.running     = True
        self.backup_path = None
        self.alerted     = False
        self.known_files = set()

    def start(self):
        log("Vaccin anti-ransomware démarré", "OK")
        log(f"Surveillance de : {os.path.abspath(self.watch_dir)}", "INFO")

        # Créer un backup préventif
        if os.path.exists(self.watch_dir):
            self.backup_path = backup_directory(self.watch_dir)
            self.known_files = set(scan_encrypted_files(self.watch_dir))

        log("En attente d'activité malveillante... (Ctrl+C pour arrêter)", "INFO")
        print()

        try:
            while self.running:
                self._check()
                time.sleep(CHECK_INTERVAL)
        except KeyboardInterrupt:
            log("Vaccin arrêté par l'utilisateur", "WARN")

    def _check(self):
        threat_detected = False

        # 1. Vérifier les processus
        pid, name = detect_ransomware_process()
        if pid:
            log(f"ALERTE ! Ransomware détecté : {name} (PID {pid})", "ALERT")
            log("Tentative de blocage...", "WARN")
            kill_process(pid)
            threat_detected = True

        # 2. Vérifier les connexions C2
        if detect_c2_connection():
            log(f"ALERTE ! Connexion vers le C2 (port {RANSOMWARE_C2_PORT}) détectée", "ALERT")
            threat_detected = True

        # 3. Vérifier les fichiers chiffrés
        current_encrypted = set(scan_encrypted_files(self.watch_dir))
        new_encrypted = current_encrypted - self.known_files
        if new_encrypted:
            for f in new_encrypted:
                log(f"ALERTE ! Fichier chiffré détecté : {f}", "ALERT")
            self.known_files = current_encrypted
            threat_detected = True

        # 4. Si menace détectée et backup disponible → restaurer
        if threat_detected and not self.alerted and self.backup_path:
            self.alerted = True
            log("Restauration automatique depuis le backup...", "WARN")
            time.sleep(1)  # Laisser le temps au processus d'être tué
            restore_from_backup(self.backup_path, self.watch_dir)

        elif not threat_detected and not self.alerted:
            # Afficher un point de vie toutes les 5 secondes
            pass  # silencieux si tout va bien


# ─── Point d'entrée ───────────────────────────────────────────────────────────

if __name__ == "__main__":
    watch = sys.argv[1] if len(sys.argv) > 1 else WATCH_DIRECTORY

    print(f"""
{GREEN}╔══════════════════════════════════════╗
║   VACCIN ANTI-RANSOMWARE v1.0        ║
║   TP Jour 5 — Challenge 2            ║
╚══════════════════════════════════════╝{RESET}

IOCs surveillés :
  • Processus : {RANSOMWARE_PROCESS_NAMES}
  • Port C2   : {RANSOMWARE_C2_PORT}
  • Extension : {RANSOMWARE_FILE_EXT}
  • Note      : {RANSOMWARE_NOTE}
""")

    vaccine = RansomwareVaccine(watch)
    vaccine.start()
