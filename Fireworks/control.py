import serial
import json
import time
import os
from datetime import datetime
from shutil import copyfile
from enum import IntEnum

# 添加存储相关常量
DATA_DIR = "C:/Users/East/Desktop/fireworks-1/Fireworks"
EFFECTS_FILE = os.path.join(DATA_DIR, "effects_data.json")
BACKUP_DIR = os.path.join(DATA_DIR, "backups")
MAX_BACKUPS = 5

# 简化状态定义
STATES = {
    'IDLE': 'IDLE MODE',
    'CUSTOMIZE': 'CUSTOMIZE MODE', 
    'PREVIEW': 'PREVIEW MODE',
    'SAVE': 'SAVE MODE'
}

# 添加与Arduino对应的枚举类
class LaunchMode(IntEnum):
    NORMAL_ASCEND = 0
    STEP_ASCEND = 1
    PENDULUM_ASCEND = 2

class ExplodeMode(IntEnum):
    NORMAL = 0
    BLINK = 1
    RANDOM = 2

class GradientMode(IntEnum):
    GRADIENT = 0
    FADE = 1
    SWITCH = 2

class LaserColor(IntEnum):
    LASER_NONE = 0
    LASER_GREEN = 1
    LASER_RED = 2

class FireworkController:
    def __init__(self, port='COM9', baudrate=115200):
        self.arduino = serial.Serial(port, baudrate)
        self.effects_data = {}
        self.test_data = []
        self.current_state = 'IDLE'
        self.firework_queue = []  # 存储要播放的烟花序号
        self.setup_storage()
        self.load_from_file()
    
    def setup_storage(self):
        """初始化存储目录结构"""
        # 创建数据目录
        os.makedirs(DATA_DIR, exist_ok=True)
        os.makedirs(BACKUP_DIR, exist_ok=True)

    def create_backup(self):
        """创建数据文件的备份"""
        if os.path.exists(EFFECTS_FILE):
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            backup_file = os.path.join(BACKUP_DIR, f"effects_data_{timestamp}.json")
            copyfile(EFFECTS_FILE, backup_file)
            
            # 保持最大备份数量
            backups = sorted(os.listdir(BACKUP_DIR))
            while len(backups) > MAX_BACKUPS:
                os.remove(os.path.join(BACKUP_DIR, backups[0]))
                backups.pop(0)

    def save_to_file(self):
        """保存效果数据到文件"""
        try:
            # 创建备份
            self.create_backup()
            
            # 保存当前数据
            with open(EFFECTS_FILE, 'w') as f:
                json.dump({
                    'last_updated': datetime.now().isoformat(),
                    'effects_count': len(self.effects_data),
                    'effects': self.effects_data
                }, f, indent=2)
            print(f"Effects saved successfully. Total effects: {len(self.effects_data)}")
        except Exception as e:
            print(f"Error saving effects: {e}")

    def load_from_file(self):
        """加载效果数据"""
        try:
            if os.path.exists(EFFECTS_FILE):
                with open(EFFECTS_FILE, 'r') as f:
                    data = json.load(f)
                    self.effects_data = data.get('effects', {})
                print(f"Loaded {len(self.effects_data)} effects from storage")
            else:
                self.effects_data = {}
                print("No existing effects data found")
        except Exception as e:
            print(f"Error loading effects: {e}")
            self.effects_data = {}

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
                # 生成唯一效果ID
                effect_id = len(self.effects_data) + 1
                
                # 修改为与Arduino端对应的数据结构
                effect_data = {
                    'id': effect_id,
                    'color1': {
                        'r': int(parts[1]),
                        'g': int(parts[2]),
                        'b': int(parts[3])
                    },
                    'color2': {
                        'r': int(parts[4]),
                        'g': int(parts[5]),
                        'b': int(parts[6])
                    },
                    'maxBrightness': int(parts[7]),
                    'launchMode': LaunchMode(int(parts[8])),
                    'gradientMode': GradientMode(int(parts[9])),
                    'explodeMode': ExplodeMode(int(parts[10])),
                    'laserColor': LaserColor(int(parts[11])),
                    'mirrorAngle': int(parts[12]),
                    'explosionLEDCount': int(parts[13]),
                    'speedDelay': int(parts[14]),
                    'timestamp': datetime.now().isoformat()
                }
                
                self.effects_data[effect_id] = effect_data
                self.save_to_file()
                print(f"Effect {effect_id} saved with configuration:")
                print(f"  Launch: {effect_data['launchMode'].name}")
                print(f"  Explode: {effect_data['explodeMode'].name}")
                print(f"  Gradient: {effect_data['gradientMode'].name}")
                print(f"  Laser: {effect_data['laserColor'].name}")

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
            # 构建与Arduino端对应的播放命令
            msg = f"P,{effect['color1']['r']},{effect['color1']['g']},{effect['color1']['b']}," + \
                  f"{effect['color2']['r']},{effect['color2']['g']},{effect['color2']['b']}," + \
                  f"{effect['maxBrightness']},{int(effect['launchMode'])}," + \
                  f"{int(effect['gradientMode'])},{int(effect['explodeMode'])}," + \
                  f"{int(effect['laserColor'])},{effect['mirrorAngle']}," + \
                  f"{effect['explosionLEDCount']},{effect['speedDelay']}\n"
            
            self.arduino.write(msg.encode())
            print(f"Playing effect {index} with mode {effect['launchMode'].name}")
    
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

    def get_effect_stats(self):
        """获取效果统计信息"""
        if not self.effects_data:
            return "No effects stored"
            
        stats = {
            'total_effects': len(self.effects_data),
            'last_effect_id': max(self.effects_data.keys()),
            'storage_file': EFFECTS_FILE,
            'backup_count': len(os.listdir(BACKUP_DIR)),
            'modes_used': {
                'launch': set(e['launchMode'].name for e in self.effects_data.values()),
                'explode': set(e['explodeMode'].name for e in self.effects_data.values()),
                'gradient': set(e['gradientMode'].name for e in self.effects_data.values()),
                'laser': set(e['laserColor'].name for e in self.effects_data.values())
            }
        }
        return stats

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
    print("Initial stats:", controller.get_effect_stats())
    controller.run()
