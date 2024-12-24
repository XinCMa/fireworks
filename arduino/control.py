import serial
import json
import time
from datetime import datetime

class FireworkController:
    def __init__(self, port='COM3', baudrate=115200):
        self.arduino = serial.Serial(port, baudrate)
        self.effects_data = {}
        self.test_data = []
        
    def process_message(self, msg):
        parts = msg.split(',')
        msg_type = parts[0]
        
        if msg_type == 'S':  # 保存效果
            effect_data = {
                'color1': {'r': int(parts[1]), 'g': int(parts[2]), 'b': int(parts[3])},
                'color2': {'r': int(parts[4]), 'g': int(parts[5]), 'b': int(parts[6])},
                'brightness': int(parts[7]),
                'launchMode': int(parts[8]),
                'laserColor': int(parts[9]),
                'mirrorAngle': int(parts[10]),
                'explosionCount': int(parts[11]),
                'speed': int(parts[12]),
                'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            }
            index = len(self.effects_data) + 1
            self.effects_data[index] = effect_data
            self.save_to_file()
            print(f"Effect {index} saved")
            
        elif msg_type == 'T':  # 测试数据
            self.test_data.append({
                'type': parts[1],
                'name': parts[2],
                'value': parts[3:],
                'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            })
            print(f"Test data received: {parts[1]} - {parts[2]}")
    
    def play_effect(self, index):
        if index in self.effects_data:
            effect = self.effects_data[index]
            msg = f"P,{effect['color1']['r']},{effect['color1']['g']},{effect['color1']['b']}," + \
                  f"{effect['color2']['r']},{effect['color2']['g']},{effect['color2']['b']}," + \
                  f"{effect['brightness']},{effect['launchMode']},{effect['laserColor']}," + \
                  f"{effect['mirrorAngle']},{effect['explosionCount']},{effect['speed']}\n"
            self.arduino.write(msg.encode())
            print(f"Playing effect {index}")
    
    def save_to_file(self):
        with open('effects_data.json', 'w') as f:
            json.dump(self.effects_data, f, indent=2)
        
        with open('test_data.json', 'w') as f:
            json.dump(self.test_data, f, indent=2)
    
    def load_from_file(self):
        try:
            with open('effects_data.json', 'r') as f:
                self.effects_data = json.load(f)
        except FileNotFoundError:
            self.effects_data = {}
    
    def run(self):
        print("Firework Controller Started")
        while True:
            try:
                if self.arduino.in_waiting > 0:
                    msg = self.arduino.readline().decode().strip()
                    self.process_message(msg)
                time.sleep(0.1)
            except KeyboardInterrupt:
                print("Controller stopped")
                break

if __name__ == "__main__":
    controller = FireworkController()
    controller.load_from_file()
    controller.run()
