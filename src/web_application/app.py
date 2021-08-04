from flask import Flask, session, json, render_template, request, redirect, url_for, abort, flash, get_flashed_messages
import os
import orm
from deployment import secret_key

app = Flask(__name__)
app.config.from_object(__name__)

app.config.update({'SECRET_KEY': secret_key })

@app.route('/login', methods=['GET', 'POST'])
def login():

    if request.method == "GET":

        print("get request to /login")
        return render_template('login.html')

    else:

        try:

            username = request.form['username']
            password = request.form['password'] 

            print(username)
            print(password)

            current_user = orm.login_user(username, password)

            print('hello')

            if current_user is not None:

                print(current_user.id)
                session['user'] = current_user.id
                return redirect(url_for('home'))

            else:

                print("current user is none")
                raise Exception()

        except:

            flash("Invalid login details")
            return render_template('login.html')

@app.route('/register', methods=['GET', 'POST'])
def register():

    if request.method == "GET":

        print("get request to /register")
        return render_template('register.html')

    else:

        try:

            username = request.form['username']
            password = request.form['password'] 
            full_name = request.form['full_name']
            role = request.form['role']

            conn = orm.connect_db(orm.db_path)

            print(username)
            print(password)
            print(full_name)
            print(role)

            if role == "teacher":
                new_user_id = orm.Teacher.create(conn, username, full_name, password)
            else:
                new_user_id = orm.Student.create(conn, username, full_name, password)

            if new_user_id == None:

                raise Exception()

            else: 
                
                session["user"] = new_user_id
                return redirect(url_for('home'))

        except:

            flash("Invalid login details")
            return render_template('register.html')

@app.route('/logout')
def logout():
    del session['user']
    return "logged out"

@app.route('/home')
def home():

    if not session['user']:
        return redirect(url_for('login'))

    conn = orm.connect_db(orm.db_path)
    current_user = orm.create_user_object(conn, session["user"])
    return current_user.username

if __name__ == "__main__":

    app.run()
