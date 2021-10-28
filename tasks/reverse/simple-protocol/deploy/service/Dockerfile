FROM ubuntu:20.04

RUN apt-get update && apt-get upgrade -yyq
RUN apt-get install socat -yyq

ADD --chown=root ./server /server
ADD flag.txt /flag.txt

WORKDIR /

CMD ["socat", "TCP-LISTEN:9000,reuseaddr,fork,keepalive", "EXEC:./server"]