FROM keltecc/nsjail:latest

RUN apt update

RUN useradd -M -s /bin/false ctf

COPY entrypoint.sh /tmp/
RUN chmod 555 /tmp/entrypoint.sh

COPY mydocs flag.txt /var/service/
COPY flag.txt /tmp/
RUN chmod ugo+r /var/service/ && chmod ugo-wx /var/service/
RUN chmod ugo+r /tmp/flag.txt && chmod ugo-wx /tmp/flag.txt
RUN chmod ugo+rx /var/service/mydocs && chmod ugo-w /var/service/mydocs

USER ctf

ENTRYPOINT /tmp/entrypoint.sh
