# Makefile pour TP Final - Sécurisation de code
# Compilation des versions vulnérable et sécurisée

CC = gcc
CFLAGS_VULNERABLE = -w -g
CFLAGS_SECURE = -Wall -Wextra -g -fstack-protector-all -D_FORTIFY_SOURCE=2
LIBS = -lcrypto

# Cibles
VULNERABLE = vulnerable
SECURE = secure

# Couleurs pour l'affichage
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
NC = \033[0m # No Color

.PHONY: all vulnerable secure clean test help valgrind

# Par défaut : compiler uniquement la version vulnérable
all: vulnerable
	@echo "$(YELLOW)✓ Compilation terminée$(NC)"
	@echo "$(YELLOW)ℹ️  Pour compiler la version sécurisée : make secure$(NC)"

# Version vulnérable (pour analyse)
vulnerable: vulnerable.c
	@echo "$(RED)⚠️  Compilation de la version VULNÉRABLE...$(NC)"
	$(CC) $(CFLAGS_VULNERABLE) -o $(VULNERABLE) vulnerable.c
	@echo "$(RED)✓ $(VULNERABLE) compilé (contient des vulnérabilités)$(NC)"
	@echo ""

# Version sécurisée (à créer par l'étudiant)
secure: secure.c
	@echo "$(GREEN)🔒 Compilation de la version SÉCURISÉE...$(NC)"
	$(CC) $(CFLAGS_SECURE) -o $(SECURE) secure.c $(LIBS)
	@echo "$(GREEN)✓ $(SECURE) compilé avec succès$(NC)"
	@echo ""

# Compilation avec toutes les protections
secure-hardened: secure.c
	@echo "$(GREEN)🛡️  Compilation avec protections maximales...$(NC)"
	$(CC) $(CFLAGS_SECURE) -fPIE -pie -Wl,-z,relro,-z,now -o $(SECURE) secure.c $(LIBS)
	@echo "$(GREEN)✓ $(SECURE) compilé en mode durci$(NC)"

# Nettoyer les fichiers générés
clean:
	@echo "🧹 Nettoyage..."
	rm -f $(VULNERABLE) $(SECURE)
	rm -f *.o
	rm -f vgcore.*
	rm -f *.backup
	@echo "✓ Nettoyage terminé"

# Tests basiques
test: vulnerable
	@echo "$(YELLOW)🧪 Tests basiques de la version vulnérable$(NC)"
	@echo "Test 1: Lancement du programme"
	@echo "0" | ./$(VULNERABLE)
	@echo ""

test-secure: secure
	@echo "$(GREEN)🧪 Tests de la version sécurisée$(NC)"
	@echo "Test 1: Compilation sans warnings"
	@$(CC) $(CFLAGS_SECURE) -o $(SECURE) secure.c $(LIBS) 2>&1 | grep -i warning && echo "$(RED)⚠️  Warnings détectés$(NC)" || echo "$(GREEN)✓ Aucun warning$(NC)"
	@echo ""
	@echo "Test 2: Lancement du programme"
	@echo "0" | ./$(SECURE)

# Analyse mémoire avec Valgrind
valgrind: secure
	@echo "$(GREEN)🔍 Analyse mémoire avec Valgrind...$(NC)"
	@echo "0" | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(SECURE)

# Analyse mémoire rapide
valgrind-quick: secure
	@echo "$(GREEN)🔍 Analyse mémoire rapide...$(NC)"
	@echo "0" | valgrind --leak-check=yes ./$(SECURE)

# Vérifier les dépendances
check-deps:
	@echo "🔍 Vérification des dépendances..."
	@which gcc > /dev/null && echo "$(GREEN)✓ GCC installé$(NC)" || echo "$(RED)✗ GCC non trouvé$(NC)"
	@which valgrind > /dev/null && echo "$(GREEN)✓ Valgrind installé$(NC)" || echo "$(YELLOW)⚠️  Valgrind non trouvé (optionnel)$(NC)"
	@ldconfig -p | grep -q libcrypto && echo "$(GREEN)✓ OpenSSL installé$(NC)" || echo "$(RED)✗ OpenSSL non trouvé (requis pour SHA-256)$(NC)"
	@echo ""

# Afficher l'aide
help:
	@echo "════════════════════════════════════════════════════════"
	@echo "  Makefile - TP Final Sécurisation de Code"
	@echo "════════════════════════════════════════════════════════"
	@echo ""
	@echo "Cibles disponibles:"
	@echo "  $(YELLOW)make$(NC)              - Compile la version vulnérable"
	@echo "  $(GREEN)make secure$(NC)       - Compile la version sécurisée"
	@echo "  $(GREEN)make secure-hardened$(NC) - Compile avec protections maximales"
	@echo "  make clean         - Supprime les fichiers compilés"
	@echo "  make test          - Tests basiques (vulnérable)"
	@echo "  make test-secure   - Tests basiques (sécurisé)"
	@echo "  make valgrind      - Analyse mémoire complète"
	@echo "  make valgrind-quick - Analyse mémoire rapide"
	@echo "  make check-deps    - Vérifie les dépendances"
	@echo "  make help          - Affiche cette aide"
	@echo ""
	@echo "Exemples d'utilisation:"
	@echo "  1. Analyser le code vulnérable:"
	@echo "     $(YELLOW)make && ./vulnerable$(NC)"
	@echo ""
	@echo "  2. Compiler votre version sécurisée:"
	@echo "     $(GREEN)make secure$(NC)"
	@echo ""
	@echo "  3. Vérifier les fuites mémoire:"
	@echo "     $(GREEN)make valgrind$(NC)"
	@echo ""
	@echo "  4. Nettoyer et recompiler:"
	@echo "     make clean && make secure"
	@echo ""
	@echo "════════════════════════════════════════════════════════"

# Règle pour vérifier que secure.c existe
secure.c:
	@echo "$(RED)✗ Erreur: secure.c n'existe pas$(NC)"
	@echo "$(YELLOW)Vous devez d'abord créer secure.c en corrigeant vulnerable.c$(NC)"
	@echo "Commande suggérée: cp vulnerable.c secure.c"
	@exit 1
