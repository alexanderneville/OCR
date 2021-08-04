import orm, os, sqlite3

# replace with the path to the data directory

data_path = orm.data_path
db_path = orm.db_path
secret_key = '1234'

def create_tables():

    conn = orm.connect_db(db_path)

    # run create table functions
    orm.init_user(conn)
    orm.init_class(conn)
    orm.init_class_student(conn)
    orm.init_model(conn)
    orm.init_cache(conn)

    conn.close()

def test_insertion():
    
    conn = orm.connect_db(db_path)

    teacher_id = orm.Teacher.create(conn, "bethan123", "Bethan Neville", "1234")
    if teacher_id:
        teacher = orm.Teacher(conn, teacher_id)
        print(teacher.username)
        print(teacher.full_name)

    # teacher2 = orm.Teacher.create(conn, "richard123", "Richard Neville", "1234")

    conn.close()

def populate_tables():

    pass


if __name__ == "__main__":

    create_tables()
    # test_insertion()
    # populate_tables()
