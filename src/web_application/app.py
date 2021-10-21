from flask import Flask, session, json, render_template, request, redirect, url_for, abort, flash, get_flashed_messages, \
    jsonify
import os, time
import orm, ocr, pipeline
from datetime import datetime

app = Flask(__name__)
app.config.from_object(__name__)
app.config.update({'SECRET_KEY': orm.secret_key})


def handle_form_data(data, keys):
    """ Pass a dictionary and a set of keys which must be extracted from it. """
    
    # prepare a list to store results
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
    """ Endpoint for the login page, supports GET and POST. """
    
    if request.method == "GET":

        return render_template('login.html')

    else:

        try:

            if not request.form:
                raise orm.Insufficient_Data()

            keys = ["username", "password"]
            fields = handle_form_data(request.form, keys)
            current_user = orm.login_user(fields[0], fields[1])
            print(current_user.id)
            session['user'] = current_user.id
            return redirect(url_for('home'))

        except orm.Invalid_Credentials:

            flash("Invalid login details")
            return render_template('login.html')

        except orm.No_Such_ID:

            flash("Invalid login details")
            return render_template('login.html')

        except orm.Insufficient_Data:
            flash("Fill in every field")
            return render_template('login.html')


@app.route('/register', methods=['GET', 'POST'])
def register():
    """ Endpoint for the registration page, supports GET and POST. """

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

            flash(
                "Password must be at least 8 characters, contatining one upper case, one lower case, one numeric and one special character")
            return render_template('register.html')

        except orm.Insufficient_Data:

            flash("Fill in every field")
            return render_template('register.html')


@app.route('/home')
def home():
    """ Serve the home page to logged in users. """
    
    if "user" in session and session["user"]:

        current_user = orm.create_user_object(orm.connect_db(orm.db_path), session["user"])

        if isinstance(current_user, orm.Teacher):

            classes = [orm.ClassGroup(orm.connect_db(orm.db_path), i[0]) for i in current_user.list_classes()]
            details = []
            for i in classes:
                students = [orm.create_user_object(orm.connect_db(orm.db_path), j[0]) for j in i.list_students()]
                models = [[orm.Model(orm.connect_db(orm.db_path), model[0]) for model in student.list_models()] for
                          student in students]
                class_data = [[j[0], j[1]] for j in zip(students, models)]
                details.append([i, class_data])
            models = [orm.Model(orm.connect_db(orm.db_path), model[0]) for model in current_user.list_models()]
            return render_template('teacher_home.html', user=current_user, models=models, details=details)

        else:

            models = [orm.Model(orm.connect_db(orm.db_path), model[0]) for model in current_user.list_models()]
            return render_template('student_home.html', user=current_user, models=models)

    else:

        return redirect(url_for('index'))


@app.route('/index')
@app.route('/')
def index():
    return render_template('index.html')


@app.route('/logout')
def logout():
    session['user'] = None
    return redirect(url_for('index'))


# create/join/leave class

@app.route('/create_class', methods=['GET', 'POST'])
def create_class():
    print("create_class function")
    if "user" in session and session["user"]:

        current_user = orm.create_user_object(orm.connect_db(orm.db_path), session["user"])

        if isinstance(current_user, orm.Student):
            return redirect(url_for('home'))

        if request.method == 'GET':
            return render_template('create_class.html')

        else:

            try:

                if not request.form:
                    raise orm.Insufficient_Data()

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


@app.route('/join_class', methods=['GET', 'POST'])
def join_class():
    if "user" in session and session["user"]:

        current_user = orm.create_user_object(orm.connect_db(orm.db_path), session["user"])

        if isinstance(current_user, orm.Teacher):
            return redirect(url_for('home'))

        if request.method == "GET":

            return render_template('join_class.html')

        else:

            try:

                if not request.form:
                    raise orm.Insufficient_Data()
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


@app.route('/_leave_class', methods=['POST'])
def _leave_class():
    print("_leave_class function")
    data = request.get_json()
    print(data)

    try:

        student = orm.create_user_object(orm.connect_db(orm.db_path), data['user_id'])
        student.leave_class(data['class_id'])
        return jsonify({'status': 1})

    except orm.No_Such_ID:

        return jsonify({'status', 0})

    except orm.Invalid_Role:

        return jsonify({'status', 0})


# create/train/use model

@app.route('/create_model', methods=['GET', 'POST'])
def create_model():
    if "user" in session and session["user"]:

        if request.method == 'GET':

            return render_template('create_model.html')

        else:

            current_user = orm.create_user_object(orm.connect_db(orm.db_path), session["user"])

            try:

                if not request.form:
                    raise orm.Insufficient_Data()

                if not request.form['model_name']:
                    raise orm.Insufficient_Data()

                id = current_user.create_model(request.form['model_name'])
                infile_name = str(id) + ".png"
                uploaded = request.files['infile']

                if uploaded.filename == '':
                    raise orm.Insufficient_Data()

                uploaded.save(orm.tmp_path + infile_name)
                image_checker = pipeline.Pipeline()
                rc = image_checker.check_header(orm.tmp_path + infile_name)
                del (image_checker)

                if rc == 1:

                    os.rename(orm.tmp_path + infile_name, orm.infile_path + infile_name)

                    image_processor = pipeline.Pipeline()
                    image_processor.load_file(orm.infile_path + infile_name)
                    image_processor.scan_image()
                    image_processor.generate_dataset(orm.dataset_path + str(id) + ".txt",
                                                     orm.sample_path + str(id) + ".txt",
                                                     orm.info_path + str(id) + ".json")
                    image_processor.save_to_file(orm.outfile_path + infile_name)

                    conn = orm.connect_db(orm.db_path)
                    cursor = conn.cursor()
                    cursor.execute(
                        'UPDATE model SET infile_path=?, outfile_path=?, dataset_path=?, sample_path=?, info_path=? WHERE id=?',
                        [orm.infile_path + infile_name, orm.outfile_path + infile_name,
                         orm.dataset_path + str(id) + ".txt", orm.sample_path + str(id) + ".txt",
                         orm.info_path + str(id) + ".json", id])
                    conn.commit()
                    cursor.close()
                    conn.close()
                    return redirect(url_for('home'))

                else:

                    raise orm.Invalid_FileType()

            except orm.Invalid_FileType:

                os.remove(orm.tmp_path + infile_name)
                current_user.delete_model(id)
                flash('Invalid image file (PNG only)')
                return redirect(url_for('create_model'))

            except orm.Insufficient_Data:

                flash('Fill in every field')
                return redirect(url_for('create_model'))

            except orm.Existing_Model:

                flash('Models must have a unique name')
                return redirect(url_for('create_model'))

    else:

        return redirect(url_for('index'))


@app.route('/train_model', methods=["GET", "POST"])
def train_model():
    if request.method == 'GET':

        if session['user']:

            model_id = request.args.get('model_id')

            if model_id:

                return render_template('train_model.html',
                                       model=orm.Model(orm.connect_db(orm.db_path), int(model_id)),
                                       user=orm.create_user_object(orm.connect_db(orm.db_path),
                                                                   session["user"]))

            else:

                return redirect(url_for('home'))

        else:

            return redirect(url_for('index'))

    else:  # post request was made

        data = request.get_json()
        current_user = orm.create_user_object(orm.connect_db(orm.db_path), data["user_id"])
        models = current_user.list_models()
        ids = [int(element[0]) for element in models]
        if int(data["model_id"]) not in ids:
            return jsonify({'status': 0})

        current_model = orm.Model(orm.connect_db(orm.db_path), data["model_id"])
        if current_model.is_labelled:
            return jsonify({'status': 0, 'complete': 1})

        info = ocr.get_info(current_model.data_paths[4])
        labels = [character["label"] for character in info["characters"]]

        if "label" in data:

            if data["label"] == None or data["label"] == "":

                try:

                    position = labels.index(None)
                    del (info["characters"][position])
                    ocr.save_info(info, current_model.data_paths[4])

                except ValueError:

                    return jsonify({'status': 0, 'complete': 1})

            else:

                try:

                    position = labels.index(None)
                    info["characters"][position]["label"] = data["label"]
                    ocr.save_info(info, current_model.data_paths[4])

                except ValueError:

                    return jsonify({'status': 0, 'complete': 1})

        try:

            labels = [character["label"] for character in info["characters"]]
            position = labels.index(None)  # find the next unlabelled element
            new_character = ocr.get_single_character(position, current_model.data_paths[3])
            # print(new_character)
            return jsonify({'status': 1, 'complete': 0, 'new_character': new_character})

        except ValueError:  # this means everything has been labelled

            current_model.set_labelled()
            return jsonify({'status': 1, 'complete': 1})


@app.route('/use_model', methods=['GET', 'POST'])
def use_model():

    if "user" in session and session['user']:

        current_user = orm.create_user_object(orm.connect_db(orm.db_path), session["user"])

        if request.method == 'GET':

            model_id = request.args.get('model_id')
            print(model_id)

            if model_id == None or model_id == '':
                return redirect(url_for('home'))

            available_models = current_user.list_models()
            model_ids = [str(i[0]) for i in available_models]
            if str(model_id) not in model_ids:
                if isinstance(current_user, orm.Teacher):
                    available_models = current_user.list_all_models()
                    model_ids = [str(i[1]) for i in available_models]
                    if str(model_id) not in model_ids:
                        return redirect(url_for('home'))
                else:
                    return redirect(url_for('home'))

            return render_template('use_model.html', model_id=model_id)

        else:

            infile_path = orm.tmp_path + str(current_user.id) + "_infile" + ".png"
            dataset_path = orm.tmp_path + str(current_user.id) + "_dataset" + ".txt"
            sample_path = orm.tmp_path + str(current_user.id) + "_sample" + ".txt"
            info_path = orm.tmp_path + str(current_user.id) + "_info" + ".json"

            try:

                if not request.form:
                    raise orm.Insufficient_Data()

                keys = ["model_id"]
                fields = handle_form_data(request.form, keys)
                current_model = orm.Model(orm.connect_db(orm.db_path), fields[0])

                # save and check the infile
                uploaded = request.files['infile']
                if uploaded.filename == '':
                    raise orm.Insufficient_Data()
                uploaded.save(infile_path)
                image_checker = pipeline.Pipeline()
                rc = image_checker.check_header(infile_path)
                del (image_checker)

                if rc == 1:

                    image_processor = pipeline.Pipeline()
                    image_processor.load_file(infile_path)
                    image_processor.scan_image()
                    image_processor.generate_dataset(dataset_path, sample_path, info_path)
                    text = ocr.use_existing_model(info_path, sample_path, current_model.model_path)
                    current_user.create_cache(text)  # just a prototype for now
                    os.remove(infile_path)
                    os.remove(dataset_path)
                    os.remove(sample_path)
                    os.remove(info_path)
                    del (image_processor)

                else:

                    raise orm.Invalid_FileType()  # TODO move this exception to ocr package

                return redirect(url_for("view_cache"))

            except orm.Invalid_FileType:
                os.remove(infile_path)
                flash('Invalid image file (PNG only)')
                return render_template('create_class.html')
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

        return redirect(url_for("/index"))


@app.route("/view_cache")
def view_cache():
    if "user" in session and session["user"]:

        return render_template('view_cache.html',
                               user=orm.create_user_object(orm.connect_db(orm.db_path), session["user"]))

    else:

        return redirect(url_for('index'))


@app.route('/_delete_cache', methods=['POST'])
def _delete_cache():
    print("_delete_model function")
    data = request.get_json()
    print(data)
    user = orm.create_user_object(orm.connect_db(orm.db_path), data['user_id'])
    user.delete_cache(data["cache_id"])

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
    app.run(host='0.0.0.0', port=8000, debug=True)
