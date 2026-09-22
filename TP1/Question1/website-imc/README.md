# IMC Website

Simple serveur http python, qui sert une page html

## Pour acceder au site
**Pre-requis:**
- Navigateur internet
- Une connexion internet

URL: [https://imc.niaksi.me/](https://imc.niaksi.me/)

## Herberger le serveur web

Afin de faciliter la portabiliter, le serveur web tourne dans un container docker

**Pre-requis:**
- Docker
- Docker Compose

Il suffit de lancer la commande:
```bash
cd website-imc/
docker compose up --build -d
```

Le serveur est alors disponible a l'adresse *localhost:8088*


