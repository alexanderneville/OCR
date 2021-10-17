const express = require('express');
const salt = require('../../config/keys').salt;
const { User } = require('../../src/orm');
const { new_hash, existing_hash, create_new_token, authenticate } = require('../../src/authenticate')
const sqlite3 = require("sqlite3");
const {open_connection, close_connection} = require("../../src/db_conn");
const {use} = require("express/lib/router");


const router = express.Router();

router.get('/', authenticate, async (req, res) => {
    if (req.query.class_id) {
        let user = res.locals.user;
        console.log(await user.check_class_ownership(req.query.class_id));
        if (await user.check_class_ownership(req.query.class_id) == true) {
            let conn = open_connection();
            conn.get("SELECT * FROM class WHERE id=?", [req.query.class_id], (error, row) => {
                if (error) {
                    res.sendStatus(500);
                } else {
                    conn.all("SELECT username, id FROM user WHERE id in (SELECT student_id FROM class_student WHERE class_id=?)", [req.query.class_id], async (error, rows) => {
                        if (error) {
                            res.sendStatus(500);
                        } else {
                            for (let i = 0; i < rows.length; i++) {
                                let data = await new Promise((resolve, reject) => {
                                    conn.all("SELECT id FROM model WHERE owner_id=?", [rows[i].id], (error, rows) => {
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
                            close_connection(conn);
                            res.json(row);
                        }
                    });
                }
            })
        } else {
            res.sendStatus(403);
        }
    }
    else {
        res.sendStatus(400);
    }
});

router.post('/', authenticate, async (req, res) => {
    if (req.body.class_id && req.body.student_id) {
        let user = res.locals.user;
        if (user.check_class_ownership(req.body.class_id)) {
            user.kick_student(req.body.class_id, req.body.student_id);
        } else {
            res.sendStatus(403);
        }

    } else {

    }
    res.json({messsage: "called with post"});
});


module.exports = router;