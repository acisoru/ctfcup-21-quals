FROM python:2-alpine

ADD ./app /app
WORKDIR /app

RUN apk add --no-cache python-dev socat gcc alpine-sdk
RUN python -m pip install pycrypto
RUN chmod +x /app/run.sh /app/padding_server.py

ENTRYPOINT [ "sh", "run.sh" ]

