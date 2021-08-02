#!/usr/bin/env python

import abc, sqlite3

class entity_model(abc.ABC):

    @abc.abstractmethod
    def __init__(self, connection: sqlite3.Connection, id):
        pass

    @staticmethod
    @abc.abstractmethod
    def create():
        pass

    @abc.abstractmethod
    def delete(self):
        pass
