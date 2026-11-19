import subprocess
import time
import urllib.request
import urllib.parse
import urllib.error
import json

# Запускаем сервер в фоне
server = subprocess.Popen(["./app.exe"])
time.sleep(1.0)  # Ждем старта

BASE = "http://localhost:8080"

def req(path, method="GET"):
    request = urllib.request.Request(BASE + path, method=method)
    try:
        with urllib.request.urlopen(request) as resp:
            return resp.status, json.loads(resp.read().decode("utf-8"))
    except urllib.error.HTTPError as e:
        return e.code, json.loads(e.read().decode("utf-8"))

try:
    # 1. Проверка /init
    status, init_data = req("/init")
    assert status == 200
    assert "keyboard" in init_data and "chords" in init_data and "intervals" in init_data
    print("INIT JSON OK:", json.dumps(init_data, indent=2))

    # 2. Нажатие трезвучия по нотам
    req("/press?type=note&pitch=60", "POST")
    req("/press?type=note&pitch=64", "POST")
    status, state = req("/press?type=note&pitch=67", "POST")
    assert state["keyboard"] == [60, 64, 67]
    assert any(c["chordType"] == "major" and c["rootNote"] == 0 for c in state["chords"])
    print("CHORD RECOGNITION OK:", state["chords"])

    # 3. Проверка фильтрации нот вне диапазона 21..108
    status, state = req("/press?type=note&pitch=10", "POST")
    assert 10 not in state["keyboard"]
    req("/release?type=note&pitch=10", "POST")

    # 4. Проверка URL-encoding с пробелом
    q = urllib.parse.urlencode({"type": "interval", "distance": 4, "intervalType": "major third"})
    status, state = req(f"/press?{q}", "POST")
    assert status == 200
    print("URL ENCODING OK:", state["intervals"])

    # 5. Проверка защиты от падения (кривой pitch и неизвестный type)
    status_bad_num, err1 = req("/press?type=note&pitch=abc", "POST")
    assert status_bad_num == 400
    status_bad_type, err2 = req("/press?type=magic&pitch=60", "POST")
    assert status_bad_type == 400
    print("ERROR HANDLING (400) OK:", err1, err2)

    print("\nALL TESTS PASSED SUCCESSFULLY!")
finally:
    server.terminate()