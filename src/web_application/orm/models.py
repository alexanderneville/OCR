#!/usr/bin/env python

from datetime import datetime
import abc, sqlite3, time
from . import authenticate
from . import config
def check_username_unused(conn: sqlite3.Connection, username: str):

    """return none if there is no user with that name, else return the record."""
    cursor = conn.cursor()
    if cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone() != None:
        raise Existing_Username

def create_user_object(conn: sqlite3.Connection, id: int):

    cursor = conn.cursor()
    data = cursor.execute('SELECT role FROM user WHERE id=?', [id]).fetchone()
    role = data[0]
    if role == "teacher":
        return Teacher(conn, id)
    elif role == "student":
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

    def list_classes(self):
        pass

    def create_model(self, model_name):
        try:
            cursor = self.conn.cursor()
            current = datetime.now()
            timestamp = time.mktime(current.timetuple())
            cursor.execute('INSERT INTO model (owner_id, name, timestamp) VALUES (?,?,?)', [self.id, model_name, timestamp])
            self.conn.commit()
            return cursor.execute('SELECT id FROM model WHERE owner_id=? ORDER BY id DESC limit 1', [self.id]).fetchone()[0]
        except sqlite3.IntegrityError:
            raise Existing_Model()

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
        hashed, salt = authenticate.new_hash(password)
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
        cursor.execute("DELETE FROM class_student WHERE student_id=? AND class_id=?", [student_id, class_id])
        self.conn.commit()
        pass

    def create_class(self, class_name, pin):

        cursor = self.conn.cursor()
        try:
            cursor.execute('INSERT into class (class_name, pin, teacher_id) VALUES (?, ?, ?)', [class_name, pin, self.id])
            self.conn.commit()
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
        else:
            if int(data[0]) == self.id:
                cursor.execute('DELETE FROM class WHERE id=?', [class_id])
                self.conn.commit()
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
        hashed, salt = authenticate.new_hash(password)
        cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);', [username, hashed, salt, full_name, 'student'])
        new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
        conn.commit()

        return new_user_id[0]

    def join_class(self, class_id: int, pin: int):
        cursor = self.conn.cursor()
        data = cursor.execute('SELECT pin FROM class WHERE id=?', [class_id]).fetchone()
        if data == None:
            raise Invalid_Credentials()

        try:
            if int(pin) == data[0]:
                cursor.execute('INSERT into class_student (class_id, student_id) VALUES (?,?)', [class_id, self._id])
                self.conn.commit()
            else:
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
        return new_class_id[0]

    def list_students(self):
        cursor = self.conn.cursor()
        data = cursor.execute("SELECT id, full_name FROM user WHERE id IN (SELECT student_id FROM class_student WHERE class_id=?)", [self.id]).fetchall()
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
        data = cursor.execute('SELECT name FROM model WHERE id=?', [self._id]).fetchone()
        self._model_name = data[0]

    @staticmethod
    def create():
        pass

    def delete(self):
        pass

    @property
    def model_name(self):
        return self._model_name

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
