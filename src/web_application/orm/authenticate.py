#!/usr/bin/env python

import hashlib
import random
import re
from typing import Tuple
from . import models as db_class
from . import config

def valid_password(password):
    # result = re.search("^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{8,}$", password)
    result = "true"
    if result:
        return True
    else:
        return False

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
    data = cursor.execute('SELECT id, role, password, salt FROM user WHERE username =?', [username]).fetchone()
    print(data)

    if data: # if a user with that username exists

        hashed = hash(password, data[3]) # generate the has of the entered password with the salt used to hash the original
        if hashed == data[2]: # if the hash equals the hash stored in the db

            return db_class.create_user_object(conn, data[0])

    raise db_class.Invalid_Credentials()
