const sqlite3 = require('sqlite3');
const db_path = require('../config/paths').db_path;

function open_connection() {
    const conn = new sqlite3.Database(db_path, (err) => {});
    return conn;
}

function close_connection(conn) {
   conn.close((err) => {
       if (err) {
           console.error(err.message);
       }
    });
}

module.exports = {open_connection, close_connection};
