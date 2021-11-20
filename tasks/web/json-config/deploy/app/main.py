import os
import json
import os
import secrets
import shutil
import subprocess
import tempfile

import redis
from celery import chord
from celery.result import AsyncResult
from flask import Flask, render_template, redirect, request, flash, abort, jsonify, send_from_directory

from cel_helper import make_celery
from config import configure

app = Flask(__name__)

app = configure(app)

celery = make_celery(app)
red = redis.Redis(host=app.config['REDIS_HOST'], port=app.config['REDIS_PORT'], db=0)


def get_pairs(setting_name):
    res = red.hgetall(setting_name)
    return {k.decode(): v.decode() for k, v in res.items()}


@app.get('/')
def index_page():
    return render_template('index.html')


@app.get('/domain/<string:domain>')
def get_domain(domain):
    settings_list = [x.decode() for x in red.lrange(domain, 0, -1)]
    return render_template('domain.html', settings=settings_list, domain=domain)


@app.post('/domain/<string:domain>')
def add_setting(domain):
    setting = request.form.get('setting')
    if red.exists(setting):
        flash('Key already exists')
        return redirect('/domain/' + domain)
    red.lpush(domain, setting)
    return redirect('/domain/' + domain)


@app.post('/domain/<string:domain>/flush')
def flush_domain(domain):
    red.ltrim(domain, -1, 1)
    red.lpop(domain)
    return redirect('/domain/' + domain)


@app.post('/domain/<string:domain>/dump')
def dump_domain(domain):
    temp_dir = tempfile.mkdtemp(dir=app.config['DATA_DIR'])
    settings_list = [x.decode() for x in red.lrange(domain, 0, -1)]
    out_file = os.path.join(app.config['DATA_DIR'], secrets.token_hex(10) + '.zip')
    c = chord((write_settings_to_file.si(s, temp_dir) for s in settings_list), body=zip_dir.si(out_file, temp_dir))
    res = c.apply_async()
    return redirect('/dump/result/' + res.id)


@app.get('/settings/<string:domain>/<string:setting_name>')
def get_from_settings(domain, setting_name):
    pairs = [(x, y) for x, y in get_pairs(setting_name).items()]
    return render_template('settings.html', domain=domain, setting=setting_name, pairs=pairs)


@app.post('/settings/<string:domain>/<string:setting_name>')
def set_kv_setting(domain, setting_name):
    k = request.form.get('k')
    v = request.form.get('v')
    red.hset(setting_name, k, v)
    return redirect(request.url)


@app.get('/settings/<string:domain>/<string:setting_name>/<string:key>/delete')
def del_kv_setting(domain, setting_name, key):
    red.hdel(setting_name, key)
    return redirect("/settings/{}/{}".format(domain, setting_name))


@app.get('/api/settings/<string:domain>/<string:setting_name>')
def get_api_settings(domain, setting_name):
    return jsonify(get_pairs(setting_name))


@app.get('/dump/result/<string:tid>')
def get_dump_result(tid):
    res = celery.AsyncResult(tid)
    ctx = {'ready': False}
    if res.ready():
        ctx['ready'] = True
        d = res.get()
        ctx['log'] = d['log']
        ctx['path'] = d['out']
    return render_template('dump_result.html', **ctx)


@app.route("/dumps/download/<filename>")
def get_image(filename):
    try:
        return send_from_directory(app.config["DATA_DIR"], path=filename, filename=filename, as_attachment=True)
    except FileNotFoundError:
        abort(404)


@celery.task()
def write_settings_to_file(setting_name, dir):
    with open(os.path.join(dir, setting_name + '.json'), 'w') as f:
        f.write(json.dumps(get_pairs(setting_name)))


@celery.task()
def zip_dir(out_file, dir):
    try:
        x = subprocess.check_output(f'zip -r -j {out_file} {dir}/*', shell=True)
        log = x.decode()
    except Exception as e:
        log = str(e)
    shutil.rmtree(dir)

    _, out_file = os.path.split(out_file)
    return {'out': out_file, 'log': log}
