const express = require('express');
const salt = require('../../config/keys').salt;
const { User } = require('../../src/classes');
const { new_hash, existing_hash, create_new_token, authenticate } = require('../../src/authenticate')
const sqlite3 = require("sqlite3");
const {open_connection, close_connection} = require("../../src/db_conn");
const {use} = require("express/lib/router");


const router = express.Router();

router.get('/', authenticate, async (req, res) => {
    console.log(req.query.id);
    res.sendStatus(200);
});

router.post('/', async (req, res) => {
    res.json({messsage: "called with post"});
});


module.exports = router;