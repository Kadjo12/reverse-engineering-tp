#!/usr/bin/env python3
"""
Recovery Tool — Déchiffreur de Ransomware
TP Jour 5 — Étape 4

Clé obtenue via :
  - Analyse statique (strings sur le binaire) : "Sup3rS3cr3tK3y2024!"
  - Capture réseau (network_capture.log Étape 3) : champ "key" du POST /api/key
  - Debug GDB (x/s encryption_key) : confirmé identique

Algorithme : XOR avec clé cyclique (key[i % len(key)])
XOR est symétrique : déchiffrer = re-appliquer le même XOR avec la même clé.
"""

import os
import sys
from pathlib import Path

# ============================================================
# CLÉ EXTRAITE — Étape 2 (GDB) et confirmée Étape 3 (réseau)
# ============================================================
ENCRYPTION_KEY = b"Sup3rS3cr3tK3y2024!"
KEY_LENGTH = len(ENCRYPTION_KEY)  # 19 octets


def xor_decrypt(data: bytes, key: bytes) -> bytes:
    """
    Déchiffre (ou chiffre) les données avec XOR cyclique.

    XOR est symétrique : encrypt(encrypt(data, key), key) == data
    La clé boucle sur toute la longueur des données : key[i % len(key)]
    """
    decrypted = bytearray()
    key_len = len(key)
    for i, byte in enumerate(data):
        decrypted_byte = byte ^ key[i % key_len]   # XOR avec la clé cyclique
        decrypted.append(decrypted_byte)
    return bytes(decrypted)


def decrypt_file(encrypted_filepath: str, output_filepath: str = None) -> bool:
    """
    Déchiffre un fichier .encrypted et écrit le résultat déchiffré.

    Args:
        encrypted_filepath: Chemin du fichier chiffré (.encrypted)
        output_filepath   : Chemin de sortie (optionnel)

    Returns:
        True si succès, False sinon
    """
    if not os.path.exists(encrypted_filepath):
        print(f"❌ Erreur: Fichier introuvable: {encrypted_filepath}")
        return False

    # Déterminer le nom du fichier de sortie
    if output_filepath is None:
        if encrypted_filepath.endswith(".encrypted"):
            # Retirer l'extension .encrypted
            output_filepath = encrypted_filepath[:-len(".encrypted")]
        else:
            output_filepath = encrypted_filepath + ".decrypted"

    try:
        # Lire le fichier chiffré en binaire
        print(f"📖 Lecture de {encrypted_filepath}...")
        with open(encrypted_filepath, "rb") as f:
            encrypted_data = f.read()

        # Déchiffrer
        print(f"🔓 Déchiffrement en cours...")
        decrypted_data = xor_decrypt(encrypted_data, ENCRYPTION_KEY)

        # Écrire le fichier déchiffré
        print(f"💾 Sauvegarde dans {output_filepath}...")
        with open(output_filepath, "wb") as f:
            f.write(decrypted_data)

        print(f"✅ Succès: {output_filepath}")
        return True

    except Exception as e:
        print(f"❌ Erreur lors du déchiffrement: {e}")
        return False


def find_encrypted_files(directory: str) -> list:
    """
    Trouve tous les fichiers .encrypted dans un répertoire (récursif).
    """
    encrypted_files = []
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith(".encrypted"):
                full_path = os.path.join(root, file)
                encrypted_files.append(full_path)
    return encrypted_files


def verify_key():
    """Vérifie que la clé est bien configurée."""
    if ENCRYPTION_KEY == b"VOTRE_CLE_ICI":
        print("❌ ERREUR: Clé non configurée !")
        return False
    if len(ENCRYPTION_KEY) == 0:
        print("❌ ERREUR: Clé vide !")
        return False
    print(f"✅ Clé configurée: {len(ENCRYPTION_KEY)} octets — {ENCRYPTION_KEY.decode()}")
    return True


def main():
    print("=" * 60)
    print("  Recovery Tool — Déchiffreur de Ransomware")
    print("  TP Jour 5 — Étape 4")
    print("=" * 60)
    print()

    if not verify_key():
        sys.exit(1)

    if len(sys.argv) < 2:
        print("Usage:")
        print(f"  {sys.argv[0]} <fichier.encrypted>")
        print(f"  {sys.argv[0]} <répertoire>")
        sys.exit(1)

    target = sys.argv[1]

    if os.path.isfile(target):
        print(f"Mode: Déchiffrement d'un seul fichier")
        print()
        success = decrypt_file(target)
        sys.exit(0 if success else 1)

    elif os.path.isdir(target):
        print(f"Mode: Déchiffrement de tous les fichiers du répertoire")
        print()
        encrypted_files = find_encrypted_files(target)

        if not encrypted_files:
            print(f"❌ Aucun fichier .encrypted trouvé dans {target}")
            sys.exit(1)

        print(f"📋 {len(encrypted_files)} fichier(s) trouvé(s):")
        for f in encrypted_files:
            print(f"   - {f}")
        print()

        success_count = 0
        fail_count = 0
        for encrypted_file in encrypted_files:
            if decrypt_file(encrypted_file):
                success_count += 1
            else:
                fail_count += 1
            print()

        print("=" * 60)
        print(f"✅ Fichiers déchiffrés avec succès: {success_count}")
        if fail_count > 0:
            print(f"❌ Échecs: {fail_count}")
        print("=" * 60)
        sys.exit(0 if fail_count == 0 else 1)

    else:
        print(f"❌ Erreur: {target} n'est ni un fichier ni un répertoire")
        sys.exit(1)


if __name__ == "__main__":
    main()
