import serial
import json
import time
from datetime import datetime

# 简化状态定义
STATES = {
    'IDLE': 'IDLE MODE',
    'CUSTOMIZE': 'CUSTOMIZE MODE', 
    'PREVIEW': 'PREVIEW MODE',
    'SAVE': 'SAVE MODE'
}

class FireworkController:
    def __init__(self, port='COM3', baudrate=115200):
        self.arduino = serial.Serial(port, baudrate)
        self.effects_data = {}
        self.test_data = []
        self.current_state = 'IDLE'
        self.current_mode = None
        self.current_laser = None
        self.current_explode = 0
        self.firework_queue = []  # 存储要播放的烟花序号
    
    def process_message(self, msg):
        # 1. 状态跟踪
        if 'MODE' in msg:
            for state, state_msg in STATES.items():
                if state_msg in msg:
                    self.current_state = state
                    print(f"State changed to: {self.current_state}")
                    
                    # 当进入IDLE模式时,开始播放烟花长河
                    if state == 'IDLE' and len(self.effects_data) > 0:
                        self.start_firework_sequence()
                    return

        # 2. 保存效果
        if msg.startswith('S,'):
            parts = msg.split(',')
            if len(parts) >= 15:
                effect_data = {
                    'color1': {'r': int(parts[1]), 'g': int(parts[2]), 'b': int(parts[3])},
                    'color2': {'r': int(parts[4]), 'g': int(parts[5]), 'b': int(parts[6])},
                    'brightness': int(parts[7]),
                    'launchMode': int(parts[8]),
                    'params': parts[9:],  # 保存其他参数
                    'timestamp': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                }
                index = len(self.effects_data) + 1
                self.effects_data[index] = effect_data
                self.save_to_file()
                print(f"Effect {index} saved")

        elif msg.startswith('S,'):  # 测试数据
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
    
    def start_firework_sequence(self):
        """开始烟花长河播放"""
        print("Starting firework sequence...")
        # 按保存顺序创建播放队列
        self.firework_queue = list(self.effects_data.keys())
        self.play_next_firework()

    def play_next_firework(self):
        """播放队列中的下一个烟花"""
        if not self.firework_queue:
            print("Firework sequence completed")
            return

        index = self.firework_queue.pop(0)
        self.play_effect(index)
        
        # 设置下一个烟花的播放
        if self.firework_queue:
            # 3秒后播放下一个
            time.sleep(3)
            self.play_next_firework()

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
                    if msg:  # 只处理非空消息
                        self.process_message(msg)
                time.sleep(0.1)
            except KeyboardInterrupt:
                print("Controller stopped")
                break
            except Exception as e:
                print(f"Error: {e}")
                continue

if __name__ == "__main__":
    controller = FireworkController()
    controller.load_from_file()
    controller.run()
