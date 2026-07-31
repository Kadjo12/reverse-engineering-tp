#!/usr/bin/env python3
"""
Recovery Tool - Déchiffreur de Ransomware
TP Jour 5 - Étape 4

Ce script déchiffre les fichiers chiffrés par le ransomware éducatif.
Complétez les sections marquées TODO.
"""

import os
import sys
from pathlib import Path

# ============================================================================
# TODO 1: Compléter avec la clé extraite à l'Étape 2
# ============================================================================
# La clé est un objet bytes, ex: b"exemple"
ENCRYPTION_KEY = b"VOTRE_CLE_ICI"  # Remplacer par la vraie clé

# Longueur de la clé (doit correspondre à KEY_LENGTH du ransomware)
KEY_LENGTH = len(ENCRYPTION_KEY)


def xor_decrypt(data: bytes, key: bytes) -> bytes:
    """
    Déchiffre les données avec XOR.

    Note: XOR est réversible, donc encrypt = decrypt

    Args:
        data: Données chiffrées
        key: Clé de chiffrement

    Returns:
        Données déchiffrées
    """
    # ========================================================================
    # TODO 2: Compléter la fonction de déchiffrement XOR
    # ========================================================================
    # Indice: la clé est plus courte que les données -> elle "boucle"
    #         key[i % len(key)]

    decrypted = bytearray()
    key_len = len(key)

    for i, byte in enumerate(data):
        # TODO 2a: Calculer le byte déchiffré (XOR entre le byte et la clé)
        decrypted_byte = ___________________________

        # TODO 2b: Ajouter le byte au résultat
        ___________________________

    return bytes(decrypted)


def decrypt_file(encrypted_filepath: str, output_filepath: str = None) -> bool:
    """
    Déchiffre un fichier .encrypted

    Args:
        encrypted_filepath: Chemin du fichier chiffré
        output_filepath: Chemin de sortie (optionnel)

    Returns:
        True si succès, False sinon
    """
    # ========================================================================
    # TODO 3: Implémenter la logique de déchiffrement
    # ========================================================================

    # Vérifier que le fichier existe
    if not os.path.exists(encrypted_filepath):
        print(f"❌ Erreur: Fichier introuvable: {encrypted_filepath}")
        return False

    # Déterminer le nom du fichier de sortie
    if output_filepath is None:
        # TODO 3a: Retirer l'extension ".encrypted" du nom de fichier
        #          Sinon, ajouter ".decrypted" au nom
        #          Indice: str.endswith() et le slicing [:-N]
        if _______________________________________:
            output_filepath = _______________________________________
        else:
            output_filepath = _______________________________________

    try:
        # TODO 3b: Lire le fichier chiffré en binaire ('rb')
        print(f"📖 Lecture de {encrypted_filepath}...")
        with open(_______________, _______) as f:
            encrypted_data = _______________

        # TODO 3c: Déchiffrer les données (appeler xor_decrypt)
        print(f"🔓 Déchiffrement en cours...")
        decrypted_data = _______________________________________

        # TODO 3d: Écrire le fichier déchiffré en binaire ('wb')
        print(f"💾 Sauvegarde dans {output_filepath}...")
        with open(_______________, _______) as f:
            _______________________________________

        print(f"✅ Succès: {output_filepath}")
        return True

    except Exception as e:
        print(f"❌ Erreur lors du déchiffrement: {e}")
        return False


def find_encrypted_files(directory: str) -> list:
    """
    Trouve tous les fichiers .encrypted dans un répertoire

    Args:
        directory: Répertoire à scanner

    Returns:
        Liste des chemins de fichiers chiffrés
    """
    encrypted_files = []

    # ========================================================================
    # TODO 4: Scanner le répertoire pour trouver les fichiers .encrypted
    # ========================================================================
    # Indice: os.walk(directory) renvoie (root, dirs, files) pour chaque
    #         sous-répertoire. os.path.join(root, file) reconstruit le chemin.

    for root, dirs, files in _______________________:
        for file in files:
            # TODO 4a: Ne garder que les fichiers finissant par ".encrypted"
            if _______________________________________:
                # TODO 4b: Construire le chemin complet et l'ajouter à la liste
                full_path = _______________________________________
                _______________________________________

    return encrypted_files


def verify_key():
    """
    Vérifie que la clé a été correctement définie
    """
    if ENCRYPTION_KEY == b"VOTRE_CLE_ICI":
        print("❌ ERREUR: Vous devez remplacer ENCRYPTION_KEY avec la vraie clé !")
        print("   Indice: Allez voir l'Étape 2 où vous avez extrait la clé avec GDB")
        return False

    if len(ENCRYPTION_KEY) == 0:
        print("❌ ERREUR: La clé est vide !")
        return False

    print(f"✅ Clé configurée: {len(ENCRYPTION_KEY)} bytes")
    return True


def main():
    """
    Fonction principale
    """
    print("=" * 60)
    print("  Recovery Tool - Déchiffreur de Ransomware")
    print("  TP Jour 5 - Étape 4")
    print("=" * 60)
    print()

    # Vérifier la clé
    if not verify_key():
        sys.exit(1)

    # Vérifier les arguments
    if len(sys.argv) < 2:
        print("Usage:")
        print(f"  {sys.argv[0]} <fichier.encrypted>")
        print(f"  {sys.argv[0]} <répertoire>")
        print()
        print("Exemples:")
        print(f"  {sys.argv[0]} document.txt.encrypted")
        print(f"  {sys.argv[0]} ./encrypted_files/")
        sys.exit(1)

    target = sys.argv[1]

    # Si c'est un fichier
    if os.path.isfile(target):
        print(f"Mode: Déchiffrement d'un seul fichier")
        print()
        success = decrypt_file(target)
        sys.exit(0 if success else 1)

    # Si c'est un répertoire
    elif os.path.isdir(target):
        print(f"Mode: Déchiffrement de tous les fichiers du répertoire")
        print()

        # Trouver tous les fichiers chiffrés
        encrypted_files = find_encrypted_files(target)

        if not encrypted_files:
            print(f"❌ Aucun fichier .encrypted trouvé dans {target}")
            sys.exit(1)

        print(f"📋 {len(encrypted_files)} fichier(s) trouvé(s):")
        for f in encrypted_files:
            print(f"   - {f}")
        print()

        # Déchiffrer tous les fichiers
        success_count = 0
        fail_count = 0

        for encrypted_file in encrypted_files:
            if decrypt_file(encrypted_file):
                success_count += 1
            else:
                fail_count += 1
            print()

        # Résumé
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
