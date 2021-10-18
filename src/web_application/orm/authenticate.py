#!/usr/bin/env python

import hashlib
import random
import re
from typing import Tuple
from . import models as db_class
from . import config


def validate_password(password):
    """ Run a regex to ensure the password is complex. """

    if not re.fullmatch(r'[A-Za-z0-9!@#$%^&\*()_\-+=\\|/.,`~\'\":;<>?]{8,}', password):
        raise db_class.Insecure_Password()


def new_hash(plaintext) -> Tuple[str, str]:
    """ Given some plain text hash it and return the salt. """

    salt = str(random.randint(1000, 9999))
    salted = plaintext + salt
    hashed = hashlib.sha256(salted.encode("utf-8")).hexdigest()

    return hashed, salt


def hash(plaintext: str, salt: str) -> str:
    """ Use an existing salt to hash a plaintext password. """
    salted = plaintext + str(salt)
    hashed = hashlib.sha256(salted.encode("utf-8")).hexdigest()

    return hashed


def login_user(username: str, password: str):
    """ Perform user authentication with a username and password. """

    conn = config.connect_db(config.db_path)
    cursor = conn.cursor()
    data = cursor.execute('SELECT id, password, salt FROM user WHERE username =?', [username]).fetchone()
    del (cursor)
    del (conn)

    if data:
        # if data was returned, check if the password matches the hash
        if hash(password, data[2]) == data[1]:
            return db_class.create_user_object(config.connect_db(config.db_path), data[0])

    # raise exception is the login is not successful
    raise db_class.Invalid_Credentials()
