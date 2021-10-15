const salt = require('../config/keys').salt;
const express = require('express');
const jwt = require('jsonwebtoken');

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
    let user_data = await get_user_from_token(token, salt).catch(() => {
        res.sendStatus(400);
        next('route');
    });
    res.locals.id = user_data.id;
    next()
}

module.exports = {create_new_token, authenticate};