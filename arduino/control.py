import serial
import time

# 配置串口
arduino = serial.Serial('COM3', 9600)

while True:
    try:
        # 读取Arduino的数据
        if arduino.in_waiting > 0:
            data = arduino.readline().decode('utf-8').strip()
            print(f"Received: {data}")

        # 如果需要可以发送指令
        # Example: arduino.write(b'1')  # 发送'1'作为指令

        time.sleep(0.1)
    except KeyboardInterrupt:
        print("Program Stopped")
        break
