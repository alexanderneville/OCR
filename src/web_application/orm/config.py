import sqlite3

secret_key = '1234'

data_path = "/home/alex/code/nea/data/"
training = data_path+"training_data/"
infile_path = training+"infile/"
outfile_path = training+"outfile/"
dataset_path = training+"dataset/"
info_path = training+"info/"
sample_path = training+"sample/"
model_path = data_path+"models/"
tmp_path = data_path+"tmp/"
db_path = data_path+'application_data.db'

def connect_db(path:str) -> sqlite3.Connection:
    conn = sqlite3.connect(path, detect_types=sqlite3.PARSE_DECLTYPES)
    return conn
