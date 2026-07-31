#!/usr/bin/env bash
# provision.sh — installe la boîte à outils RE-Lab (headless / CLI).
# Idempotent : relançable sans casse. Testé sur bento/ubuntu-22.04.
set -euo pipefail
export DEBIAN_FRONTEND=noninteractive

echo "[*] Mise à jour des dépôts..."
apt-get update -y

echo "[*] Outils système & build..."
apt-get install -y \
  build-essential gcc-multilib gdb \
  binutils file xxd hexedit bsdmainutils \
  git curl wget unzip p7zip-full jq \
  python3 python3-pip python3-venv \
  ltrace strace \
  yara \
  nodejs npm

echo "[*] radare2 (paquet .deb officiel — pas de candidat apt ; build source exigerait meson/ninja)..."
if ! command -v r2 >/dev/null 2>&1; then
  R2_VER=6.1.8
  wget -q "https://github.com/radareorg/radare2/releases/download/${R2_VER}/radare2_${R2_VER}_amd64.deb" -O /tmp/radare2.deb
  apt-get install -y /tmp/radare2.deb || { dpkg -i /tmp/radare2.deb; apt-get install -yf; }
  rm -f /tmp/radare2.deb
fi

echo "[*] Outils Python (pwntools, capstone, ropgadget)..."
pip3 install --no-cache-dir pwntools capstone ropgadget 2>/dev/null || \
  pip3 install --no-cache-dir --break-system-packages pwntools capstone ropgadget

echo "[*] pwndbg (surcouche gdb)..."
if [ ! -d /opt/pwndbg ]; then
  git clone --depth 1 https://github.com/pwndbg/pwndbg /opt/pwndbg
  ( cd /opt/pwndbg && ./setup.sh ) || echo "[!] pwndbg setup a échoué — voir README (installation manuelle)."
fi
grep -q pwndbg /home/vagrant/.gdbinit 2>/dev/null || echo "source /opt/pwndbg/gdbinit.py" >> /home/vagrant/.gdbinit

echo "[*] WABT (WebAssembly Binary Toolkit : wasm2wat, wasm-decompile)..."
apt-get install -y wabt || echo "[!] wabt indispo via apt — voir README (release GitHub)."

echo "[*] .NET SDK 8 + ilspycmd (décompilation C# cross-platform)..."
if ! command -v dotnet >/dev/null 2>&1; then
  wget -q https://packages.microsoft.com/config/ubuntu/22.04/packages-microsoft-prod.deb -O /tmp/ms.deb
  dpkg -i /tmp/ms.deb
  apt-get update -y
  apt-get install -y dotnet-sdk-8.0
fi
# ilspycmd 8.2.x cible net6.0 : sans roll-forward il s'installe mais refuse de démarrer
# ("Framework 'Microsoft.NETCore.App', version '6.0.0' not found").
# On ne pose PAS dotnet-runtime-6.0 : le paquet Ubuntu s'installe dans /usr/lib/dotnet,
# invisible pour le host Microsoft (/usr/share/dotnet). LatestMajor le fait tourner sur le runtime 8.
export DOTNET_ROLL_FORWARD=LatestMajor
# ilspycmd installé pour l'utilisateur vagrant.
# Version épinglée : la 9.x exige .NET 9 → paquet cassé sur SDK 8 ("DotnetToolSettings.xml").
ILSPY_VER=8.2.0.7535
sudo -u vagrant bash -lc "dotnet tool install -g ilspycmd --version ${ILSPY_VER} 2>/dev/null || dotnet tool update -g ilspycmd --version ${ILSPY_VER}" || true
# sfextract : extrait les DLL managées d'un bundle single-file (étape 2 du TP A1).
# ilspycmd ne sait pas lire un bundle : sans sfextract, le TP décompilation est bloqué.
sudo -u vagrant bash -lc "dotnet tool install -g sfextract 2>/dev/null || dotnet tool update -g sfextract" || true
grep -q '.dotnet/tools' /home/vagrant/.bashrc || \
  echo 'export PATH="$PATH:$HOME/.dotnet/tools"' >> /home/vagrant/.bashrc
grep -q 'DOTNET_ROLL_FORWARD' /home/vagrant/.bashrc || \
  echo 'export DOTNET_ROLL_FORWARD=LatestMajor' >> /home/vagrant/.bashrc

# Les outils .NET globaux (ilspycmd 8.2.x, sfextract) sont compilés pour net6.0 : on autorise
# le roll-forward vers le runtime 8 dans leur runtimeconfig. La variable d'environnement seule
# ne suffit pas : ~/.bashrc n'est pas lu par les shells non interactifs (scripts, vagrant ssh -c).
sudo -u vagrant python3 - <<'PY'
import glob, json
pat = '/home/vagrant/.dotnet/tools/.store/*/*/*/*/tools/*/any/*.runtimeconfig.json'
for p in glob.glob(pat):
    with open(p) as f:
        cfg = json.load(f)
    cfg.setdefault('runtimeOptions', {})['rollForward'] = 'LatestMajor'
    with open(p, 'w') as f:
        json.dump(cfg, f, indent=2)
    print(f'[*] rollForward=LatestMajor -> {p}')
PY

echo "[*] (Optionnel) Ghidra headless — analyzeHeadless — décommenter si besoin."
# GHIDRA_VER=11.1.2 ; ... (voir README : lourd, réservé aux bonus)

echo "[*] Arborescence des labs..."
sudo -u vagrant mkdir -p /home/vagrant/labs/{A_re-malware,B_web-wasm,C_memoire,D_protections,E_projet,cibles}
sudo -u vagrant mkdir -p /home/vagrant/labs/A_re-malware/lab-sandbox

# Déploiement des cibles fournies (shared/cibles/ -> ~/labs/cibles/).
# La COPIE vers ext4 est indispensable : un bundle .NET single-file exécuté depuis le
# dossier synchronisé (vboxsf côté VirtualBox, HGFS côté VMware) ne détecte pas son
# propre bundle et refuse de démarrer.
if [ -d /home/vagrant/shared/cibles ] && [ -n "$(ls -A /home/vagrant/shared/cibles 2>/dev/null)" ]; then
  echo "[*] Déploiement des cibles fournies dans ~/labs/cibles/ ..."
  for f in /home/vagrant/shared/cibles/*; do
    [ -f "$f" ] || continue
    install -o vagrant -g vagrant -m 755 "$f" "/home/vagrant/labs/cibles/$(basename "$f")"
    echo "    -> $(basename "$f")"
  done
else
  echo "[!] Aucune cible dans ~/shared/cibles — les TP A1/A2/A3 en auront besoin."
fi

# Script de vérification des outils.
cat > /home/vagrant/labs/check_tools.sh <<'EOF'
#!/usr/bin/env bash
echo "=== Vérification des outils RE-Lab ==="
for t in gcc gdb objdump readelf nm strings file xxd radare2 r2 python3 pwn yara node wasm2wat dotnet ilspycmd; do
  if command -v "$t" >/dev/null 2>&1; then printf "  [OK]  %-10s %s\n" "$t" "$(command -v $t)"; else printf "  [!!]  %-10s MANQUANT\n" "$t"; fi
done
echo "ASLR (0=off,2=on) : $(cat /proc/sys/kernel/randomize_va_space)"
EOF
chmod +x /home/vagrant/labs/check_tools.sh
chown -R vagrant:vagrant /home/vagrant/labs

echo "[*] Provisioning terminé."
echo "[*] Lancer : ~/labs/check_tools.sh"
