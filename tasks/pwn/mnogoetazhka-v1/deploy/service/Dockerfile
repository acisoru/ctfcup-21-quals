FROM keltecc/nsjail:latest

RUN apt update

RUN useradd -M -s /bin/false ctf

COPY entrypoint.sh /tmp/

RUN chmod 555 /tmp/entrypoint.sh

COPY mnogoetazhka-v1 flag.txt ld-linux-x86-64.so.2 libc.so.6 /var/service/
COPY flag.txt /tmp/
RUN chmod ugo+r /tmp/flag.txt && chmod ugo-wx /tmp/flag.txt
RUN chmod ugo+r /var/service/flag.txt && chmod ugo-wx /var/service/flag.txt
RUN chmod ugo+rx /var/service/ld-linux-x86-64.so.2 && chmod ugo-w /var/service/ld-linux-x86-64.so.2
RUN chmod ugo+rx /var/service/mnogoetazhka-v1 && chmod ugo-w /var/service/mnogoetazhka-v1
RUN chmod ugo+rx /var/service/libc.so.6 && chmod ugo-w /var/service/libc.so.6

USER ctf

ENTRYPOINT /tmp/entrypoint.sh
