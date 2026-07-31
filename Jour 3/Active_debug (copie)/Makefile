# Makefile pour Active Debug Target

CC = gcc
CFLAGS = -g -fno-pie -no-pie -Wall
TARGET = active_target
SRC = active_target.c

all: $(TARGET)
	@echo "✅ Compilation terminée!"
	@echo "Pour lancer: ./$(TARGET)"
	@echo "Pour débugger: sudo gdb -p \$$(pgrep $(TARGET))"

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
	@echo "🧹 Nettoyage effectué"

run: $(TARGET)
	./$(TARGET)

debug: $(TARGET)
	gdb ./$(TARGET)

help:
	@echo "Makefile pour Active Debug Target"
	@echo ""
	@echo "Commandes disponibles:"
	@echo "  make          - Compile le programme"
	@echo "  make run      - Compile et lance le programme"
	@echo "  make debug    - Lance le programme dans GDB"
	@echo "  make clean    - Supprime les fichiers compilés"
	@echo "  make help     - Affiche cette aide"
	@echo ""
	@echo "Workflow typique:"
	@echo "  Terminal 1:  make run"
	@echo "  Terminal 2:  sudo gdb -p \$$(pgrep active_target)"

.PHONY: all clean run debug help
