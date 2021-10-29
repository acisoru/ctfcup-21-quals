FROM keltecc/nsjail:latest

RUN apt update

RUN useradd -M -s /bin/false ctf

COPY entrypoint.sh /tmp/

RUN chmod 555 /tmp/entrypoint.sh

COPY mnogoetazhka_v1 flag.txt ld-linux-x86-64.so.2 libc.so.6 /var/service/

USER ctf

ENTRYPOINT /tmp/entrypoint.sh
