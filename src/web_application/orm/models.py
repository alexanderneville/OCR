#!/usr/bin/env python

import abc, sqlite3, random
from . import authenticate

def check_exists(conn: sqlite3.Connection, username: str):

    cursor = conn.cursor()
    return cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()

def create_user_object(conn: sqlite3.Connection, id: int):

    cursor = conn.cursor()
    data = cursor.execute('SELECT role FROM user WHERE id=?', [id]).fetchone()
    role = data[0]
    if role == "teacher":
        return Teacher(conn, id)
    else:
        return Student(conn, id)

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


class Teacher(entity_model):

    def __init__(self, conn: sqlite3.Connection, id: int):

        entity_model.__init__(self, conn, id)

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT username, full_name, role FROM user WHERE id =?', [self._id]).fetchone()

        if not data:

            raise Exception("user not found")

        elif data[2] != "teacher":

            raise Exception("user is a student, not a teacher")

        else:

            self._username = data[0]
            self._full_name = data[1]
            self._role = data[2]


    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):

        cursor = conn.cursor()

        if check_exists(conn, username):

            # an entity with that username already exists
            return None

        else:

            # good to create new user
            hashed, salt = authenticate.new_hash(password)
            cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);', [username, hashed, salt, full_name, 'teacher'])
            new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
            conn.commit()

            return int(new_user_id[0])

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

class Student(entity_model):

    def __init__(self, conn: sqlite3.Connection, id: int):

        entity_model.__init__(self, conn, id)

        cursor = self.conn.cursor()
        data = cursor.execute('SELECT username, full_name, role FROM user WHERE id =?', [self._id]).fetchone()

        if not data:

            raise Exception("user not found")

        elif data[2] != "student":

            raise Exception("user is a teacher, not a student")

        else:

            self._username = data[0]
            self._full_name = data[1]
            self._role = data[2]


    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):

        cursor = conn.cursor()

        if check_exists(conn, username):

            # an entity with that username already exists
            return None

        else:

            # good to create new user
            hashed, salt = authenticate.new_hash(password)
            cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);', [username, hashed, salt, full_name, 'student'])
            new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
            conn.commit()

            return new_user_id[0]

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

class School_Class(entity_model):
    pass

