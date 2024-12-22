import serial
import time

# 配置串口
arduino = serial.Serial('COM3', 9600)
fireworks_data = {}

def store_firework_data(data):
    parts = data.split(',')
    if len(parts) == 2:
        firework_number = parts[0]
        parameters = parts[1]
        fireworks_data[firework_number] = parameters
        print(f"Stored: Firework {firework_number} with parameters {parameters}")

def replay_firework(firework_number):
    if firework_number in fireworks_data:
        parameters = fireworks_data[firework_number]
        arduino.write(f"{firework_number},{parameters}\n".encode('utf-8'))
        print(f"Replaying: Firework {firework_number} with parameters {parameters}")
    else:
        print(f"Firework {firework_number} not found")

while True:
    try:
        # 读取Arduino的数据
        if arduino.in_waiting > 0:
            data = arduino.readline().decode('utf-8').strip()
            print(f"Received: {data}")
            store_firework_data(data)

        # 检查是否有重放请求
        for i in range(1, 9):
            if arduino.in_waiting > 0:
                numpad_data = arduino.readline().decode('utf-8').strip()
                if numpad_data.isdigit() and 1 <= int(numpad_data) <= 8:
                    replay_firework(numpad_data)

        time.sleep(0.1)
    except KeyboardInterrupt:
        print("Program Stopped")
        break
