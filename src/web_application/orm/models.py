#!/usr/bin/env python

from datetime import datetime
import abc, sqlite3, time
from . import authenticate as auth
from . import config


def check_username_unused(conn: sqlite3.Connection, username: str):
    """ Raise exception if the username is taken. """
    cursor = conn.cursor()
    if cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone() is not None:
        raise Existing_Username


def create_user_object(conn: sqlite3.Connection, id: int):
    """ From a unique id initialise a user or raise an exception. """

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
    """ Abstract blueprint for all database classes. """

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

    """ An abstract representation of both a teacher and a student. """

    def __init__(self, conn: sqlite3.Connection, id: int):

        # set common fields
        self._username = None
        self._full_name = None
        self._role = None

    def list_models(self):
        """ Return a list of the models owned by the current user. """
        cursor = self.conn.cursor()
        return cursor.execute('SELECT * FROM model WHERE owner_id=?', [self.id]).fetchall()

    def delete_model(self, model_id):
        """ Delete a model owned by the current user. """

        cursor = self.conn.cursor()
        cursor.execute('DELETE FROM model WHERE id=? AND owner_id=?', [model_id, self.id])
        self.conn.commit()
        cursor.close()

    def list_classes(self):
        pass

    def create_model(self, model_name):
        """ Attempt to create a model. """

        cursor = self.conn.cursor()

        try:

            current = datetime.now()
            timestamp = time.mktime(current.timetuple())
            cursor.execute('INSERT INTO model (owner_id, name, trained, labelled, timestamp) VALUES (?,?,?,?,?)',
                           [self.id, model_name, 0, 0, timestamp])
            self.conn.commit()
            # obtain the id for the new record
            id = cursor.execute('SELECT id FROM model WHERE owner_id=? ORDER BY id DESC limit 1',
                                [self.id]).fetchone()[0]
            cursor.close()
            return id

        except sqlite3.IntegrityError:

            # a model with this name and owner already exists
            cursor.close()
            raise Existing_Model()

    def list_cache(self):
        """ Return a list of the cache entries belonging to the current user. """

        cursor = self.conn.cursor()
        entries = cursor.execute("SELECT * FROM cache WHERE owner_id=? ORDER BY timestamp DESC;", [self._id]).fetchall()

        # format the return value
        for entry in range(len(entries)):
            entries[entry] = list(entries[entry])
            entries[entry][3] = datetime.fromtimestamp(entries[entry][3])

        cursor.close()
        return entries

    def delete_cache(self, id):
        """ Delete a cache entry belonging to the current user. """

        cursor = self.conn.cursor()
        cursor.execute("DELETE FROM cache WHERE id=? and owner_id=?", [id, self._id])
        self.conn.commit()
        cursor.close()

    def create_cache(self, text):
        """ Create a new cache entry. """

        cursor = self.conn.cursor()
        # get current timestamp
        current = datetime.now()
        timestamp = time.mktime(current.timetuple())
        cursor.execute("INSERT INTO cache (owner_id, contents, timestamp) VALUES (?,?,?)", [self._id, text, timestamp])
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
    """ Double inherited class to model a teacher. """

    def __init__(self, conn: sqlite3.Connection, id: int):

        # call to superclass constructor
        entity_model.__init__(self, conn, id)
        cursor = self.conn.cursor()
        data = cursor.execute('SELECT username, full_name, role FROM user WHERE id =?', [self._id]).fetchone()
        cursor.close()

        # check that instantiation is valid
        if not data:
            raise No_Such_ID("no user associated with that username.")
        elif data[2] != "teacher":
            raise Invalid_Role("user is a student, not a teacher")
        else:
            # populate fields
            self._username = data[0]
            self._full_name = data[1]
            self._role = data[2]

    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):
        """ Attempt to create a new teacher. """

        cursor = conn.cursor()
        check_username_unused(conn, username)  # will raise existing username exception if username exists
        auth.validate_password(password)       # will raise insecure password exception if username exists
        hashed, salt = auth.new_hash(password) # hash the new password

        # perform database insertion and obtain the id of the new user
        cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);',
                       [username, hashed, salt, full_name, 'teacher'])
        new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
        conn.commit()

        return int(new_user_id[0])

    def list_classes(self):
        """ List the classes taught by the current teacher. """

        cursor = self.conn.cursor()
        # get the id, name, pin and teacher name
        data = cursor.execute("""SELECT class.id, class.class_name, class.pin, user.username
                                 FROM class
                                 INNER JOIN user ON class.teacher_id = user.id
                                 WHERE class.teacher_id=?""", [self._id]).fetchall()
        return data

    def kick_student(self, student_id: int, class_id: int):
        """ Remove a student from a class taught by the current teacher. """

        cursor = self.conn.cursor()
        data = cursor.execute("SELECT id FROM class WHERE teacher_id=?", [self.id]).fetchall()
        ids = [element[0] for element in data]
        # ensure that current user has ownership of class
        if class_id not in ids:
            cursor.close()
            # raise error if invalid permissions
            raise Invalid_Credentials()
        else:
            # perform the deletion
            cursor.execute("DELETE FROM class_student WHERE student_id=? AND class_id=?", [student_id, class_id])
            cursor.close()
            self.conn.commit()

    def create_class(self, class_name, pin):
        """ Attempt to create a new class. """

        cursor = self.conn.cursor()
        try:
            cursor.execute('INSERT into class (class_name, pin, teacher_id) VALUES (?, ?, ?)',
                           [class_name, pin, self.id])
            self.conn.commit()
            cursor.close()
        except sqlite3.IntegrityError:
            # if the teacher already teaches a class of the same name
            raise Existing_Class()

    def list_all_models(self):
        """ Return a list of all the models available to a teacher. """
        cursor = self.conn.cursor()
        return cursor.execute("""SELECT model.name AS model_name, model.id, user.username
                                 FROM model INNER JOIN user ON model.owner_id=user.id
                                 WHERE owner_id in
                                 (SELECT student_id FROM class_student WHERE class_id =
                                 (SELECT id FROM class WHERE teacher_id=?));""", [self.id]).fetchall()

    def delete_class(self, class_id):
        """ Attempt to delete a class. """
        cursor = self.conn.cursor()
        data = cursor.execute('SELECT teacher_id FROM class WHERE id=?', [class_id]).fetchone()
        # ensure the current user owns the class
        if not data:
            raise Invalid_Credentials()
        elif int(data[0]) == self.id:
            # if all is okay, perform the deletion
            cursor.execute('DELETE FROM class WHERE id=?', [class_id])
            self.conn.commit()
            cursor.close()
        else:
            raise Invalid_Credentials()

    def delete(self):
        pass


class Student(User):
    """ Double inherited class to model a student. """

    def __init__(self, conn: sqlite3.Connection, id: int):

        # call to superclass constructor
        entity_model.__init__(self, conn, id)

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT username, full_name, role FROM user WHERE id =?', [self._id]).fetchone()

        if not data:

            raise No_Such_ID("no user associated with that username.")

        elif data[2] != "student":

            raise Invalid_Role("user is a teacher, not a student")

        else:

            # populate fields
            self._username = data[0]
            self._full_name = data[1]
            self._role = data[2]

    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):
        """ Attempt to create a new student. """

        cursor = conn.cursor()
        # ensure that the username does not exist
        check_username_unused(conn, username)
        auth.validate_password(password)
        hashed, salt = auth.new_hash(password)
        cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);',
                       [username, hashed, salt, full_name, 'student'])
        # obtain the id of the new user
        new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
        conn.commit()

        return new_user_id[0]

    def join_class(self, class_id: int, pin: int):
        """ Attempt to join a class with the given credentials. """

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT pin FROM class WHERE id=?', [class_id]).fetchone()

        if data == None:
            # the class does not exist
            cursor.close()
            raise Invalid_Credentials()

        try:
            if int(pin) == data[0]:
                # if the pin is correct perform the insertion
                cursor.execute('INSERT into class_student (class_id, student_id) VALUES (?,?)', [class_id, self._id])
                self.conn.commit()
                cursor.close()
            else:
                cursor.close()
                raise Invalid_Credentials()
        except sqlite3.IntegrityError:
            # the current user is already a member of the class
            raise Existing_Member()

    def leave_class(self, class_id: int):
        """ Leave the specified class. """

        cursor = self.conn.cursor()
        cursor.execute('DELETE FROM class_student WHERE student_id=? AND class_id=?', [self._id, class_id])
        self.conn.commit()

    def list_classes(self):
        """ List all of the classes the current student is a member of. """

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
    """ Designed to model a class. """

    def __init__(self, conn: sqlite3.Connection, id: int):

        # call to superclass constructor
        super().__init__(conn, id)
        cursor = self.conn.cursor()
        data = cursor.execute('SELECT class_name, pin FROM class WHERE id =?', [self._id]).fetchone()
        # populate fields
        self._class_name = data[0]
        self._pin = data[1]

    @staticmethod
    def create(conn: sqlite3.Connection, class_name: str, pin: int, teacher_id: int):
        """ Attempt to create a new class. """

        cursor = conn.cursor()
        if cursor.execute('SELECT * FROM class WHERE teacher_id=? AND class_name=?',
                          [teacher_id, class_name]).fetchone() != None:
            raise Existing_Class()
        cursor.execute('INSERT into class (class_name, pin, teacher_id) VALUES (?, ?, ?)',
                       [class_name, pin, teacher_id])
        # obtain the id of the new class
        new_class_id = cursor.execute('SELECT id FROM class WHERE teaacher_id=? AND class_name=?',
                                      [teacher_id, class_name]).fetchone()
        conn.commit()
        cursor.close()
        return new_class_id[0]

    def list_students(self):
        """ Return a list of the students who are members of the current class. """
        cursor = self.conn.cursor()
        data = cursor.execute(
            "SELECT id, full_name FROM user WHERE id IN (SELECT student_id FROM class_student WHERE class_id=?)",
            [self.id]).fetchall()
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
    """ Designed to represent a machine learning model. """

    def __init__(self, conn: sqlite3.Connection, id: int):
        # call to superclass constructor
        super().__init__(conn, id)
        cursor = self.conn.cursor()
        data = cursor.execute('SELECT name, trained, labelled FROM model WHERE id=?', [self._id]).fetchone()
        cursor.close()
        # populate fields
        self._model_name = data[0]
        self._trained = data[1]
        self._labelled = data[2]

    @staticmethod
    def create():
        pass

    def set_labelled(self):
        """ Update the db to mark the model as labelled. """

        cursor = self.conn.cursor()
        cursor.execute("UPDATE model SET labelled=1 WHERE id=?", [self._id])
        self._labelled = 1
        self.conn.commit()
        cursor.close()

    def set_trained(self):
        """ Update the db to mark the model as trained. """

        cursor = self.conn.cursor()
        cursor.execute("UPDATE model SET trained=1 WHERE id=?", [self._id])
        self._trained = 1
        self.conn.commit()
        cursor.close()

    def delete(self):
        pass

    @property
    def model_name(self):
        return self._model_name

    @property
    def model_path(self):
        if self._trained == 0:
            return None
        else:
            cursor = self.conn.cursor()
            data = cursor.execute("SELECT model_path FROM model WHERE id=?", [self._id]).fetchone()
            cursor.close()
            return data[0]

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
        paths = cursor.execute(
            'SELECT infile_path, outfile_path, dataset_path, sample_path, info_path, model_path FROM model WHERE id=?',
            [self._id]).fetchone()
        cursor.close()
        return paths


##############
# EXCEPTIONS #
##############

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
