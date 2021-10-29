FROM keltecc/nsjail:latest

RUN apt update

RUN useradd -M -s /bin/false ctf

COPY entrypoint.sh /tmp/

RUN chmod 555 /tmp/entrypoint.sh

COPY mydocs /var/service/
COPY flag.txt /tmp/
RUN chmod ugo+r /tmp/flag.txt && chmod ugo-wx /tmp/flag.txt

USER ctf

ENTRYPOINT /tmp/entrypoint.sh
