const sqlite3 = require('sqlite3');
const fs = require("fs");
const {open_connection} = require("./db_conn");
const model_path = require('../config/paths').model_path;

class User {

    constructor(conn, id, username) {
        this.id = id;
        this.conn = conn;
        this.username = username;
    }

    return_data() {
        let data = {
            id: this.id,
            username: this.username
        }
        return data;
    }

    end_conn() {
        this.conn.close((err) => {
            if (err) {
                console.error(err.message);
            }
        });
    }

    async list_classes() {
        return new Promise(async (resolve, reject) => {
            let data = await new Promise((resolve, reject) => {
                this.conn.all("SELECT * FROM class WHERE teacher_id=?", [this.id], (error, rows) => {
                    if (error) {
                        reject(error);
                    } else {
                        resolve(rows);
                    }
                });
            });
            console.log(data);
            resolve(data)
        });
    }

    async list_models() {
        return new Promise(async (resolve, reject) => {
            let data = await new Promise((resolve, reject) => {
                this.conn.all("SELECT * FROM model WHERE owner_id=?", [this.id], (error, rows) => {
                    if (error) {
                        reject(error);
                    } else {
                        resolve(rows);
                    }
                });
            });
            resolve(data)
        });
    }

    async list_all_models() {
        return new Promise(async (resolve, reject) => {
            let data = await new Promise((resolve, reject) => {
                this.conn.all("SELECT * FROM model WHERE owner_id in (SELECT student_id FROM class_student WHERE class_id = (SELECT id FROM class WHERE teacher_id=?));", [this.id], (error, rows) => {
                    if (error) {
                        reject(error);
                    } else {
                        resolve(rows);
                    }
                });
            });
            resolve(data)
        });
    }

    async check_class_ownership(class_id) {
        return new Promise(async (resolve, reject) => {
            this.conn.get('SELECT * FROM class WHERE id=? AND teacher_id=?', [class_id, this.id], (error, row) => {
                if (row == undefined) {
                    resolve(false);
                } else {
                    resolve(true);
                }
            });
        });
    }

    async check_model_ownership(model_id) {
        return new Promise(async (resolve, reject) => {
            this.conn.get("SELECT * FROM model WHERE id=? AND (owner_id in (SELECT student_id FROM class_student WHERE class_id = (SELECT id FROM class WHERE teacher_id=?)) OR owner_id=?);", [model_id, this.id, this.id], (err, row) => {
                if (row == undefined) {
                    resolve(false);
                } else {
                    resolve(true);
                }
            });
        });
    }

    async check_valid_model_modification(model_id) {
        return new Promise(async (resolve, reject) => {
            this.conn.get("SELECT * FROM model WHERE id=? AND owner_id=?;", [model_id, this.id], (error, row) => {
                if (row == undefined) {
                    resolve(false);
                } else {
                    resolve(true);
                }
            });
        });
    }

    async get_model_info(model_id) {
        return new Promise(async (resolve, reject) => {
            this.conn.get("SELECT * FROM model WHERE id=?;", [model_id], (error, row) => {
                if (error) {
                    reject(error);
                } else {
                    if (row.trained == 1) {
                        let raw = fs.readFileSync(row.model_path);
                        let model_as_json = JSON.parse(raw);
                        row.nn_model = model_as_json;
                    }
                    resolve(row);
                }
            });
        });
    }

    async update_model(model_id, nn_model) {
        return new Promise(async (resolve, reject) => {
            this.conn.run("UPDATE model SET model_path=?, labelled=1, trained=1 WHERE id=?", [model_path+model_id.toString()+".json", model_id], (error, row) => {
                if (error) {
                    reject(error);
                } else {
                    const data = JSON.stringify(nn_model);
                    fs.writeFileSync(model_path+model_id.toString()+".json", data, (error) => {
                        if (error) {
                            reject(error);
                        }
                    });
                    resolve();
                }
            });
        });
    }

    async delete_model(model_id) {
        return new Promise(async (resolve, reject) => {
            this.conn.run("DELETE FROM model WHERE id=?", [model_id], (error) => {
                if (error) {
                    reject(error);
                } else {
                    resolve();
                }
            });
        });
    }

    delete_class(class_id) {
        db.run('DELETE FROM class WHERE id=?', [class_id], (error) => {
            if (error) {
                console.log(error);
            }
        });
    }

    new_class(class_name, pin) {
        db.run('INSERT INTO class (teacher_id, class_name, pin) VALUES (?,?,?)', [this.id, class_name, pin], (error) => {
            if (error) {
                return false;
            } else {
                return true;
            }
        });
    }

    kick_student(class_id, student_id) {}

};

module.exports = { User };
