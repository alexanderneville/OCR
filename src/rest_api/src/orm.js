const sqlite3 = require('sqlite3');
const fs = require("fs");
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

    list_classes() {
        return new Promise((resolve, reject) => {
            this.conn.all("SELECT * FROM class WHERE teacher_id=?", [this.id], (error, rows) => {
                if (error) {
                    reject(error);
                } else {
                    resolve(rows);
                }
            });
        });
    }

    list_models() {
        return new Promise(async (resolve, reject) => {
            this.conn.all("SELECT * FROM model WHERE owner_id=?", [this.id], (error, rows) => {
                if (error) {
                    reject(error);
                } else {
                    resolve(rows);
                }
            });
        });
    }

    list_all_models() {
        return new Promise((resolve, reject) => {
            this.conn.all("SELECT * FROM model WHERE owner_id in (SELECT student_id FROM class_student WHERE class_id = (SELECT id FROM class WHERE teacher_id=?));", [this.id], (error, rows) => {
                if (error) {
                    reject(error);
                } else {
                    resolve(rows);
                }
            });
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

    create_model(model_name) {
        return new Promise((resolve, reject) => {
            this.conn.run('INSERT INTO model (owner_id, name, trained, labelled, timestamp) VALUES (?,?,0,0,?)', [this.id, model_name, Date.now()], (error) => {
                if (error) {
                    reject();
                } else {
                   resolve();
                }
            });
        });
    }

    get_model_info(model_id) {
        return new Promise((resolve, reject) => {
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

    update_model(model_id, nn_model) {
        return new Promise((resolve, reject) => {
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

    delete_model(model_id) {
        return new Promise((resolve, reject) => {
            this.conn.run("DELETE FROM model WHERE id=?", [model_id], (error) => {
                if (error) {
                    reject(error);
                } else {
                    resolve();
                }
            });
        });
    }

    get_class_info(class_id) {
        return new Promise((resolve, reject) => {
            this.conn.get("SELECT * FROM class WHERE id=?", [class_id], (error, row) => {
                if (error) {
                    reject();
                } else {
                    this.conn.all("SELECT username, id FROM user WHERE id in (SELECT student_id FROM class_student WHERE class_id=?)", [class_id], async (error, rows) => {
                        if (error) {
                            reject();
                        } else {
                            for (let i = 0; i < rows.length; i++) {
                                let data = await new Promise((resolve, reject) => {
                                    this.conn.all("SELECT id FROM model WHERE owner_id=?", [rows[i].id], (error, rows) => {
                                        if (error) {
                                            reject(error);
                                        } else {
                                            resolve(rows);
                                        }
                                    });
                                });
                                rows[i].models = [];
                                for (let j = 0; j < data.length; j++) {
                                    rows[i].models.push(data[j].id);
                                }
                            }
                            row.students = rows;
                            resolve(row);
                        }
                    });
                }
            })
        });
    }

    delete_class(class_id) {
        return new Promise((resolve, reject) => {
            this.conn.run('DELETE FROM class WHERE id=?', [class_id], (error) => {
                if (error) {
                    reject();
                } else {
                   resolve();
                }
            });
        });
    }

    create_class(class_name, pin) {
        return new Promise((resolve, reject) => {
            this.conn.run('INSERT INTO class (teacher_id, class_name, pin) VALUES (?,?,?)', [this.id, class_name, pin], (error) => {
                if (error) {
                    reject();
                } else {
                   resolve();
                }
            });
        });
    }

    kick_student(class_id, student_id) {
        return new Promise((resolve, reject) => {
            this.conn.run('DELETE FROM class_student WHERE class_id=? AND student_id=?', [class_id, student_id], (error) => {
                if (error) {
                    reject();
                } else {
                   resolve();
                }
            });
        });
    }

};

module.exports = { User };
