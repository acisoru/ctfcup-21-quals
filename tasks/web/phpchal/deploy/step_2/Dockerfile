FROM golang:1.17-buster

RUN apt update && apt install -y php

WORKDIR /app

RUN echo "CUP{4f7fb992b912c1f1d1c69b1907001f46}" > /flag
COPY main.go .
COPY php_scripts ./php_scripts


RUN go build -o app -a main.go

CMD ["/app/app"]