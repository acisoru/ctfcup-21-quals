FROM keltecc/nsjail:latest

RUN apt update

RUN useradd -M -s /bin/false ctf

COPY entrypoint.sh /tmp/

RUN chmod 555 /tmp/entrypoint.sh

COPY rds flag.txt /var/service/
COPY flag.txt /tmp/
RUN chmod ugo+r /var/service/flag.txt && chmod ugo-wx /var/service/flag.txt
RUN chmod ugo+r /tmp/flag.txt && chmod ugo-wx /tmp/flag.txt
RUN chmod ugo+rx /var/service/rds && chmod ugo-w /var/service/rds

USER ctf

ENTRYPOINT /tmp/entrypoint.sh
