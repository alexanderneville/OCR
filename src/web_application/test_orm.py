import orm
conn = orm.connect_db(orm.db_path)


def test_registration():

    print("attempting to register as a student with a new username ... ", end="")
    try:

        new_student = orm.Student.create(conn, "alex", "Alex Neville", "1234")
        print("new student id: ", new_student)

    except orm.Existing_Username:

        print("username taken")

    print("attempting to register as a teacher with a new username ... ", end="")
    try:

        new_teacher = orm.Teacher.create(conn, "John", "John Smith", "1234")
        print("new teacher id: ", new_teacher)

    except orm.Existing_Username:

        print("username taken")

def test_login():

    print("attempting to login with valid credentials ... ", end="")
    try:

        user = orm.login_user("alex", "1234")
        print("the user id is: ", user.id)

    except orm.Invalid_Credentials:

        print("invalid credential exception handled")

    print("attempting to login with invalid credentials ... ", end="")
    try:

        user = orm.login_user("", "")
        print("the user id is: ", user.id)

    except orm.Invalid_Credentials:

        print("invalid credental exception handled")

def test_instantiation():

    print("attempting to instantiate a new teacher from id ... ", end="")
    object = orm.create_user_object(conn, 10)
    print(object.full_name, "is a", object.role)
    print("attempting to instantiate a new student from id ... ", end="")
    object = orm.create_user_object(conn, 48)
    print(object.full_name, "is a", object.role)

def test_user_methods():

    teacher = orm.create_user_object(conn, 10)
    student = orm.create_user_object(conn, 48)

    print("classes belonging to the test teacher: ", teacher.list_classes())
    print("classes that the test student is part of: ", student.list_classes())

    # create a new class
    teacher.create_class("new_test_class", 1234)
    teacher_classes = teacher.list_classes()
    print("classes belonging to test teacher after creating new class: ", teacher_classes)

    # join the new classes
    student.join_class(teacher_classes[0][0], 1234)
    student.join_class(teacher_classes[1][0], 1234)
    print("classes that the test student is now part of:", student.list_classes())

    print("attempting to join a class with invalid credentials ...", end="")

    try:
        student.join_class(teacher_classes[0][0], 345)
    except orm.Invalid_Credentials:
        print("invalid credential exception handled")

    teacher.kick_student(48, teacher_classes[0][0])
    print("classes that the test student is part of after being kicked:", student.list_classes())

    print("teacher's personal models:", teacher.list_models())
    print("student's personal models:", student.list_models())

    print("all models available to the teacher:", teacher.list_all_models())

def main():

    # new dataset
    orm.new_database()

    # test register/login functions
    test_registration()
    print("\n")
    test_login()
    print("\n")

    # test instantiation function
    # id:int -> Teacher|Student
    test_instantiation()
    print("\n")

    # test assorted methods of the classes
    test_user_methods()
    print("\n")

if __name__ == "__main__":
    main()

