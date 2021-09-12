#!/usr/bin/env python

import abc, sqlite3, random
from . import authenticate

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
        self._username = None
        self._full_name = None
        self._role = None

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

    @property
    def username(self):
        return self._username

    @property
    def full_name(self):
        return self._full_name

    @property
    def role(self):
        return self._role




class Teacher(entity_model):

    def __init__(self, conn: sqlite3.Connection, id: int):

        entity_model.__init__(self, conn, id)

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT username, full_name, role FROM user WHERE id =?', [self._id]).fetchone()

        if not data:

            raise No_Such_ID("no user associated with that username.")

        elif data[2] != "teacher":

            raise Exception("user is a student, not a teacher")

        else:

            self._username = data[0]
            self._full_name = data[1]
            self._role = data[2]


    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):

        cursor = conn.cursor()

        try:

            check_username_unused(conn, username)
            hashed, salt = authenticate.new_hash(password)
            cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);', [username, hashed, salt, full_name, 'teacher'])
            new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
            conn.commit()

            return int(new_user_id[0])

        except Existing_Username:

            return None

    def delete(self):
        pass


class Student(entity_model):

    def __init__(self, conn: sqlite3.Connection, id: int):

        entity_model.__init__(self, conn, id)

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT username, full_name, role FROM user WHERE id =?', [self._id]).fetchone()

        if not data:

            raise No_Such_ID("no user associated with that username.")

        elif data[2] != "student":

            raise Exception("user is a teacher, not a student")

        else:

            self._username = data[0]
            self._full_name = data[1]
            self._role = data[2]


    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):

        cursor = conn.cursor()

        try:

            check_username_unused(conn, username)
            hashed, salt = authenticate.new_hash(password)
            cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);', [username, hashed, salt, full_name, 'student'])
            new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
            conn.commit()

            return new_user_id[0]

        except Existing_Username:

            return None

    def delete(self):
        pass



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

class Invalid_Credentials(Exception):
    pass

class Insecure_Password(Exception):
    pass

class Insufficient_Data(Exception):
    pass
