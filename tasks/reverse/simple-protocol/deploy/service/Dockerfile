FROM ubuntu:20.04

RUN apt-get update && apt-get upgrade -yyq
RUN apt-get install socat -yyq

ADD --chown=root ./server /server
ADD flag.txt /flag.txt
RUN chmod ugo+rx /server && chmod ugo-w /server
RUN chmod ugo+r /flag.txt && chmod ugo-wx /flag.txt

WORKDIR /

CMD ["socat", "TCP-LISTEN:9000,reuseaddr,fork,keepalive", "EXEC:./server"]