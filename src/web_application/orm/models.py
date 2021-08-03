#!/usr/bin/env python

import abc, sqlite3, random
from . import authenticate

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


    @staticmethod
    def create(conn: sqlite3.Connection, username: str, full_name: str, password: str):

        cursor = conn.cursor()
        username_taken = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()

        if username_taken:

            return None

        else:

            hashed, salt = authenticate.hash(password)
            cursor.execute('INSERT into user (username, password, salt, full_name, role) VALUES (?, ?, ?, ?, ?);', [username, hashed, salt, full_name, 'teacher'])
            new_user_id = cursor.execute('SELECT id FROM user WHERE username =?', [username]).fetchone()
            conn.commit()

            return new_user_id[0]

    def delete(self):
        pass

    @property
    def username(self):
        return self._username

    @property
    def full_name(self):
        return self._full_name

class Student(entity_model):
    @staticmethod
    def create():
        pass

class School_Class(entity_model):
    pass

