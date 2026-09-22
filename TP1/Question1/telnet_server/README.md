# IMC Telnet
Programme C qui sert un serveur simple TCP, Telnet pour calculer l'IMC d'un utilisateur.

## Pour acceder au service
**Pre-requis:**
- Telnet
- Une connexion Internet

Il suffit d'executer la commande dans un terminal:
```bash
telnet imc.niaksi.me 2324
```

Le port inital du protocol Telnet (23) a ete changer par un port custom (2324) pour des question de securite.

## Heberger le serveur Telnet
Pour compiler le programme avec GCC, faire:
```bash
gcc telnet_server.c -o telnet_server
```
Pour lancer le programme:
```bash
./telnet_server
```
