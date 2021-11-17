FROM python:3

ENV PYTHONUNBUFFERED=1

COPY wait-for-it.sh /tmp/wait-for-it.sh
COPY requirements.txt /tmp/requirements.txt
RUN pip install -r /tmp/requirements.txt

COPY init.py /tmp/init.py


ENTRYPOINT ["bash", "/tmp/wait-for-it.sh", "-t", "0", "db:9200", "--", "python", "/tmp/init.py"]