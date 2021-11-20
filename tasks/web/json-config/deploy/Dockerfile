FROM python:3.9.6-slim-bullseye

RUN apt update && apt -y install zip

WORKDIR /app

COPY app/requirements.txt requirements.txt

RUN pip3 install -r requirements.txt

COPY app .


CMD gunicorn main:app --bind 0.0.0.0:5321 --worker-class gevent --worker-connections 256
