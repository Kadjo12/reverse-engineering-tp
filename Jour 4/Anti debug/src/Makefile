# Makefile pour le TP Anti-Debug
# Compile un binaire avec protections anti-debug

CC = gcc
CFLAGS = -g -fno-pie -no-pie -O0
TARGET = protected_target
SOURCE = protected_target.c

.PHONY: all clean check

all: $(TARGET)
	@echo ""
	@echo "✅ Compilation réussie !"
	@echo ""
	@echo "📦 Binaire créé: $(TARGET)"
	@echo "🔍 Avec symboles de debug (-g)"
	@echo "🎯 Adresses fixes (-fno-pie -no-pie)"
	@echo ""
	@echo "Pour lancer:"
	@echo "  ./$(TARGET)              # Exécution normale"
	@echo "  gdb ./$(TARGET)          # Debug avec GDB"
	@echo ""

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)
	@chmod +x $(TARGET)

clean:
	rm -f $(TARGET)
	@echo "🧹 Nettoyage effectué"

check:
	@echo "🔍 Vérification des protections..."
	@echo ""
	@echo "1. Symboles de debug présents:"
	@file $(TARGET) | grep -q "not stripped" && echo "   ✅ Yes" || echo "   ❌ No"
	@echo ""
	@echo "2. PIE désactivé:"
	@checksec --file=$(TARGET) 2>/dev/null | grep -i pie || readelf -h $(TARGET) | grep "Type:" | grep -q "EXEC" && echo "   ✅ PIE disabled (EXEC)" || echo "   ⚠️  PIE enabled"
	@echo ""
	@echo "3. Stack protections:"
	@checksec --file=$(TARGET) 2>/dev/null | grep -i canary || echo "   (checksec non installé, skip)"
	@echo ""

help:
	@echo "Commandes disponibles:"
	@echo "  make          # Compiler le programme"
	@echo "  make clean    # Nettoyer les fichiers compilés"
	@echo "  make check    # Vérifier les protections"
	@echo "  make help     # Afficher cette aide"
