FROM python:3.12-slim

WORKDIR /app

COPY app/server.py .

ENV PORT=8082

EXPOSE 8082

CMD ["python", "server.py"]