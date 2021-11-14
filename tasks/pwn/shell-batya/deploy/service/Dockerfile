FROM keltecc/nsjail:latest

RUN apt update && apt install -y libseccomp-dev

RUN useradd -M -s /bin/false ctf

COPY entrypoint.sh /tmp/

RUN chmod 555 /tmp/entrypoint.sh

COPY shell-batya flag.txt ld-linux-x86-64.so.2  libc.so.6  libseccomp.so.2 /var/service/

USER ctf

ENTRYPOINT /tmp/entrypoint.sh
