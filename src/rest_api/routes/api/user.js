const express = require('express');
const salt = require('../../config/keys').salt;
const { User } = require('../../src/classes');
const { create_new_token, authenticate } = require('../../src/authenticate')
const { new_hash, existing_hash } = require('../../src/hash');
const sqlite3 = require("sqlite3");

// Create an express router to handle routes
const router = express.Router();

router.get("/test", ((req, res) => {
    res.json({message: "hello"});
}))

router.post('/register', async (req, res) => {
    if (req.body.username && req.body.password && req.body.name) {
        data = await new_hash(req.body.password).catch(() => res.sendStatus(500));
        const user = new User_db({
            username: req.body.username,
            name: req.body.name,
            password: data.hashed_password,
            salt: data.salt
        });
    } else {
        res.sendStatus(400);
    }
});

router.post('/login', async (req, res) => {
    if (req.body.username && req.body.password) {
        const db = new sqlite3.Database('/home/alex/code/nea/data/application_data.db', (error) => {
            if (error) {
                console.error(error.message);
                res.sendStatus(500);
            }
        });
        db.get("SELECT * FROM user WHERE username=?", [req.body.username], async (error, row) => {
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
                        res.json(row);
                    } else {
                        res.sendStatus(403)
                    }
                }
            }
        });
    } else {
        res.sendStatus(400);
    }
});


router.get('/dev-verify', authenticate, async (req, res) => {
    let user = new User(res.locals.id);
    let user_data = await user.return_info();
    res.json(user_data);
});

// Delete the account of a client making valid, authenticated delete request
router.delete('/delete', authenticate, async (req, res) => {
    let user = new User(res.locals.id);
    await User_db.deleteOne({ _id: user._id });
    res.json({ message: "success" });
});

module.exports = router;
