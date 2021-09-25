#!/usr/bin/env python

import hashlib
import random
import re
from typing import Tuple
from . import models as db_class
from . import config

def validate_password(password):
    if not re.fullmatch(r'[A-Za-z0-9!@#$%^&\*()_\-+=\\|/.,`~\'\":;<>?]{8,}', password):
        raise db_class.Insecure_Password()

def new_hash(plaintext)-> Tuple[str, str]:

    salt = str(random.randint(1000, 9999))
    salted = plaintext + salt
    hashed = hashlib.sha256(salted.encode("utf-8")).hexdigest()

    return hashed, salt

def hash(plaintext:str, salt:str) -> str:

    salted = plaintext + str(salt)
    hashed = hashlib.sha256(salted.encode("utf-8")).hexdigest()

    return hashed

def login_user(username: str, password: str):

    conn = config.connect_db(config.db_path)
    cursor = conn.cursor()
    data = cursor.execute('SELECT id, password, salt FROM user WHERE username =?', [username]).fetchone()
    del(cursor)
    del(conn)

    if data:
        if hash(password, data[2]) == data[1]:
            return db_class.create_user_object(config.connect_db(config.db_path), data[0])

    raise db_class.Invalid_Credentials()
