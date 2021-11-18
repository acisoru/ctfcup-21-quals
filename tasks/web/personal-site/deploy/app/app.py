import asyncio
import base64
import os
import secrets
import selectors
import time

from celery.result import AsyncResult
from flask import Flask, g, session, render_template, redirect, request, flash, abort

from cel_helper import make_celery
from admin_sim import AdminSimulator
from db import DB, User
from config import configure

app = Flask(__name__)

app = configure(app)

celery = make_celery(app)


# DB Connection create
def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = DB(app.config['DATABASE'])
    return db


def get_nonce():
    nonce = getattr(g, '_nonce', None)
    if nonce is None:
        nonce = g._nonce = base64.b64encode(secrets.token_bytes(20)).decode()
    return nonce


@app.context_processor
def inject_basic_info():
    user = session.get('user', None)
    auth = True if user else False
    return dict(auth=auth, user=user)


@app.get('/')
def index_page():
    user = session.get('user', None)
    auth = True if user else False
    return render_template('index.html')


@app.get('/register')
def register_page():
    user = session.get('user', None)
    auth = True if user else False
    if auth:
        return redirect('/info')
    return render_template('register.html')


@app.post('/register')
def register_handle():
    user = session.get('user')
    auth = True if user else False
    if auth:
        return redirect('/info')
    username = request.form.get('username', '')
    password = request.form.get('password', '')
    if username == '' or password == '':
        flash('Empty login or password', category='error')
        return render_template('register.html'), 418
    db = get_db()
    if db.find_user(username) is not None:
        flash('User already exists', category='error')
        return render_template('register.html'), 418
    new_user = db.add_user(username, password)
    session['user'] = new_user
    flash('Succesfully registered!')
    return redirect('/info')


@app.get('/login')
def login_page():
    user = session.get('user', None)
    auth = True if user else False
    if auth:
        return redirect('/info')
    return render_template('login.html', auth=auth, user=user)


@app.post('/login')
def login_handle():
    user = session.get('user', None)
    auth = True if user else False
    if auth:
        return redirect('/info')
    username = request.form.get('username', '')
    password = request.form.get('password', '')
    db = get_db()
    user = db.find_user(username)
    if user is None or user.password != password:
        flash('No such user', category='error')
        return render_template('login.html', auth=auth, user=user), 418
    session['user'] = user
    flash('Succesfully registered!')
    return redirect('/info')


@app.get('/info')
def messages_list():
    user = session.get('user', None)
    auth = True if user else False
    if not auth:
        return redirect('/')

    db = get_db()
    user = User._make(user)
    info = db.get_user_info(user.id)
    return render_template('info.html', auth=auth, user=user, info=info)


@app.post('/info')
def send_msg_page():
    user = session.get('user', None)
    auth = True if user else False
    if not auth:
        flash("Login first", category='error')
        return redirect('/')

    db = get_db()
    user = User._make(user)
    db.add_info(user.id, request.form.get('text'))
    return redirect('/info')


@app.get('/share/<uuid>')
def share_handler(uuid):
    db = get_db()
    info = db.get_info_by_uuid(uuid)
    if info is None:
        abort(404, "Not found")
        return
    user_info = db.get_user(info.user_id)

    return render_template('share_info.html', author=user_info.login, info=info)


@app.get('/report/<uuid>')
def report_page(uuid):
    user = session.get('user', None)
    auth = True if user else False
    if not auth:
        flash("Login first", category='error')
        return redirect('/')
    return render_template('report.html', uuid=uuid)


@app.post('/report/<uuid>')
def report_handle(uuid):
    user = session.get('user', None)
    auth = True if user else False
    if not auth:
        flash("Login first", category='error')
        return redirect('/')
    last_report_time = int(session.get('last_report_time', '0'))
    if time.time() - last_report_time < 30:
        flash('Please wait a few seconds', category='error')
        return redirect('/report/' + uuid)
    session['last_report_time'] = time.time()
    res = report_as_admin.apply_async((uuid,))
    return redirect('/report/result/' + res.id)


@app.get('/report/result/<string:tid>')
def get_dump_result(tid):
    res = AsyncResult(tid)
    ctx = {'ready': False, 'result': None}
    if res.ready():
        ctx['ready'] = True
        ctx['result'] = res.get()

    return render_template('dump-result.html', **ctx)


@app.get('/logout')
def logout_page():
    session.clear()
    return redirect('/')


@app.before_first_request
def init_db():
    db = get_db()
    db.init_db(flag=app.config['FLAG'], admin_account=app.config['ADMIN_LOGIN'],
               admin_password=app.config['ADMIN_PASSWORD'])


@app.before_request
def fill_nonce():
    g.nonce = get_nonce()


@app.after_request
def fill_headers(response):
    nonce = g.nonce
    response.headers['Content-Security-Policy'] = "script-src 'nonce-{nonce}'; " \
                                                  "style-src 'nonce-{nonce}'; " \
                                                  "object-src 'none';".format(
        nonce=nonce)

    return response


@celery.task()
def report_as_admin(uuid):
    ads = AdminSimulator(app.config['HOST'] + ":" + app.config['PORT'],
                         login=app.config['ADMIN_LOGIN'],
                         password=app.config['ADMIN_PASSWORD'],
                         exec_path=app.config['CHROME_PATH'],
                         chrome_host=app.config['CHROME_HOST'],
                         )
    try:
        asyncio.SelectorEventLoop(selectors.SelectSelector()).run_until_complete(ads.fetch_as_admin_chrome(uuid))
    except Exception as e:
        print(str(e))
        return str(e)
    else:
        return "VISITED"
