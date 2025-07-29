# Robot Mesafe Verisi Takip ve Haritalama Sistemi | Distance Tracking and Mapping System for Robot

## 📌 Proje Açıklaması | Project Description

Bu proje, bir robotun çevresel mesafe sensörlerinden aldığı verilerin Flask API üzerinden **MySQL veritabanına** aktarılmasını ve ardından bu verilerle **Python (Matplotlib)** kullanılarak robotun haritasının oluşturulmasını amaçlamaktadır.

This project is designed to collect distance measurements from a robot's surrounding sensors, store them in a **MySQL database** via a **Flask API**, and then generate a visual map using **Python (Matplotlib)**.

---

## 🧱 Bileşenler | Components

### 1. `veri_aktar.py`
- Flask tabanlı bir REST API sunucusudur.
- Robotun gönderdiği `front`, `back`, `left`, `right` verilerini alır ve veritabanına kaydeder.

### 2. `harita.py`
- MySQL veritabanından verileri çeker.
- Gürültü filtresi (hareketli ortalama) uygular.
- Robotun rotasını ve çevresini matplotlib ile görselleştirir.

---

## 🛠️ Kullanılan Teknolojiler | Technologies Used

- Python 3
- Flask
- MySQL & SQLAlchemy
- Matplotlib
- PyMySQL / mysql-connector-python

---

## ⚙️ Kurulum | Installation

### Gerekli kütüphaneler | Required libraries

pip install flask mysql-connector-python matplotlib sqlalchemy pymysql


Veritabanı Yapısı | Database Structure
sql

CREATE DATABASE robot_data;

CREATE TABLE mesafe_verileri (
    id INT AUTO_INCREMENT PRIMARY KEY,
    front FLOAT,
    back FLOAT,
    left FLOAT,
    right FLOAT,
    timestamp DATETIME
);
