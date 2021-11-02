import os
import pathlib


def configure(app):
    app.secret_key = os.getenv('FLASK_SECRET_KEY') or 'reallystrongsecretkey'
    app.config['REDIS_HOST'] = os.getenv('REDIS_HOST') or '127.0.0.1'
    app.config['REDIS_PORT'] = os.getenv('REDIS_PORT') or 6379
    app.config['REDIS_URL'] = 'redis://{}:{}'.format(app.config['REDIS_HOST'], app.config['REDIS_PORT'])
    app.config.update(
        CELERY_BROKER_URL=app.config['REDIS_URL'],
        CELERY_RESULT_BACKEND=app.config['REDIS_URL']
    )
    app.config['DATA_DIR'] = os.getenv('DATA_DIR') or '/tmp/data'
    pathlib.Path(app.config['DATA_DIR']).mkdir(parents=True, exist_ok=True)
    return app
