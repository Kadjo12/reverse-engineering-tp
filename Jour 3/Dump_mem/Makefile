# Makefile pour le TP Dump Mémoire

CC = gcc
CFLAGS = -Wall -g -fno-stack-protector -z execstack -no-pie
TARGET = vulnerable_app
SRC = vulnerable_app.c

# Désactiver les protections pour faciliter l'analyse
# -fno-stack-protector: désactive la protection de stack
# -z execstack: stack exécutable
# -no-pie: désactive PIE (Position Independent Executable)
# -g: inclut les symboles de debug

all: $(TARGET)

$(TARGET): $(SRC)
	@echo "Compilation de l'application vulnérable..."
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "✓ Compilation terminée: $(TARGET)"
	@echo "  PID sera affiché au lancement"

debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

clean:
	@echo "Nettoyage..."
	rm -f $(TARGET) *.o dump.* core.* *.txt *.log
	@echo "✓ Nettoyage terminé"

run: $(TARGET)
	@echo "Lancement de l'application..."
	./$(TARGET)

help:
	@echo "Makefile pour TP Dump Mémoire"
	@echo ""
	@echo "Cibles disponibles:"
	@echo "  make          - Compile le programme"
	@echo "  make run      - Compile et lance le programme"
	@echo "  make debug    - Compile avec symboles de debug"
	@echo "  make clean    - Nettoie les fichiers générés"
	@echo "  make help     - Affiche cette aide"

.PHONY: all clean run debug help
