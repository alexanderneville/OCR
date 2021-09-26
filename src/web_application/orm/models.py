#!/usr/bin/env python

from datetime import datetime
import abc, sqlite3, time
from . import authenticate as auth
from . import config

def check_username_unused(conn: sqlite3.Connection, username: str):

    """return none if there is no user with that name, else return the record."""
    cursor = conn.cursor()
    if cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone() != None:
        raise Existing_Username

def create_user_object(conn: sqlite3.Connection, id: int):

    cursor = conn.cursor()
    data = cursor.execute('SELECT role FROM user WHERE id=?', [id]).fetchone()
    if data:
        if data[0] == "teacher":
            return Teacher(conn, id)
        else:
            return Student(conn, id)
    else:
        raise No_Such_ID("no user with that ID is found")

class entity_model(abc.ABC):

    @abc.abstractmethod
    def __init__(self, conn: sqlite3.Connection, id: int):
        self.conn = conn
        self._id = id

    @staticmethod
    @abc.abstractmethod
    def create():
        pass

    @abc.abstractmethod
    def delete(self):
        pass

    @property
    def id(self):
        return self._id

class User(entity_model):

    def __init__(self, conn: sqlite3.Connection, id: int):

        self._username = None
        self._full_name = None
        self._role = None

    def list_models(self):

        cursor = self.conn.cursor()
        return cursor.execute('SELECT * FROM model WHERE owner_id=?', [self.id]).fetchall()

    def delete_model(self, model_id):

        cursor = self.conn.cursor()
        cursor.execute('DELETE FROM model WHERE id=? AND owner_id=?', [model_id, self.id])
        self.conn.commit()
        cursor.close()

    def list_classes(self):
        pass

    def create_model(self, model_name):

        cursor = self.conn.cursor()

        try:

            current = datetime.now()
            timestamp = time.mktime(current.timetuple())
            cursor.execute('INSERT INTO model (owner_id, name, trained, labelled, timestamp) VALUES (?,?,?,?,?)', [self.id, model_name, 0, 0, timestamp])
            self.conn.commit()
            id = cursor.execute('SELECT id FROM model WHERE owner_id=? ORDER BY id DESC limit 1', [self.id]).fetchone()[0]
            cursor.close()
            return id

        except sqlite3.IntegrityError:

            cursor.close()
            raise Existing_Model()

    def list_cache(self):

        cursor = self.conn.cursor()
        entries = cursor.execute("SELECT * FROM cache WHERE owner_id=? ORDER BY timestamp DESC;", [self._id]).fetchall()

        for entry in range(len(entries)):
            entries[entry] = list(entries[entry])
            entries[entry][3] = datetime.fromtimestamp(entries[entry][3])

        cursor.close()
        return entries

    def delete_cache(self, id):

        cursor = self.conn.cursor()
        cursor.execute("DELETE FROM cache WHERE id=? and owner_id=?", [id, self._id])
        self.conn.commit()
        cursor.close()

    @property
    def username(self):
        return self._username

    @property
    def full_name(self):
        return self._full_name

    @property
    def role(self):
        return self._role

class Teacher(User):

    def __init__(self, conn: sqlite3.Connection, id: int):

        entity_model.__init__(self, conn, id)

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT username, full_name, role FROM user WHERE id =?', [self._id]).fetchone()
        cursor.close()

        if not data:

            raise No_Such_ID("no user associated with that username.")

        elif data[2] != "teacher":

            raise Invalid_Role("user is a student, not a teacher")

        else:

            self._username = data[0]
            self._full_name = data[1]
            self._role = data[2]


    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):

        cursor = conn.cursor()

        check_username_unused(conn, username) # will raise existing username exception if username exists
        auth.validate_password(password) # will raise insecure password exception if username exists
        hashed, salt = auth.new_hash(password)
        cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);', [username, hashed, salt, full_name, 'teacher'])
        new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
        conn.commit()

        return int(new_user_id[0])


    def list_classes(self):
        cursor = self.conn.cursor()
        # get the id, name, pin and teacher name
        data = cursor.execute("""SELECT class.id, class.class_name, class.pin, user.username
                                 FROM class
                                 INNER JOIN user ON class.teacher_id = user.id
                                 WHERE class.teacher_id=?""", [self._id]).fetchall()
        return data


    def kick_student(self, student_id: int, class_id: int):
        cursor = self.conn.cursor()
        data = cursor.execute("SELECT id FROM class WHERE teacher_id=?", [self.id]).fetchall()
        ids = [element[0] for element in data]
        if class_id not in ids:
            cursor.close()
            raise Invalid_Credentials()
        else:
            cursor.execute("DELETE FROM class_student WHERE student_id=? AND class_id=?", [student_id, class_id])
            cursor.close()
            self.conn.commit()

    def create_class(self, class_name, pin):

        cursor = self.conn.cursor()
        try:
            cursor.execute('INSERT into class (class_name, pin, teacher_id) VALUES (?, ?, ?)', [class_name, pin, self.id])
            self.conn.commit()
            cursor.close()
        except sqlite3.IntegrityError:
            raise Existing_Class()

    def list_all_models(self):
        cursor = self.conn.cursor()
        return cursor.execute("""SELECT model.name AS model_name, model.id, user.username
                                 FROM model INNER JOIN user ON model.owner_id=user.id
                                 WHERE owner_id in
                                 (SELECT student_id FROM class_student WHERE class_id =
                                 (SELECT id FROM class WHERE teacher_id=?));""", [self.id]).fetchall()

    def delete_class(self, class_id):

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT teacher_id FROM class WHERE id=?', [class_id]).fetchone()
        if not data:
            raise Invalid_Credentials()
        elif int(data[0]) == self.id:
            cursor.execute('DELETE FROM class WHERE id=?', [class_id])
            self.conn.commit()
            cursor.close()
        else:
            raise Invalid_Credentials()

    def delete(self):
        pass

class Student(User):

    def __init__(self, conn: sqlite3.Connection, id: int):

        entity_model.__init__(self, conn, id)

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT username, full_name, role FROM user WHERE id =?', [self._id]).fetchone()

        if not data:

            raise No_Such_ID("no user associated with that username.")

        elif data[2] != "student":

            raise Invalid_Role("user is a teacher, not a student")

        else:

            self._username = data[0]
            self._full_name = data[1]
            self._role = data[2]


    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):

        cursor = conn.cursor()

        check_username_unused(conn, username)
        auth.validate_password(password)
        hashed, salt = auth.new_hash(password)
        cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);', [username, hashed, salt, full_name, 'student'])
        new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
        conn.commit()

        return new_user_id[0]

    def join_class(self, class_id: int, pin: int):

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT pin FROM class WHERE id=?', [class_id]).fetchone()

        if data == None:
            cursor.close()
            raise Invalid_Credentials()

        try:
            if int(pin) == data[0]:
                cursor.execute('INSERT into class_student (class_id, student_id) VALUES (?,?)', [class_id, self._id])
                self.conn.commit()
                cursor.close()
            else:
                cursor.close()
                raise Invalid_Credentials()
        except sqlite3.IntegrityError:
            raise Existing_Member()

    def leave_class(self, class_id: int):
        cursor = self.conn.cursor()
        cursor.execute('DELETE FROM class_student WHERE student_id=? AND class_id=?', [self._id, class_id])
        self.conn.commit()

    def list_classes(self):

        cursor = self.conn.cursor()
        data = cursor.execute("""SELECT class.id, class.class_name, class.pin, user.username
                            FROM class_student
                            INNER JOIN class ON class_student.class_id = class.id
                            INNER JOIN user ON class.teacher_id = user.id
                            WHERE class_student.student_id=?""", [self._id]).fetchall();

        return data

    def delete(self):
        pass

class ClassGroup(entity_model):

    def __init__(self, conn: sqlite3.Connection, id: int):
        super().__init__(conn, id)
        cursor = self.conn.cursor()
        data = cursor.execute('SELECT class_name, pin FROM class WHERE id =?', [self._id]).fetchone()
        self._class_name = data[0]
        self._pin = data[1]

    @staticmethod
    def create(conn: sqlite3.Connection, class_name: str, pin: int, teacher_id: int):

        cursor = conn.cursor()
        if cursor.execute('SELECT * FROM class WHERE teacher_id=? AND class_name=?', [teacher_id, class_name]).fetchone() != None:
            raise Existing_Class()
        cursor.execute('INSERT into class (class_name, pin, teacher_id) VALUES (?, ?, ?)', [class_name, pin, teacher_id])
        new_class_id = cursor.execute('SELECT id FROM class WHERE teaacher_id=? AND class_name=?', [teacher_id, class_name]).fetchone()
        conn.commit()
        cursor.close()
        return new_class_id[0]

    def list_students(self):
        cursor = self.conn.cursor()
        data = cursor.execute("SELECT id, full_name FROM user WHERE id IN (SELECT student_id FROM class_student WHERE class_id=?)", [self.id]).fetchall()
        cursor.close()
        return data

    def delete(self):
        pass

    @property
    def class_name(self):
        return self._class_name

    @property
    def pin(self):
        return self._pin

class Model(entity_model):

    def __init__(self, conn: sqlite3.Connection, id: int):
        super().__init__(conn, id)
        cursor = self.conn.cursor()
        data = cursor.execute('SELECT name, trained, labelled FROM model WHERE id=?', [self._id]).fetchone()
        cursor.close
        self._model_name = data[0]
        self._trained = data[1]
        self._labelled = data[2]

    @staticmethod
    def create():
        pass

    def set_labelled(self):
        pass

    def set_trained(self):
        pass

    def delete(self):
        pass

    @property
    def model_name(self):
        return self._model_name

    @property
    def is_trained(self):

        if self._trained == 1:
            return True
        else:
            return False

    @property
    def is_labelled(self):

        if self._labelled == 1:
            return True
        else:
            return False

    @property
    def data_paths(self):

        cursor = self.conn.cursor()
        paths = cursor.execute('SELECT infile_path, outfile_path, dataset_path, sample_path, info_path, model_path FROM model WHERE id=?', [self._id]).fetchone()
        cursor.close()
        return paths

##############
# EXCEPTIONS #
##############

class School_Class(entity_model):
    pass

class No_Such_ID(Exception):
    pass

class No_Such_Username(Exception):
    pass

class Existing_Username(Exception):
    pass

class Existing_Class(Exception):
    pass

class Existing_Model(Exception):
    pass

class Invalid_Credentials(Exception):
    pass

class Invalid_Role(Exception):
    pass

class Insecure_Password(Exception):
    pass

class Insufficient_Data(Exception):
    pass

class Existing_Member(Exception):
    pass

class Invalid_FileType(Exception):
    pass
