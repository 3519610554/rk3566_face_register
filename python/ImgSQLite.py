import os
import sqlite3

image_path = os.getcwd() + '/sql'
image_sqlite = image_path + "/image.db"

class ImgSQLite:
    def __init__(self):
        if not os.path.exists(image_path):
            os.makedirs(image_path)
        conn = sqlite3.connect(image_sqlite)
        cursor = conn.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS images (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                time TEXT NOT NULL,
                base64 TEXT NOT NULL
            )
        ''')
        conn.commit()
        conn.close()

    def insert(self, in_time, in_base64):
        conn = sqlite3.connect(image_sqlite)
        cursor = conn.cursor()
        cursor.execute('''
            INSERT INTO images (time, base64) VALUES (?, ?)
        ''', (in_time, in_base64))
        conn.commit()
        last_id = cursor.lastrowid
        conn.close()
        return last_id

    def get_row_count(self):
        conn = sqlite3.connect(image_sqlite)
        cursor = conn.cursor()
        cursor.execute("SELECT COUNT(*) FROM images")
        row_count = cursor.fetchone()[0]
        conn.close()
        return row_count

    def delete_by_id(self, in_id):
        conn = sqlite3.connect(image_sqlite)
        cursor = conn.cursor()
        cursor.execute('''
            DELETE FROM images WHERE id = ?
        ''', (in_id,))
        conn.commit()
        conn.close()

    def get_all_data(self):
        conn = sqlite3.connect(image_sqlite)
        cursor = conn.cursor()
        cursor.execute("SELECT id, time, base64 FROM images")
        rows = cursor.fetchall()
        conn.close()
        rows.reverse()
        return rows
