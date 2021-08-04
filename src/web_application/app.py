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

            if current_user is not None:

                session['user'] = current_user.id
                return redirect(url_for('home'))

            else:

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
            role = request.form['role']

            print(username)
            print(password)
            print(role)

            return 'success'

        except:

            return render_template('login.html')

@app.route('/home')
def home():

    conn = orm.connect_db(orm.db_path)
    current_user = orm.create_user(conn, session["user"])
    return current_user.username

if __name__ == "__main__":
    app.run()
