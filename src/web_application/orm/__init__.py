import sqlite3
import os

from .models import *
from .init_db import *
from .authenticate import *

data_path = "/home/alex/code/nea/data/"
db_path = data_path+'application_data.db'

def connect_db(path:str) -> sqlite3.Connection:
    conn = sqlite3.connect(path, detect_types=sqlite3.PARSE_DECLTYPES)
    return conn

