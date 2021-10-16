const sqlite3 = require('sqlite3');
const {open_connection} = require("./db_conn");

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
                this.conn.all("SELECT * FROM class WHERE teacher_id=?", ["1"], (err, rows) => {
                    if (err) {
                        reject(err);
                    } else {
                        resolve(rows);
                    }
                });
            });
            console.log(data);
            resolve(data)
        });
    }

    check_class_ownership(class_id) {
        this.conn.get('SELECT * FROM class WHERE id=? AND teacher_id=?', [class_id, this.id], (err, row) => {
            if (row == undefined) {
                return false;
            } else {
                return true;
            }
        })

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
};

module.exports = { User };
