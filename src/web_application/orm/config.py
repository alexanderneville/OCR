import sqlite3

secret_key = '1234'
data_path = "/home/alex/code/nea/data/"
input_path = data_path+"input/"
model_path = data_path+"models/"
tmp_path = data_path+"tmp/"
db_path = data_path+'application_data.db'

def connect_db(path:str) -> sqlite3.Connection:
    conn = sqlite3.connect(path, detect_types=sqlite3.PARSE_DECLTYPES)
    return conn
