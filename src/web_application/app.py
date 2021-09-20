from flask import Flask, session, json, render_template, request, redirect, url_for, abort, flash, get_flashed_messages, jsonify
import os
import orm

app = Flask(__name__)
app.config.from_object(__name__)
app.config.update({'SECRET_KEY': orm.secret_key })

def handle_form_data(data, keys):

    fields = []

    for i in range(len(keys)):
        if keys[i] in data:
            if request.form[keys[i]] == '':
                raise orm.Insufficient_Data()
            else:
                fields.append(request.form[keys[i]])
        else:
            raise orm.Insufficient_Data()

    return fields

@app.route('/login', methods=['GET', 'POST'])
def login():

    if request.method == "GET":

        print("get request to /login")
        return render_template('login.html')

    else:

        try:

            if not request.form:
                raise orm.Insufficient_Data()

            keys = ["username", "password"]
            fields = handle_form_data(request.form, keys)

            current_user = orm.login_user(fields[0], fields[1])

            if current_user is not None:

                print(current_user.id)
                session['user'] = current_user.id
                return redirect(url_for('home'))

        except orm.Invalid_Credentials:

            flash("Invalid login details")
            return render_template('login.html')

        except orm.Insufficient_Data:
            flash("Fill in every field")
            return render_template('login.html')

@app.route('/register', methods=['GET', 'POST'])
def register():

    if request.method == "GET":

        print("get request to /register")
        return render_template('register.html')

    else:

        try:

            if not request.form:
                raise orm.Insufficient_Data()

            keys = ["username", "password", "full_name", "role"]
            fields = handle_form_data(request.form, keys)

            conn = orm.connect_db(orm.db_path)

            print(fields[0])
            print(fields[1])
            print(fields[2])
            print(fields[3])

            orm.validate_password(fields[1])

            if fields[3] == "teacher":
                new_user_id = orm.Teacher.create(conn, fields[0], fields[2], fields[1])
            else:
                new_user_id = orm.Student.create(conn, fields[0], fields[2], fields[1])

            session["user"] = new_user_id
            return redirect(url_for('home'))

        except orm.Existing_Username:

            flash("That username is already taken")
            return render_template('register.html')

        except orm.Insecure_Password:

            flash("Password must be at least 8 characters, contatining one upper case, one lower case, one numeric and one special character")
            return render_template('register.html')

        except orm.Insufficient_Data:
            flash("Fill in every field")
            return render_template('register.html')

@app.route('/home')
def home():

    if session["user"]:

        conn = orm.connect_db(orm.db_path)
        current_user = orm.create_user_object(conn, session["user"])

        if isinstance(current_user, orm.Teacher):
            conn = orm.connect_db(orm.db_path)
            classes = [orm.ClassGroup(conn, i[0]) for i in current_user.list_classes()]
            return render_template('teacher_home.html', user = current_user, classes=classes)
        else:
            return render_template('student_home.html', user = current_user)

    else:
        return redirect(url_for('index'))

@app.route('/index')
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/logout')
def logout():
    session['user'] = None
    return redirect(url_for('home'))








@app.route('/join_class', methods=['GET', 'POST'])
def join_class():

    if session["user"]:

        current_user = orm.create_user_object(orm.connect_db(orm.db_path), session["user"])

        if isinstance(current_user, orm.Teacher):
            return redirect(url_for('home'))

        if request.method == "GET":
            return render_template('join_class.html')
        else:

            try:

                if not request.form:
                    raise orm.Insufficient_Data
                keys = ["class_id", "pin"]
                fields = handle_form_data(request.form, keys)
                current_user.join_class(fields[0], fields[1])
                return redirect(url_for("home"))

            except orm.Insufficient_Data:
                flash("Fill in both fields")
                return render_template('join_class.html')
            except orm.Invalid_Credentials:
                flash("Invalid join code or pin")
                return render_template('join_class.html')
            except orm.Existing_Member:
                flash("Already a member of this class")
                return render_template('join_class.html')

    else:
        return redirect(url_for('index'))

@app.route('/create_model')
def create_model():
    if session["user"]:
        return render_template('create_model.html')
    else:
        return redirect(url_for('index'))

@app.route('/create_class', methods=['GET', 'POST'])
def create_class():
    print("create_class function")
    if session["user"]:

        current_user = orm.create_user_object(orm.connect_db(orm.db_path), session["user"])
        if isinstance(current_user, orm.Student):
            return redirect(url_for('home'))
        if request.method == 'GET':
            return render_template('create_class.html')
        else:

            try:

                if not request.form:
                    raise orm.Insufficient_Data
                keys = ["class_name", "pin"]

                fields = handle_form_data(request.form, keys)
                print(fields)
                current_user.create_class(fields[0], fields[1])
                return redirect(url_for("home"))

            except orm.Insufficient_Data:
                flash("Fill in both fields")
                return render_template('create_class.html')
            except orm.Invalid_Credentials:
                flash("Invalid join code or pin")
                return render_template('create_class.html')
            except orm.Existing_Class:
                flash("Existing class with this name")
                return render_template('create_class.html')

    else:

        return redirect(url_for('index'))





@app.route('/_leave_class', methods=['POST'])
def _leave_class():
    print("_leave_class function")
    data = request.get_json()
    print(data)

    try:
        student = orm.create_user_object(orm.connect_db(orm.db_path), data['user_id'])
        student.leave_class(data['class_id'])
    except orm.No_Such_ID:
        return jsonify({'status', 0})
    except orm.Invalid_Role:
        return jsonify({'status', 0})

    return jsonify({'status': 1})

@app.route('/_delete_model', methods=['POST'])
def _delete_model():
    print("_delete_model function")
    data = request.get_json()
    print(data)

    try:

        user = orm.create_user_object(orm.connect_db(orm.db_path), data['user_id'])
        user.delete_model(data['model_id'])

    except orm.No_Such_ID:
        return jsonify({'status', 0})
    except orm.Invalid_Role:
        return jsonify({'status', 0})

    return jsonify({'status': 1})

@app.route("/_kick_student", methods=['POST'])
def _kick_student():
    print("_kick_student function")
    data = request.get_json()
    print(data)

    try:
        teacher = orm.create_user_object(orm.connect_db(orm.db_path), data['user_id'])
        teacher.kick_student(data["student_id"], data["class_id"])
        return jsonify({"status": 1})
    except:
        return jsonify({"status": 0})

@app.route("/_delete_class", methods=['POST'])
def _delete_class():
    print("_delete_class function")
    data = request.get_json()
    print(data)

    try:
        teacher = orm.create_user_object(orm.connect_db(orm.db_path), data['user_id'])
        teacher.delete_class(data["class_id"])
        return jsonify({"status": 1})
    except orm.Invalid_Credentials:
        return jsonify({"status": 0})


if __name__ == "__main__":

    app.run(host='0.0.0.0', port=80, debug=True)
