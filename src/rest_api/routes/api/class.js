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
            let data = await user.get_class_info(req.query.class_id);
            res.json(data);
        } else {
            res.sendStatus(403);
        }
    }
    else {
        res.sendStatus(400);
    }
});

router.post('/', authenticate, async (req, res) => {
    if (req.body.class_name && req.body.pin) {
        let user = res.locals.user;
        try {
            await user.create_class(req.body.class_name, req.body.pin);
            let conn = open_connection();
            conn.get("SELECT id FROM class WHERE teacher_id=? ORDER BY id DESC LIMIT 1;", [user.return_data().id], async (error, row) => {
                if (error) {
                    throw error;
                } else {
                    let data = await user.get_class_info(row.id);
                    res.json(data);
                }
            });
        } catch (error) {
            res.sendStatus(403);
        }
    } else {
        res.sendStatus(400);
    }
});

router.delete('/', authenticate, async (req, res) => {
    let user = res.locals.user;
    if (req.query.class_id && req.query.student_id) {
        if (await user.check_class_ownership(req.query.class_id) == true) {
            await user.kick_student(req.query.class_id, req.query.student_id);
            res.sendStatus(200);
        } else {
            res.sendStatus(403);
        }
    } else if (req.query.class_id) {
        if (await user.check_class_ownership(req.query.class_id) == true) {
            await user.delete_class(req.query.class_id);
            res.sendStatus(200);
        } else {
            res.sendStatus(403);
        }
    } else {

    }
});

module.exports = router;