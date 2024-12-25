import serial
import json
import time
import os
from datetime import datetime
from shutil import copyfile
from enum import IntEnum
from dataclasses import dataclass
from typing import Tuple

DATA_DIR = "C:/Users/East/Desktop/fireworks-1/Fireworks"
EFFECTS_FILE = os.path.join(DATA_DIR, "effects_data.json")
BACKUP_DIR = os.path.join(DATA_DIR, "backups")
MAX_BACKUPS = 5

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

@dataclass
class FireworkEffect:
    color1: Tuple[int, int, int]
    color2: Tuple[int, int, int]
    max_brightness: int
    launch_mode: LaunchMode
    gradient_mode: GradientMode
    explode_mode: ExplodeMode
    laser_color: LaserColor
    mirror_angle: int
    explosion_led_count: int
    speed_delay: int

class FireworkController:
    def __init__(self, port='COM8', baudrate=115200):
        self.arduino = serial.Serial(port, baudrate)
        self.effects_data = {}
        self.test_data = []
        self.current_state = 'IDLE'
        self.firework_queue = []  # 存储要播放的烟花序号
        self.is_sequence_playing = False  # 添加标志来追踪烟花长河状态
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

class ArduinoController:
    def __init__(self, port='COM8', baudrate=115200):
        """初始化串口连接"""
        try:
            self.ser = serial.Serial(port, baudrate)
            print(f"Connected to Arduino on {port}")
        except serial.SerialException as e:
            print(f"Failed to connect to Arduino: {e}")
            raise

    def send_effect(self, effect: FireworkEffect):
        """发送效果到Arduino"""
        try:
            # 构建命令字符串
            cmd = f"P,{effect.color1[0]},{effect.color1[1]},{effect.color1[2]}," + \
                  f"{effect.color2[0]},{effect.color2[1]},{effect.color2[2]}," + \
                  f"{effect.max_brightness},{effect.launch_mode.value}," + \
                  f"{effect.gradient_mode.value},{effect.explode_mode.value}," + \
                  f"{effect.laser_color.value},{effect.mirror_angle}," + \
                  f"{effect.explosion_led_count},{effect.speed_delay}\n"
            
            print(f"Sending command: {cmd.strip()}")
            self.ser.write(cmd.encode())
            
            # 等待效果完成
            response = self.ser.readline().decode().strip()
            print(f"Arduino response: {response}")
        except Exception as e:            print(f"Error sending effect: {e}")
        
        raise

    def close(self):
        """关闭串口连接"""
        if hasattr(self, 'ser') and self.ser.is_open:
            self.ser.close()
            print("Connection closed")

def test_effects():
    """测试不同效果组合"""
    controller = ArduinoController()
    try:
        # 测试效果1：普通上升+渐变爆炸
        effect1 = FireworkEffect(
            color1=(255, 0, 0),        # 红色
            color2=(0, 255, 0),        # 绿色
            max_brightness=255,         # 最大亮度
            launch_mode=LaunchMode.NORMAL_ASCEND,
            gradient_mode=GradientMode.GRADIENT,
            explode_mode=ExplodeMode.NORMAL,
            laser_color=LaserColor.LASER_GREEN,
            mirror_angle=90,
            explosion_led_count=100,
            speed_delay=20
        )
        controller.send_effect(effect1)
        time.sleep(20)  # 等待效果完成

        # 测试效果2：断续上升+闪烁爆炸
        effect2 = FireworkEffect(
            color1=(0, 0, 255),        # 蓝色
            color2=(255, 255, 0),      # 黄色
            max_brightness=255,
            launch_mode=LaunchMode.STEP_ASCEND,
            gradient_mode=GradientMode.FADE,
            explode_mode=ExplodeMode.BLINK,
            laser_color=LaserColor.LASER_RED,
            mirror_angle=45,
            explosion_led_count=150,
            speed_delay=30
        )
        controller.send_effect(effect2)
        
    except Exception as e:
        print(f"Test failed: {e}")
    finally:
        controller.close()

if __name__ == "__main__":
    test_effects()
