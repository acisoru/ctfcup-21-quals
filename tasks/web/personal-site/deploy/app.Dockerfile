FROM python:3.9.6-slim-bullseye

RUN apt-get update && apt-get install -yq gnupg curl
RUN curl -fsSL https://dl.google.com/linux/linux_signing_key.pub | apt-key add - && \
    echo "deb http://dl.google.com/linux/chrome/deb/ stable main" >> /etc/apt/sources.list && \
    apt-get update && apt-get install -yq google-chrome-stable

WORKDIR /app

COPY app/requirements.txt requirements.txt

RUN pip3 install -r requirements.txt

COPY app .

CMD gunicorn app:app --bind 0.0.0.0:5321 --workers 3
