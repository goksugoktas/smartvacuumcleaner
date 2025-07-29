from flask import Flask, request
import mysql.connector
from datetime import datetime

app = Flask(__name__)

def veritabani_baglanti():
    return mysql.connector.connect(
        host="localhost",
        user="root",
        password="",
        database="robot_data"
    )

@app.route('/mesafe', methods=['POST'])
def mesafe_verisi():
    try:
        front = float(request.form['front'])
        back = float(request.form['back'])
        right = float(request.form['right'])
        left = float(request.form['left'])
        zaman = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        conn = veritabani_baglanti()
        cursor = conn.cursor()

        sql = "INSERT INTO mesafe_verileri (front, back, `right`, `left`, timestamp) VALUES (%s, %s, %s, %s, %s)"
        cursor.execute(sql, (front, back, right, left, zaman))
        conn.commit()

        return "Veri başarıyla kaydedildi.\n", 200
    except Exception as e:
        return f"Hata oluştu: {e}\n", 400

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)