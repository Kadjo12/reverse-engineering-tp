# Makefile pour le Ransomware Pédagogique

CC = gcc
CFLAGS = -g -Wall -Wextra -O0
TARGET = ransomware

.PHONY: all clean test setup

all: $(TARGET)

$(TARGET): ransomware.c
	@echo "🔨 Compilation du ransomware..."
	$(CC) $(CFLAGS) -o $(TARGET) ransomware.c
	@echo "✅ Compilation réussie : ./$(TARGET)"
	@echo ""
	@echo "⚠️  ATTENTION : N'exécutez PAS ce programme sur vos fichiers personnels !"
	@echo "    Utilisez uniquement dans le dossier test_files/"

# Créer un environnement de test sécurisé
setup:
	@echo "📁 Création de l'environnement de test..."
	mkdir -p test_files
	echo "Ceci est un document confidentiel." > test_files/document.txt
	echo "Données importantes de l'entreprise" > test_files/data.txt
	echo "Rapport financier Q4 2024" > test_files/rapport.txt
	@echo "✅ Fichiers de test créés dans test_files/"

# Tester le ransomware dans un environnement isolé
test: $(TARGET) setup
	@echo "🧪 Test du ransomware (mode sécurisé)..."
	@echo ""
	./$(TARGET) ./test_files
	@echo ""
	@echo "📋 Fichiers chiffrés :"
	@ls -lh test_files/*.encrypted 2>/dev/null || echo "Aucun fichier chiffré"

# Nettoyer les fichiers compilés et chiffrés
clean:
	@echo "🧹 Nettoyage..."
	rm -f $(TARGET)
	rm -f README_RANSOM.txt
	rm -rf test_files
	@echo "✅ Nettoyage terminé"

# Compiler sans symboles de debug (pour rendre l'analyse plus difficile)
release: ransomware.c
	$(CC) -O2 -Wall -o $(TARGET) ransomware.c
	strip $(TARGET)
	@echo "✅ Version release compilée (sans symboles)"

# Afficher les informations sur le binaire
info: $(TARGET)
	@echo "📊 Informations sur le binaire :"
	@echo ""
	@echo "Type de fichier :"
	@file $(TARGET)
	@echo ""
	@echo "Taille :"
	@ls -lh $(TARGET)
	@echo ""
	@echo "Hashes :"
	@md5sum $(TARGET)
	@sha256sum $(TARGET)
	@echo ""
	@echo "Strings intéressantes :"
	@strings $(TARGET) | grep -E "http|key|encrypt|victim" | head -10

help:
	@echo "Commandes disponibles :"
	@echo "  make          - Compiler le ransomware"
	@echo "  make setup    - Créer l'environnement de test"
	@echo "  make test     - Compiler et tester"
	@echo "  make clean    - Nettoyer les fichiers"
	@echo "  make info     - Afficher les infos du binaire"
	@echo "  make release  - Compiler sans symboles (plus difficile)"
	@echo "  make help     - Afficher cette aide"
