FROM python:3.12-alpine
WORKDIR /app
COPY public/ ./public/
EXPOSE 8088
CMD ["python","-m","http.server","8088","--directory","public","--bind","0.0.0.0"]
