import numpy as np
import orm, ocr, time

def main():

    conn = orm.connect_db(orm.db_path)
    cursor = conn.cursor()

    while True:
        time.sleep(10)
        tasks = cursor.execute("SELECT * FROM model WHERE labelled=1 AND trained=0 ORDER BY timestamp ASC").fetchall()
        if tasks and len(tasks) > 0:
            model_path = orm.model_path+str(tasks[0][0])+".json"
            ocr.train_new_model(tasks[0][9], tasks[0][7], model_path)
            cursor.execute("UPDATE model SET model_path=?, trained=1 WHERE id=?", [model_path, tasks[0][0]])
            conn.commit()

if __name__ == "__main__":
    main()
