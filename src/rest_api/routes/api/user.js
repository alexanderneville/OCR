const express = require('express');
const salt = require('../../config/keys').salt;
const { User } = require('../../src/classes');
const { new_hash, existing_hash, create_new_token, authenticate } = require('../../src/authenticate')
const sqlite3 = require("sqlite3");
const {open_connection, close_connection} = require("../../src/db_conn");
const {use} = require("express/lib/router");


const router = express.Router();

router.get("/test", ((req, res) => {
    res.json({message: "hello"});
}))

router.post('/login', async (req, res) => {
    if (req.body.username && req.body.password) {
        try {
            const conn = await open_connection()
            conn.get("SELECT * FROM user WHERE username=?", [req.body.username], async (error, row) => {
                if (error) {
                    console.log(error)
                } else {
                    if (row == undefined){
                        console.log("no user");
                        res.json({message: "no user"});
                    } else {
                        let hashed = await existing_hash(req.body.password, row.salt);
                        console.log(hashed);
                        console.log(row.password);
                        if (hashed == row.password) {
                            let user = new User(conn, row.id, req.body.username);
                            let data = user.return_data();
                            let token = await create_new_token(data, salt);
                            user.end_conn();
                            res.json({token: token});
                        } else {
                            res.sendStatus(403)
                        }
                    }
                }
            });
        } catch(error) {
            res.sendStatus(500);
        }
    } else {
        res.sendStatus(400);
    }
});

router.get('/dev-verify', authenticate, async (req, res) => {
    let user = res.locals.user;
    res.json(user.return_data());
});

router.get('/list-classes',authenticate, async (req, res) => {
    let user = res.locals.user;
    let classes = await user.list_classes();
    console.log(classes);
    res.json({my_classes: classes});
})

module.exports = router;
