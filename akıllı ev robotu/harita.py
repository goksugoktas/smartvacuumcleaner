import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from sqlalchemy import create_engine, text
import statistics

# MySQL bağlantı bilgileri
DATABASE_URL = "mysql+pymysql://root:@localhost/robot_data"

try:
    # SQLAlchemy engine oluştur
    engine = create_engine(DATABASE_URL)

    with engine.connect() as connection:
        result = connection.execute(text("SELECT front, back, `left`, `right` FROM mesafe_verileri"))
        veriler = result.fetchall()

except Exception as e:
    print("Bağlantı hatası veya sorgu hatası:", e)
    veriler = []

def temizle(degerler, pencere=5):
    """Hareketli ortalama filtresi (simple moving average)"""
    filtreli = []
    for i in range(len(degerler)):
        baslangic = max(0, i - pencere + 1)
        pencere_verisi = degerler[baslangic:i + 1]
        filtreli.append(statistics.mean(pencere_verisi))
    return filtreli

if veriler:
    # Verileri ayrı listelere ayır
    front_list = [f for f, _, _, _ in veriler]
    back_list = [b for _, b, _, _ in veriler]
    left_list = [l for _, _, l, _ in veriler]
    right_list = [r for _, _, _, r in veriler]

    # Gürültü filtreleme
    front_filt = temizle(front_list)
    back_filt = temizle(back_list)
    left_filt = temizle(left_list)
    right_filt = temizle(right_list)

    # Robotun başlangıç konumu ve sabit adımı
    robot_x, robot_y = 0, 0
    adim = 10
    robot_yolu = [(robot_x, robot_y)]

    plt.figure(figsize=(12, 12))

    for i in range(len(front_filt)):
        front = front_filt[i]
        back = back_filt[i]
        left = left_filt[i]
        right = right_filt[i]

        x, y = robot_x, robot_y

        front_point = (x, y + front)
        back_point = (x, y - back)
        left_point = (x - left, y)
        right_point = (x + right, y)

        plt.scatter(*front_point, color='red', label='Front' if i == 0 else "")
        plt.scatter(*back_point, color='blue', label='Back' if i == 0 else "")
        plt.scatter(*left_point, color='green', label='Left' if i == 0 else "")
        plt.scatter(*right_point, color='orange', label='Right' if i == 0 else "")
        plt.scatter(x, y, color='black', marker='x', label='Robot' if i == 0 else "")

        robot_y += adim
        robot_yolu.append((robot_x, robot_y))

    xler, yler = zip(*robot_yolu)
    plt.plot(xler, yler, color='black', linestyle='--', label='Robot Yolu')

    plt.title("Robot Haritası (Gürültü Filtreli)")
    plt.xlabel("X (cm)")
    plt.ylabel("Y (cm)")
    plt.axis("equal")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()
else:
    print("Veritabanından veri çekilemedi veya bağlantı kurulamadı.")
