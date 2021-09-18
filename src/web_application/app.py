from flask import Flask, session, json, render_template, request, redirect, url_for, abort, flash, get_flashed_messages
import os
import orm

app = Flask(__name__)
app.config.from_object(__name__)
app.config.update({'SECRET_KEY': orm.secret_key })

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
            fields = []

            for i in range(len(keys)):
                if keys[i] in request.form:
                    if request.form[keys[i]] == '':
                        raise orm.Insufficient_Data()
                    else:
                        fields.append(request.form[keys[i]])
                else:
                    raise orm.Insufficient_Data()

            current_user = orm.login_user(fields[0], fields[1])

            if current_user is not None:

                print(current_user.id)
                session['user'] = current_user.id
                return redirect(url_for('home'))

        except orm.Invalid_Credentials:

            flash("Invalid login details")
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
            fields = []

            for i in range(len(keys)):
                if keys[i] in request.form:
                    if request.form[keys[i]] == '':
                        raise orm.Insufficient_Data()
                    else:
                        fields.append(request.form[keys[i]])
                else:
                    raise orm.Insufficient_Data()

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
            flash("Fill out every field")
            return render_template('register.html')

@app.route('/logout')
def logout():
    session['user'] = None
    return "logged out"

@app.route('/home')
def home():

    if not session["user"]:
        return redirect(url_for('login'))

    conn = orm.connect_db(orm.db_path)
    current_user = orm.create_user_object(conn, session["user"])

    if isinstance(current_user, orm.Teacher):
        return render_template('teacher_home.html', name = current_user.full_name)
    else:
        return render_template('student_home.html', name = current_user.full_name)

if __name__ == "__main__":

    app.run()
