const crypto = require('crypto');

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

module.exports = { new_hash, existing_hash };
