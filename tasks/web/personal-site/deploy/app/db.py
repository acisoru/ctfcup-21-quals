import sqlite3
import collections
import secrets
from typing import List, Dict

User = collections.namedtuple('User', ['id', 'login', 'password'])
Info = collections.namedtuple('Info', ['id', 'user_id', 'uuid', 'text'])


class DB:
    def __init__(self, sqlite_path):
        self.conn = sqlite3.connect(sqlite_path)
        self.users_db_init_sql = 'CREATE TABLE IF NOT EXISTS users (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,login TEXT,password TEXT)'
        self.infos_db_init_sql = 'CREATE TABLE IF NOT EXISTS info (id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,user_id INTEGER NOT NULL UNIQUE , uuid VARCHAR(200), text TEXT)'
        self.create_admin_sql = 'INSERT OR REPLACE INTO users VALUES (1, ?, ?)'
        self.add_admin_info = 'INSERT OR REPLACE INTO info VALUES (1, 1, ?, ?)'

    def init_db(self, admin_account='admin', flag='flag', admin_password='reallYS3cretPassw0rdN0L3ak'):
        cur = self.conn.cursor()
        cur.execute(self.users_db_init_sql)
        cur.execute(self.infos_db_init_sql)
        # cur.execute(self.create_tickets_sql)
        cur.execute(self.create_admin_sql, (admin_account, admin_password))
        cur.execute(self.add_admin_info, (secrets.token_hex(16), flag,))
        self.conn.commit()

    def find_user(self, username) -> User:
        cur = self.conn.cursor()
        cur.execute('SELECT * FROM users WHERE login = ?', (username,))
        u = cur.fetchone()
        if u:
            u = User._make(u)
        return u

    def get_user(self, uid) -> User:
        cur = self.conn.cursor()
        cur.execute('SELECT * FROM users WHERE id = ?', (uid,))
        u = cur.fetchone()
        if u:
            u = User._make(u)
        return u

    def add_user(self, username, password) -> User:
        cur = self.conn.cursor()
        cur.execute('INSERT INTO users (login, password) VALUES (?, ?)', (username, password))
        self.conn.commit()
        return User(cur.lastrowid, username, password)

    def add_info(self, user_id, text) -> Info:
        cur = self.conn.cursor()
        uuid = secrets.token_hex(16)
        cur.execute('INSERT OR REPLACE INTO info (user_id, uuid, text) VALUES (?,?, ?)', (user_id, uuid, text))
        self.conn.commit()
        return Info(cur.lastrowid, user_id, uuid, text)

    def get_user_info(self, user_id) -> Info:
        cur = self.conn.cursor()
        cur.execute('SELECT * FROM info WHERE user_id = ?', (user_id,))
        msg = cur.fetchone()
        if msg:
            msg = Info._make(msg)
        return msg

    def get_info_by_id(self, _id) -> Info:
        cur = self.conn.cursor()
        cur.execute('SELECT * FROM info WHERE id = ?', (_id,))
        msg = cur.fetchone()
        if msg:
            msg = Info._make(msg)
        return msg

    def get_info_by_uuid(self, uuid) -> Info:
        cur = self.conn.cursor()
        cur.execute('SELECT * FROM info WHERE uuid = ?', (uuid,))
        msg = cur.fetchone()
        if msg:
            msg = Info._make(msg)
        return msg
