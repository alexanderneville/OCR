#!/usr/bin/env python

import hashlib
import random
from typing import Tuple

def hash(plaintext)-> Tuple[str, str]:

    salt = str(random.randint(1000, 9999))
    salted = plaintext + salt
    hashed = hashlib.sha256(salted.encode("utf-8")).hexdigest()

    return hashed, salt

