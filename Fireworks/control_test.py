import serial
import json
import time
import os
import random
from datetime import datetime
from shutil import copyfile
from enum import IntEnum
from dataclasses import dataclass
from typing import Tuple

# 全局配置
DATA_DIR = "C:/Users/East/Desktop/fireworks-1/Fireworks"
EFFECTS_FILE = os.path.join(DATA_DIR, "effects_data.json")
BACKUP_DIR = os.path.join(DATA_DIR, "backups")
MAX_BACKUPS = 5

# 串口配置
ARDUINO_PORT = 'COM8'
ARDUINO_BAUDRATE = 115200

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

class SystemState(IntEnum):
    STATE_IDLE = 0
    STATE_CUSTOMIZE = 1
    STATE_PREVIEW = 2
    STATE_SAVE = 3
    STATE_PLAY_SAVED = 4

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

def generate_random_effect() -> FireworkEffect:
    """生成随机烟花效果"""
    def random_color() -> Tuple[int, int, int]:
        return (random.randint(0, 255),
                random.randint(0, 255),
                random.randint(0, 255))

    return FireworkEffect(
        color1=random_color(),
        color2=random_color(),
        max_brightness=random.randint(128, 255),
        launch_mode=random.choice(list(LaunchMode)),
        gradient_mode=random.choice(list(GradientMode)),
        explode_mode=random.choice(list(ExplodeMode)),
        laser_color=random.choice(list(LaserColor)),
        mirror_angle=random.randint(0, 180),
        explosion_led_count=random.randint(50, 200),
        speed_delay=random.randint(10, 50)
    )

class FireworkController:
    def __init__(self, port=ARDUINO_PORT, baudrate=ARDUINO_BAUDRATE):
        self.arduino = serial.Serial(port, baudrate)
        self.effects_data = {}
        self.test_data = []
        self.system_state = SystemState.STATE_IDLE
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
    def __init__(self, port=ARDUINO_PORT, baudrate=ARDUINO_BAUDRATE):
        """初始化串口连接"""
        try:
            self.ser = serial.Serial(port, baudrate, timeout=31)
            if not self.ser.is_open:
                self.ser.open()  # 打开串口连接
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
            self.ser.write(cmd.encode('ascii'))
            self.ser.flush()
            time.sleep(0.1)
            
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
    """测试不同效果组合，包括随机效果"""
    controller = ArduinoController()
    try:
        # 测试随机生成的效果
        random_effect = generate_random_effect()
        print("Testing random effect:", vars(random_effect))
        controller.send_effect(random_effect)
        time.sleep(20)

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

def get_user_input_effect() -> FireworkEffect:
    """获取用户自定义的烟花效果"""
    print("\n=== 自定义烟花效果 ===")
    
    def get_color(prompt):
        while True:
            try:
                r = int(input(f"{prompt} - 红色 (0-255): "))
                g = int(input(f"{prompt} - 绿色 (0-255): "))
                b = int(input(f"{prompt} - 蓝色 (0-255): "))
                return (r % 256, g % 256, b % 256)
            except ValueError:
                print("请输入有效的数字！")
    
    color1 = get_color("颜色1")
    color2 = get_color("颜色2")
    
    print("\n发射模式:")
    for mode in LaunchMode:
        print(f"{mode.value}: {mode.name}")
    launch_mode = LaunchMode(int(input("选择发射模式: ")))
    
    print("\n渐变模式:")
    for mode in GradientMode:
        print(f"{mode.value}: {mode.name}")
    gradient_mode = GradientMode(int(input("选择渐变模式: ")))
    
    print("\n爆炸模式:")
    for mode in ExplodeMode:
        print(f"{mode.value}: {mode.name}")
    explode_mode = ExplodeMode(int(input("选择爆炸模式: ")))
    
    print("\n激光颜色:")
    for color in LaserColor:
        print(f"{color.value}: {color.name}")
    laser_color = LaserColor(int(input("选择激光颜色: ")))
    
    max_brightness = int(input("\n最大亮度 (0-255): "))
    mirror_angle = int(input("镜像角度 (0-180): "))
    explosion_led_count = int(input("爆炸LED数量 (50-200): "))
    speed_delay = int(input("速度延迟 (10-50): "))
    
    return FireworkEffect(
        color1=color1,
        color2=color2,
        max_brightness=max_brightness,
        launch_mode=launch_mode,
        gradient_mode=gradient_mode,
        explode_mode=explode_mode,
        laser_color=laser_color,
        mirror_angle=mirror_angle,
        explosion_led_count=explosion_led_count,
        speed_delay=speed_delay
    )

def continuous_random_effects():
    """持续生成随机效果直到用户停止"""
    controller = ArduinoController()
    import threading
    import msvcrt
    
    stop_flag = threading.Event()
    
    def check_keyboard_input():
        while not stop_flag.is_set():
            if msvcrt.kbhit():
                key = msvcrt.getch()
                if key == b'q':  # 按q键退出
                    stop_flag.set()
                    break
    
    # 启动键盘监听线程
    input_thread = threading.Thread(target=check_keyboard_input)
    input_thread.daemon = True
    input_thread.start()
    
    print("开始生成随机效果 (按 'q' 停止)")
    try:
        while not stop_flag.is_set():
            effect = generate_random_effect()
            print("\n生成新的随机效果:", vars(effect))
            controller.send_effect(effect)
            time.sleep(5)  # 等待效果完成
    except Exception as e:
        print(f"Error: {e}")
    finally:
        controller.close()
        print("\n停止生成随机效果")

def main_menu():
    """主菜单"""
    while True:
        print("\n=== 烟花控制系统 ===")
        print("1. 持续生成随机效果")
        print("2. 自定义烟花效果")
        print("3. 退出")
        
        choice = input("\n请选择操作: ")
        
        if choice == '1':
            continuous_random_effects()
        elif choice == '2':
            controller = ArduinoController()
            try:
                effect = get_user_input_effect()
                print("\n正在播放自定义效果...")
                controller.send_effect(effect)
            except Exception as e:
                print(f"Error: {e}")
            finally:
                controller.close()
        elif choice == '3':
            print("退出程序")
            break
        else:
            print("无效的选择，请重试")

if __name__ == "__main__":
    main_menu()
