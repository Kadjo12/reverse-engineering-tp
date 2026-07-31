#!/bin/bash
# Script de test rapide du TP Anti-Debug

echo "=========================================="
echo "  Test du TP Anti-Debug"
echo "=========================================="
echo ""

cd "$(dirname "$0")/src"

# Test 1: Compilation
echo "[Test 1] Compilation..."
make clean >/dev/null 2>&1
make >/dev/null 2>&1
if [ -f "protected_target" ]; then
    echo "✅ Compilation OK"
else
    echo "❌ Compilation FAILED"
    exit 1
fi
echo ""

# Test 2: Exécution normale (en background, tuer après 2 sec)
echo "[Test 2] Exécution normale..."
./protected_target > /tmp/test_normal.txt 2>&1 &
PID=$!
sleep 2
kill -9 $PID 2>/dev/null
if grep -q "Ptrace check OK" /tmp/test_normal.txt; then
    echo "✅ Exécution normale OK"
else
    echo "❌ Exécution normale FAILED"
fi
echo ""

# Test 3: Détection sous GDB
echo "[Test 3] Détection sous GDB..."
timeout 5 gdb -batch -ex "run" ./protected_target 2>&1 > /tmp/test_gdb.txt
if grep -q "DEBUGGER DÉTECTÉ" /tmp/test_gdb.txt; then
    echo "✅ Détection GDB OK"
else
    echo "❌ Détection GDB FAILED"
fi
echo ""

# Test 4: Script de bypass
echo "[Test 4] Script de bypass..."
if [ -f "../exploits/bypass.gdb" ]; then
    timeout 10 gdb -batch -x ../exploits/bypass.gdb ./protected_target 2>&1 > /tmp/test_bypass.txt
    if grep -q "FLAG{" /tmp/test_bypass.txt; then
        FLAG=$(grep -o "FLAG{[^}]*}" /tmp/test_bypass.txt | head -1)
        echo "✅ Bypass OK - Flag: $FLAG"
    else
        echo "❌ Bypass FAILED"
    fi
else
    echo "⚠️  Script bypass.gdb non trouvé"
fi
echo ""

# Test 5: LD_PRELOAD hook (si compilé)
echo "[Test 5] LD_PRELOAD hook..."
if [ -f "../exploits/anti_debug_hook.so" ]; then
    LD_PRELOAD=../exploits/anti_debug_hook.so ./protected_target > /tmp/test_hook.txt 2>&1 &
    PID=$!
    sleep 2
    kill -9 $PID 2>/dev/null
    if grep -q "HOOK" /tmp/test_hook.txt && grep -q "Toutes les vérifications passées" /tmp/test_hook.txt; then
        echo "✅ Hook LD_PRELOAD OK"
    else
        echo "⚠️  Hook LD_PRELOAD à vérifier"
    fi
else
    echo "⚠️  Hook non compilé (cd exploits/ && gcc -shared -fPIC -o anti_debug_hook.so anti_debug_hook.c -ldl)"
fi
echo ""

echo "=========================================="
echo "  Tests terminés"
echo "=========================================="
rm -f /tmp/test_*.txt
