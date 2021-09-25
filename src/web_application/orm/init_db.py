from typing import Tuple
from datetime import datetime
import time, random, sqlite3, hashlib

try:
    from . import config
    db_path = config.db_path
except:
    db_path = "/home/alex/code/nea/data/application_data.db"


def new_test_hash()-> Tuple[str, str]:

    salt = str(random.randint(1000, 9999))
    salted = "1234" + salt
    hashed = hashlib.sha256(salted.encode("utf-8")).hexdigest()

    return hashed, salt

def init_user(conn: sqlite3.Connection):

    cursor = conn.cursor()
    try:

        cursor.execute("DROP TABLE IF EXISTS user")
        cursor.execute('''CREATE TABLE user (
                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                            username TEXT UNIQUE NOT NULL,
                            password TEXT NOT NULL,
                            salt INTEGER NOT NULL,
                            full_name TEXT NOT NULL,
                            role TEXT NOT NULL);''')
        conn.commit()
        cursor.close()
        conn.close()

    except sqlite3.Error as e:

        print("error creating user table.")
        cursor.close()
        conn.close()
        raise e


def init_class(conn: sqlite3.Connection):

    cursor = conn.cursor()
    try:

        cursor.execute("DROP TABLE IF EXISTS class")
        cursor.execute('''CREATE TABLE class (
                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                            teacher_id INTEGER NOT NULL,
                            class_name TEXT NOT NULL,
                            pin INTEGER NOT NULL,
                            UNIQUE(class_name, teacher_id),
                            FOREIGN KEY (teacher_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE);''')

        conn.commit()
        cursor.close()
        conn.close()

    except sqlite3.Error as e:

        print("error creating class table.")
        cursor.close()
        conn.close()
        raise e

def init_class_student(conn: sqlite3.Connection):

    cursor = conn.cursor()
    try:

        cursor.execute("DROP TABLE IF EXISTS class_student")
        cursor.execute('''CREATE TABLE class_student (
                            student_id INTEGER NOT NULL,
                            class_id INTEGER NOT NULL,
                            UNIQUE (student_id, class_id),
                            FOREIGN KEY (student_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE,
                            FOREIGN KEY (class_id) REFERENCES class (id) ON UPDATE CASCADE ON DELETE CASCADE);''')
        conn.commit()
        cursor.close()
        conn.close()

    except sqlite3.Error as e:

        print("error creating class_student table.")
        cursor.close()
        conn.close()
        raise e

def init_model(conn: sqlite3.Connection):

    cursor = conn.cursor()
    try:

        cursor.execute("DROP TABLE IF EXISTS model")
        cursor.execute('''CREATE TABLE model (
                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                            owner_id INTEGER NOT NULL,
                            name TEXT,
                            path TEXT,
                            infile_path TEXT,
                            timestamp INTEGER,
                            UNIQUE (owner_id, name),
                            FOREIGN KEY (owner_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE
                            );''')
        conn.commit()

    except sqlite3.Error as e:

        print("error creating model table.")
        cursor.close()
        conn.close()
        raise e

def init_cache(conn: sqlite3.Connection):

    cursor = conn.cursor()
    try:

        cursor.execute("DROP TABLE IF EXISTS cache")
        cursor.execute('''CREATE TABLE cache (
                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                            owner_id INTEGER NOT NULL,
                            contents TEXT, 
                            timestamp INTEGER,
                            FOREIGN KEY (owner_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE
                            );''')
        conn.commit()

    except sqlite3.Error as e:

        print("error creating cache table.")
        cursor.close()
        conn.close()
        raise e

def create_tables():

    # run create table functions
    init_user(sqlite3.connect(db_path, detect_types=sqlite3.PARSE_DECLTYPES))
    init_class(sqlite3.connect(db_path, detect_types=sqlite3.PARSE_DECLTYPES))
    init_class_student(sqlite3.connect(db_path, detect_types=sqlite3.PARSE_DECLTYPES))
    init_model(sqlite3.connect(db_path, detect_types=sqlite3.PARSE_DECLTYPES))
    init_cache(sqlite3.connect(db_path, detect_types=sqlite3.PARSE_DECLTYPES))

def populate_tables():

    conn = sqlite3.connect(db_path, detect_types=sqlite3.PARSE_DECLTYPES)

    cursor = conn.cursor()

    users = []
    num_teachers = 10
    num_students = 300

    # populate the user table with 10 teachers
    for teacher in range(num_teachers):
        hashed, salt = new_test_hash()
        users.append(["user"+str(teacher+1), 
                        hashed, salt, 
                        "Teacher "+str(teacher+1),
                        "teacher"])

    # populate the user table with 300 students
    for student in range(num_students):
        hashed, salt = new_test_hash()
        users.append(["user"+str(student+num_teachers+1),
                        hashed, salt, 
                        "Student "+str(student+1),
                        "student"])

    # populate the class tables and associated transaction table
    classes = [((i+1), "class " + str(i+1), 1234) for i in range(10)]
    class_students = [(i + 11, (i//30) + 1) for i in range(300)]

    # populate the models table with 3 models for each user
    # populate the cache table with 3 cache entries for each user
    current = datetime.now()
    timestamp = time.mktime(current.timetuple())
    models = [(i//3 + 1, "model"+str(i+1), timestamp - (random.randint(0, 5000)* 3600)) for i in range(930)]
    cache_data = [(i//3 + 1, "cache data belonging to user"+str(i//3+1), timestamp - (random.randint(0, 5000)* 3600)) for i in range(930)]


    # insert the test data into the database
    cursor.executemany('INSERT INTO user (username, password, salt, full_name, role) VALUES (?,?,?,?,?)', users)
    cursor.executemany('INSERT INTO class (teacher_id, class_name, pin) VALUES (?,?,?)', classes)
    cursor.executemany('INSERT INTO class_student (student_id, class_id) VALUES (?,?)', class_students)
    cursor.executemany('INSERT INTO model (owner_id, name, timestamp) VALUES (?,?,?)', models)
    cursor.executemany('INSERT INTO cache (owner_id, contents, timestamp) VALUES (?,?,?)', cache_data)

    conn.commit()
    cursor.close()
    conn.close()

def new_database():

    create_tables()
    populate_tables()

if __name__ == "__main__":
    new_database()
