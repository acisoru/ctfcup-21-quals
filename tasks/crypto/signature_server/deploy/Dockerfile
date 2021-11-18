FROM python:2-alpine

ADD ./app /app
WORKDIR /app

RUN apk add --no-cache python-dev socat gcc alpine-sdk gmp-dev mpc1-dev mpfr-dev
RUN python -m pip install gmpy2 ecdsa
RUN chmod +x /app/run.sh /app/server.py

ENTRYPOINT [ "sh", "run.sh" ]

