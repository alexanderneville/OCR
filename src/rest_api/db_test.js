const sqlite3 = require('sqlite3');

const db = new sqlite3.Database('../../data/application_data.db', (err) => {
    if (err) {
        console.error(err.message);
    }
});

db.all("SELECT * FROM user WHERE username=?", ["user10"], (error, rows) => {
    if (error) {
        console.log(error)
    } else {
        if (rows == undefined){
            console.log("no user");
        } else {
            console.log(rows);
        }
    }
});

async function test() {
    let data = await new Promise((resolve,reject) => {
        db.all("SELECT * FROM class WHERE teacher_id=?", ["1"], (err,rows) => {
         if(err) reject(err);
         resolve(rows);
        })
    })
    console.log(data);
}

test();


db.close((err) => {
    if (err) {
        console.error(err.message);
    }
});
