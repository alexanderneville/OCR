from flask import Flask
import os
import orm

app = Flask(__name__)
db_path = os.getcwd() + 'data.db' 

@app.route('/login')
def login():
    return 'login'

@app.route('/register')
def register():
    return 'register'

@app.route('/home')
def home():
    return 'home'

if __name__ == "__main__":
    app.run()
