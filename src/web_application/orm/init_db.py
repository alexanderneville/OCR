import sqlite3
from datetime import datetime
import time

import config

def init_user(conn: sqlite3.Connection):

    try:

        cursor = conn.cursor()
        cursor.execute("DROP TABLE IF EXISTS user")
        cursor.execute('''CREATE TABLE user (
                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                            username TEXT UNIQUE NOT NULL,
                            password TEXT NOT NULL,
                            salt INTEGER NOT NULL,
                            full_name TEXT NOT NULL,
                            role TEXT NOT NULL);''')
        conn.commit()

    except sqlite3.Error as e:

        print("error creating user table.")
        conn.close()
        raise e


def init_class(conn: sqlite3.Connection):

    try:

        cursor = conn.cursor()
        cursor.execute("DROP TABLE IF EXISTS class")
        cursor.execute('''CREATE TABLE class (
                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                            teacher_id INTEGER NOT NULL,
                            class_name TEXT UNIQUE NOT NULL,
                            pin INTEGER NOT NULL,
                            FOREIGN KEY (teacher_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE);''')

        conn.commit()

    except sqlite3.Error as e:

        print("error creating class table.")
        conn.close()
        raise e

def init_class_student(conn: sqlite3.Connection):

    try:

        cursor = conn.cursor()
        cursor.execute("DROP TABLE IF EXISTS class_student")
        cursor.execute('''CREATE TABLE class_student (
                            student_id INTEGER NOT NULL,
                            class_id INTEGER NOT NULL,
                            FOREIGN KEY (student_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE,
                            FOREIGN KEY (class_id) REFERENCES class (id) ON UPDATE CASCADE ON DELETE CASCADE);''')
        conn.commit()

    except sqlite3.Error as e:

        print("error creating class_student table.")
        conn.close()
        raise e

def init_model(conn: sqlite3.Connection):

    try:

        cursor = conn.cursor()
        cursor.execute("DROP TABLE IF EXISTS model")
        cursor.execute('''CREATE TABLE model (
                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                            owner_id INTEGER NOT NULL,
                            name TEXT,
                            path TEXT,
                            infile_path TEXT,
                            timestamp INTEGER,
                            FOREIGN KEY (owner_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE
                            );''')
        conn.commit()

    except sqlite3.Error as e:

        print("error creating model table.")
        conn.close()
        raise e

def init_cache(conn: sqlite3.Connection):

    try:

        cursor = conn.cursor()
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
        conn.close()
        raise e

def create_tables():

    conn = sqlite3.connect(config.db_path, detect_types=sqlite3.PARSE_DECLTYPES)

    # run create table functions
    init_user(conn)
    init_class(conn)
    init_class_student(conn)
    init_model(conn)
    init_cache(conn)

    conn.close()

def populate_tables():

    conn = sqlite3.connect(config.db_path, detect_types=sqlite3.PARSE_DECLTYPES)

    cursor = conn.cursor()

    users = [
            ("alex123", "1234", "1234", "Alexander Neville", "teacher"),
            ("Sarah93", "1234", "1234", "Sarah West", "teacher"),
            ("John56", "1234", "1234", "John Smith", "teacher"),
            ("Kate54", "1234", "1234", "Kate Blaine", "student"),
            ("benjamin6", "1234", "1234", "Benjamin Johnson", "student"),
            ("thomas37", "1234", "1234", "Thomas George", "student"),
            ("richard78", "1234", "1234", "Richard Abbey", "student"),
            ("daniel43", "1234", "1234", "Daniel Jefferson", "student")
            ]

    classes = [
            (1, "Mr. Neville's class", 1234),
            (2, "Miss. West's class", 1234),
            (3, "Mr. Smith's class", 1234),
            ]
    class_students = [
            (4, 2),
            (4, 3),
            (5, 1),
            (7, 2),
            (7, 3)
            ]
    current = datetime.now()
    timestamp = time.mktime(current.timetuple())

    models = [
            ('1', "my handwriting", "model1.json", "infile", timestamp),
            ('1', "friend's handwriting", "model2.json", "infile", timestamp - 3600),
            ('3', "My favourite font", "model3.json", "infile", timestamp - (3600 * 24)),
            ('5', "Exercise book", "model4.json", "infile", timestamp - (3600 * 48)),
            ('6', "Scanned Document", "model5.json", "infile", timestamp - (3600)),
            ]

    cache_data = []

    cursor.executemany('INSERT INTO user (username, password, salt, full_name, role) VALUES (?,?,?,?,?)', users)
    cursor.executemany('INSERT INTO class (teacher_id, class_name, pin) VALUES (?,?,?)', classes)
    cursor.executemany('INSERT INTO class_student (student_id, class_id) VALUES (?,?)', class_students)
    cursor.executemany('INSERT INTO model (owner_id, name, path, infile_path, timestamp) VALUES (?,?,?,?,?)', models)
    cursor.executemany('INSERT INTO cache (owner_id, contents, timestamp) VALUES (?,?,?)', cache_data)

    conn.commit()

def main():
    create_tables()
    populate_tables()

if __name__ == "__main__":
    main()
