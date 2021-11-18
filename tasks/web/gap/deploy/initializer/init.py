import os
import time
from datetime import datetime
from opensearchpy import OpenSearch

INDEX_NAME = "smth_interesting"

es = OpenSearch([os.environ.get('DB_HOST', 'db') + ':9200'], http_auth=(os.environ.get('WRITER_USER', 'user'), os.environ.get('WRITER_PASS', 'pass')), verify_certs=False)

while True:
    try:
        print('create index')
        es.indices.create(index=INDEX_NAME, ignore=[400])
        break
    except Exception as e:
        print("Some error when connect to elastic.")
        print(e)
        print("Repeat after 5 second")
        time.sleep(5)


doc = {
    'value': 'flag',
    'text': os.environ.get("FLAG", "not set"),
    'timestamp': datetime.now(),
}
res = es.index(index=INDEX_NAME, id=1, body=doc, refresh=True)
print(res['result'])

res = es.get(index=INDEX_NAME, id=1)
print(res['_source'])

es.indices.refresh(index=INDEX_NAME)

print("All data created and checked")