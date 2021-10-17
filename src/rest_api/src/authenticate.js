const salt = require('../config/keys').salt;
const express = require('express');
const jwt = require('jsonwebtoken');
const crypto = require('crypto');
const {User} = require("./orm");
const {open_connection} = require("./db_conn");

function new_hash(password) {
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
    return new Promise((resolve, reject) => {
        const hash = crypto.createHash('sha256');
        const unhashed = password + String(salt);
        const hashed_data = hash.update(unhashed, 'utf-8');
        const hashed_password = hashed_data.digest('hex');
        resolve(hashed_password);
    });
}

function create_new_token(user, salt) {
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
    let token = await get_bearer_token(req.headers['authorization']).catch(() => {
        res.sendStatus(400);
        next('route');
    });
    let data = await get_user_from_token(token, salt).catch(() => {
        res.sendStatus(403);
        next('route');
    });
    let user = new User(await open_connection(), data.id, data.username);
    res.locals.user = user;
    next()
}

module.exports = {new_hash, existing_hash, create_new_token, authenticate};