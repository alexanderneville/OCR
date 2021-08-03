#!/usr/bin/env python

import os
import sqlite3

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
                            path TEXT NOT NULL,
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
                            age INTEGER NOT NULL,
                            FOREIGN KEY (owner_id) REFERENCES user (id) ON UPDATE CASCADE ON DELETE CASCADE
                            );''')
        conn.commit()

    except sqlite3.Error as e:

        print("error creating cache table.")
        conn.close()
        raise e

