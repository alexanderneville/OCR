const express = require('express');
const salt = require('../../config/keys').salt;
const { User } = require('../../src/orm');
const { new_hash, existing_hash, create_new_token, authenticate } = require('../../src/authenticate')
const sqlite3 = require("sqlite3");
const {open_connection, close_connection} = require("../../src/db_conn");
const {use} = require("express/lib/router");


const router = express.Router();

router.get('/', authenticate, async (req, res) => {
    if (req.query.model_id) {
        let user = res.locals.user;
        if (await user.check_model_ownership(req.query.model_id)) {
            let data = await user.get_model_info(req.query.model_id);
            res.json(data);
        } else {
            res.sendStatus(403);
        }
    } else {
        res.sendStatus(400);
    }

});

router.post('/', async (req, res) => {
    res.json({messsage: "called with post"});
});

router.put('/', authenticate, async (req, res) => {
    if (req.body.nn_model && req.body.model_id) {
        let user = res.locals.user;
        if (await user.check_valid_model_modification(req.body.model_id)) {
            await user.update_model(req.body.model_id, req.body.nn_model);
            res.json(await user.get_model_info(req.body.model_id));
        } else {
            res.sendStatus(403);
        }
    } else {
        res.sendStatus(400);
    }
});

router.delete('/', authenticate, async (req, res) => {
    if (req.query.model_id) {
        let user = res.locals.user;
        if (await user.check_valid_model_modification(req.query.model_id)) {
            await user.delete_model(req.query.model_id);
            res.sendStatus(200);
        } else {
            res.sendStatus(403);
        }
    } else {
        res.sendStatus(400);
    }
});


module.exports = router;