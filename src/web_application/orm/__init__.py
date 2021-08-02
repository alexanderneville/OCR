import sqlite3
import os

def connect_db(path:str) -> sqlite3.Connection:
    conn = sqlite3.connect(path, detect_types=sqlite3.PARSE_DECLTYPES)
    return conn

from .models import *
