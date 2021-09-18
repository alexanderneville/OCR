from typing import Tuple
from datetime import datetime
import time, random, sqlite3, config, hashlib

def new_test_hash()-> Tuple[str, str]:

    salt = str(random.randint(1000, 9999))
    salted = "1234" + salt
    hashed = hashlib.sha256(salted.encode("utf-8")).hexdigest()

    return hashed, salt

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

    # users = [
    #         ["user1", " ", " ", "teacher one", "teacher"],
    #         ["user2", " ", " ", "teacher two", "teacher"],
    #         ["user3", " ", " ", "teacher three", "teacher"],
    #         ["user4", " ", " ", "student one", "student"],
    #         ["user5", " ", " ", "student two", "student"],
    #         ["user6", " ", " ", "student three", "student"],
    #         ["user7", " ", " ", "student four", "student"],
    #         ["user8", " ", " ", "student five", "student"],
    #         ["user9", " ", " ", "student six", "student"],
    #         ["user10", " ", " ", "student seven", "student"],
    #         ["user11", " ", " ", "student eight", "student"],
    #         ["user12", " ", " ", "student nine", "student"],
    #         ["user13", " ", " ", "student ten", "student"],
    #         ["user14", " ", " ", "student eleven", "student"],
    #         ["user15", " ", " ", "student twelve", "student"],
    #         ["user16", " ", " ", "student thirteen", "student"],
    #         ["user17", " ", " ", "student fourteen", "student"],
    #         ["user18", " ", " ", "student fifteen", "student"],
    #         ["user19", " ", " ", "student sixteen", "student"],
    #         ["user20", " ", " ", "student seventeen", "student"],
    #         ]

    users = []
    num_teachers = 10
    num_students = 300

    for teacher in range(num_teachers):
        hashed, salt = new_test_hash()
        users.append(["user"+str(teacher+1), hashed, salt, "Teacher "+str(teacher+1), "teacher"])

    for student in range(num_students):
        hashed, salt = new_test_hash()
        users.append(["user"+str(student+num_teachers+1), hashed, salt, "Student "+str(student+1), "student"])



    # classes = [
    #         (1, "class 1", 1234),
    #         (1, "class 2", 1234),
    #         (1, "class 3", 1234),
    #         (2, "class 4", 1234),
    #         (2, "class 5", 1234),
    #         (2, "class 6", 1234),
    #         (3, "class 7", 1234),
    #         (3, "class 8", 1234),
    #         (3, "class 9", 1234),
    #         ]

    classes = [((i+1), "class " + str(i+1), 1234) for i in range(10)]

    class_students = [(i + 11, (i//30) + 1) for i in range(300)]

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
