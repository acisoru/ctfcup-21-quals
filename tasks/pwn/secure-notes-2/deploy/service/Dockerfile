FROM ubuntu:latest

RUN apt-get update && apt-get dist-upgrade && apt -y install qemu-system-x86
RUN apt-get install cpio socat python3 python3-pip -yyq
RUN pip install requests

RUN mkdir -p /task/fs
RUN mkdir -p /task/kernel

COPY bzImage /task/kernel/
COPY compr.sh  decompress.sh initramfs.cpio.gz  run.sh /task/fs/
COPY deploy_manager.py /
ENV  PYTHONUNBUFFERED true

WORKDIR /

CMD ["socat", "TCP-LISTEN:9000,reuseaddr,fork,keepalive", "EXEC:./deploy_manager.py"]