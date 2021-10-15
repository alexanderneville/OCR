const sqlite3 = require('sqlite3');

const db = new sqlite3.Database('../../data/application_data.db', (err) => {
    if (err) {
        console.error(err.message);
    }
});

db.get("SELECT * FROM user WHERE username=?", ["user10"], (error, row) => {
    if (error) {
        console.log(error)
    } else {
        if (row == undefined){
            console.log("no user");
        } else {
            console.log(row);
        }
    }
});


db.close((err) => {
    if (err) {
        console.error(err.message);
    }
});
