FROM keltecc/nsjail:latest

RUN apt update && apt install -y libseccomp-dev

RUN useradd -M -s /bin/false ctf

COPY entrypoint.sh /tmp/

RUN chmod 555 /tmp/entrypoint.sh

COPY shell-batya flag.txt ld-linux-x86-64.so.2  libc.so.6  libseccomp.so.2 /var/service/
COPY flag.txt /tmp/
RUN chmod ugo+r /tmp/flag.txt && chmod ugo-wx /tmp/flag.txt
RUN chmod ugo+r /var/service/flag.txt && chmod ugo-wx /var/service/flag.txt
RUN chmod ugo+rx /var/service/ld-linux-x86-64.so.2 && chmod ugo-w /var/service/ld-linux-x86-64.so.2
RUN chmod ugo+rx /var/service/shell-batya && chmod ugo-w /var/service/shell-batya
RUN chmod ugo+rx /var/service/libc.so.6 && chmod ugo-w /var/service/libc.so.6
RUN chmod ugo+rx /var/service/libseccomp.so.2 && chmod ugo-w /var/service/libseccomp.so.2

USER ctf

ENTRYPOINT /tmp/entrypoint.sh
