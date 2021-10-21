const salt = require('../config/keys').salt;
const express = require('express');
const jwt = require('jsonwebtoken');
const crypto = require('crypto');
const {User} = require("./orm");
const {open_connection} = require("./db_conn");

function new_hash(password) {
    // given a plaintext password:
    // generate a random salt
    // hash the password and salt together
    // return the salt and the hashed password
    return new Promise((resolve, reject) => {
        const salt = Math.floor((Math.random() * 1000) + 1);
        const hash = crypto.createHash('sha256');
        const unhashed = password + String(salt);
        const hashed_data = hash.update(unhashed, 'utf-8');
        const hashed_password = hashed_data.digest('hex');
        resolve({ hashed_password, salt });
    });
}

function existing_hash(password, salt) {
    // given a plaintext password and a salt:
    // concatenate the two and perform the hash
    // return the hashed string
    return new Promise((resolve, reject) => {
        const hash = crypto.createHash('sha256');
        const unhashed = password + String(salt);
        const hashed_data = hash.update(unhashed, 'utf-8');
        const hashed_password = hashed_data.digest('hex');
        resolve(hashed_password);
    });
}

function create_new_token(user, salt) {
    // take a js object and return a token
    return new Promise((resolve, reject) => {
        jwt.sign(user, String(salt), (err, token) => {
            if (err) {
                reject(err);
            } else {
                resolve(token);
            }
        });

    });
}

function get_bearer_token(string) {
    // return the token from a bearer header
    return new Promise((resolve, reject) => {
        if (string == undefined) {
            reject();
        } else {
            const token = string.split(' ')[1];
            resolve(token);
        }
    });
}

function get_user_from_token(token, salt) {
    // given a token, reverse hash and return js object
    return new Promise((resolve, reject) => {
        jwt.verify(token, salt, (err, data) => {
            if (err) {
                reject();
            } else {
                resolve(data);
            }
        });
    });
}

async function authenticate(req, res, next) {
    // middleware function to inspect header and ensure token validity
    let token = await get_bearer_token(req.headers['authorization']).catch(() => {
        res.sendStatus(400);
        next('route');
    });
    let data = await get_user_from_token(token, salt).catch(() => {
        // reject access if invalid credentials
        res.sendStatus(403);
        next('route');
    });
    // create a user object for remaining code to use.
    let user = new User(await open_connection(), data.id, data.username);
    res.locals.user = user;
    next()
}

module.exports = {new_hash, existing_hash, create_new_token, authenticate};