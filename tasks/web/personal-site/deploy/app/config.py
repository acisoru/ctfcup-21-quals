import os
import socket


def configure(app):
    app.secret_key = os.getenv('FLASK_SECRET_KEY') or 'reallystrongsecretkey'
    app.config['DATABASE'] = os.path.abspath(os.path.join(os.path.dirname(__file__), 'example.db'))
    app.config['REDIS_HOST'] = os.getenv('REDIS_HOST') or '127.0.0.1'
    app.config['REDIS_PORT'] = os.getenv('REDIS_PORT') or 6379
    app.config['REDIS_URL'] = 'redis://{}:{}'.format(app.config['REDIS_HOST'], app.config['REDIS_PORT'])
    app.config.update(
        CELERY_BROKER_URL=app.config['REDIS_URL'],
        CELERY_RESULT_BACKEND=app.config['REDIS_URL']
    )
    app.config['ADMIN_LOGIN'] = os.getenv('ADMIN_LOGIN') or 'admin'
    app.config['ADMIN_PASSWORD'] = os.getenv('ADMIN_PASSWORD') or 'password'
    app.config['FLAG'] = os.getenv('FLAG') or 'flag{flag}'
    app.config['HOST'] = os.getenv('HOST') or '127.0.0.1:5000'
    app.config['CHROME_PATH'] = os.getenv('CHROME_PATH') or '/usr/bin/google-chrome'
    app.config['PORT'] = os.getenv('PORT') or '5321'
    chrome_host = os.getenv('CHROME_HOST') or 'chrome'
    app.config['CHROME_HOST'] = socket.getaddrinfo(chrome_host, 0)[0][4][0]
    # '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome'
    # Disable security.
    app.config.update(
        SESSION_COOKIE_SECURE=False,
        SESSION_COOKIE_HTTPONLY=True,
    )

    return app
