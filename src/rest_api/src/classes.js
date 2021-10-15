const sqlite3 = require('sqlite3');

class User {

    constructor(conn, id) {
        this.id = id;
        this.conn = conn;
    }

};

module.exports = { User };
