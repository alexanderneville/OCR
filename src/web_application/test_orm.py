import orm
import unittest
import sqlite3


class ORMTest(unittest.TestCase):

    def setUp(self):
        self.conn = orm.connect_db(orm.db_path)

    def tearDown(self):
        pass

    def test_registration_with_valid_credentials(self):
        new_student = orm.Student.create(self.conn, "user311", "Student 301", "Aa9-_*'bbbbbbbbbbb")
        self.assertIsNotNone(new_student)

        new_teacher = orm.Teacher.create(self.conn, "user312", "Teacher 11", "Aa9-_*'bbbbbbbbbbb")
        self.assertIsNotNone(new_teacher)

    def test_registration_with_insecure_password(self):
        self.assertRaises(orm.Insecure_Password,
                          orm.Student.create, self.conn,
                          "user313", "Student 302", "1234")
        self.assertRaises(orm.Insecure_Password, orm.Teacher.create,
                          self.conn, "user314",
                          "Teacher 12", "1234")

    def test_registration_with_existing_username(self):
        self.assertRaises(orm.Existing_Username,
                          orm.Student.create, self.conn,
                          "user11", "Student 303",
                          "Aa9-_*'bbbbbbbbbbb")
        self.assertRaises(orm.Existing_Username,
                          orm.Teacher.create, self.conn,
                          "user1", "Teacher 13",
                          "Aa9-_*'bbbbbbbbbbb")

    def test_login_with_valid_credentials(self):
        self.assertTrue(isinstance(orm.login_user("user11", "1234"), orm.Student))
        self.assertTrue(isinstance(orm.login_user("user1", "1234"), orm.Teacher))

    def test_login_with_invalid_credentials(self):
        self.assertRaises(orm.Invalid_Credentials, orm.login_user, "user11", "123")
        self.assertRaises(orm.Invalid_Credentials, orm.login_user, "user1", "123")

    def test_instantiation_with_valid_id(self):
        self.assertTrue(isinstance(orm.create_user_object(self.conn, 11), orm.Student))
        self.assertTrue(isinstance(orm.create_user_object(self.conn, 1), orm.Teacher))

    def test_instantiation_with_invalid_id(self):
        self.assertRaises(orm.No_Such_ID, orm.create_user_object, self.conn, 4000)

    def test_list_models(self):
        student = orm.login_user("user11", "1234")
        teacher = orm.login_user("user1", "1234")
        self.assertEqual(len(student.list_models()), 4)
        self.assertEqual(len(teacher.list_models()), 4)

    def test_create_model(self):
        student = orm.login_user("user11", "1234")
        teacher = orm.login_user("user1", "1234")
        id1 = student.create_model("model4")
        id2 = teacher.create_model("model4")
        self.assertRaises(orm.Existing_Model, student.create_model, "model4")

    def test_delete_model(self):
        student = orm.login_user("user11", "1234")
        teacher = orm.login_user("user1", "1234")
        student.delete_model((int(student.id) - 1) * 3 + 4)
        teacher.delete_model((int(student.id) - 1) * 3 + 4)

    def test_list_classes(self):
        student = orm.login_user("user11", "1234")
        teacher = orm.login_user("user1", "1234")
        self.assertEqual(len(student.list_classes()), 1)
        self.assertEqual(len(student.list_classes()[0]), 4)
        self.assertEqual(len(teacher.list_classes()), 1)
        self.assertEqual(len(teacher.list_classes()[0]), 4)

    def test_user_properties(self):
        student = orm.login_user("user11", "1234")
        teacher = orm.login_user("user1", "1234")
        self.assertEqual(student.id, 11)
        self.assertEqual(student.username, "user11")
        self.assertEqual(student.full_name, "Student 1")
        self.assertEqual(student.role, "student")
        self.assertEqual(teacher.id, 1)
        self.assertEqual(teacher.username, "user1")
        self.assertEqual(teacher.full_name, "Teacher 1")
        self.assertEqual(teacher.role, "teacher")

    def test_list_all_models(self):
        teacher = orm.login_user("user2", "1234")
        self.assertIsNotNone(teacher.list_all_models())

    def test_kick_student(self):
        teacher = orm.login_user("user2", "1234")
        self.assertIsNone(teacher.kick_student(41, 2))
        affected_student = orm.login_user("user41", "1234")
        control_student = orm.login_user("user42", "1234")
        self.assertEqual(len(control_student.list_classes()), 1)
        self.assertEqual(len(affected_student.list_classes()), 0)

    def test_kick_student_with_invalid_credentials(self):
        teacher = orm.login_user("user2", "1234")
        self.assertRaises(orm.Invalid_Credentials, teacher.kick_student, 12, 1)

    def test_create_class(self):
        teacher = orm.login_user("user2", "1234")
        self.assertIsNone(teacher.create_class("test_class", 1234))
        self.assertRaises(orm.Existing_Class, teacher.create_class, "test_class", 1234)
        self.assertEqual(len(teacher.list_classes()), 2)

    def test_delete_class(self):
        teacher = orm.login_user("user2", "1234")
        self.assertIsNone(teacher.delete_class(11))
        self.assertRaises(orm.Invalid_Credentials, teacher.delete_class, 11)
        self.assertRaises(orm.Invalid_Credentials, teacher.delete_class, 10)

    def test_join_class(self):
        student = orm.login_user("user71", "1234")
        self.assertIsNone(student.join_class(4, 1234))
        self.assertEqual(len(student.list_classes()), 2)
        self.assertRaises(orm.Existing_Member, student.join_class, 3, 1234)
        self.assertRaises(orm.Invalid_Credentials, student.join_class, 1, 123)
        self.assertRaises(orm.Invalid_Credentials, student.join_class, 4000, 1234)

    def test_leave_class(self):
        student = orm.login_user("user71", "1234")
        self.assertIsNone(student.leave_class(4))
        self.assertEqual(len(student.list_classes()), 1)

    def test_list_students(self):
        class_group = orm.ClassGroup(orm.connect_db(orm.db_path), 5)
        self.assertEqual(len(class_group.list_students()), 30)

    def test_classgroup_properties(self):
        class_group = orm.ClassGroup(orm.connect_db(orm.db_path), 5)
        self.assertEqual(class_group.id, 5)
        self.assertEqual(class_group.class_name, "class 5")
        self.assertEqual(class_group.pin, 1234)

    def test_model_properties(self):
        model = orm.Model(orm.connect_db(orm.db_path), 160)
        self.assertEqual(model.id, 160)
        self.assertEqual(model.model_name, "model160")
        self.assertEqual(len(model.data_paths), 6)
        self.assertFalse(model.is_trained)


if __name__ == "__main__":
    orm.new_database()
    unittest.main()
