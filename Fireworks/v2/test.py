import serial
import time
import random

def generate_random_effect():
    return {
        'color1': (random.randint(0,255), random.randint(0,255), random.randint(0,255)),
        'color2': (random.randint(0,255), random.randint(0,255), random.randint(0,255)),
        'maxBrightness': random.randint(100,255),
        'launchMode': random.randint(0,2),
        'gradientMode': random.randint(0,2),
        'explodeMode': random.randint(0,2),
        'laserColor': random.randint(0,2),
        'mirrorAngle': random.randint(0,30),
        'explosionLEDCount': random.randint(10,50),
        'speedDelay': random.randint(3,8)
    }

import random
import colorsys

def generate_choice_random_effect():
    # Helper function to calculate contrast
    def calculate_brightness(rgb):
        # Perceived brightness formula
        return 0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2]

    # Generate two random colors with strong contrast
    while True:
        color1 = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        color2 = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        
        brightness1 = calculate_brightness(color1)
        brightness2 = calculate_brightness(color2)
        
        # Check brightness contrast difference
        if abs(brightness1 - brightness2) > 125:  # Adjust this threshold as needed
            break

    return {
        'color1': color1,
        'color2': color2,
        'maxBrightness': random.randint(100, 255),
        'launchMode': random.randint(0, 2),
        'gradientMode': random.randint(1, 2),
        'explodeMode': random.randint(1, 2),
        'laserColor': random.randint(0, 1),
        'mirrorAngle': random.randint(0, 30),
        'explosionLEDCount': random.randint(10, 50),
        'speedDelay': random.randint(3, 8)
    }


def send_effect(ser, effect):
    command = f"P,{effect['color1'][0]},{effect['color1'][1]},{effect['color1'][2]}," + \
              f"{effect['color2'][0]},{effect['color2'][1]},{effect['color2'][2]}," + \
              f"{effect['maxBrightness']},{effect['launchMode']},{effect['gradientMode']}," + \
              f"{effect['explodeMode']},{effect['laserColor']},{effect['mirrorAngle']}," + \
              f"{effect['explosionLEDCount']},{effect['speedDelay']}\n"
    
    try:
        ser.write(command.encode())
        print(f"发送命令: {command.strip()}")
        return True
    except Exception as e:
        print(f"发送错误: {e}")
        return False

def main():
    try:
        ser = serial.Serial('COM8', 115200, timeout=1)
        time.sleep(2)  # 等待连接稳定
        
        while True:
            effect = generate_choice_random_effect()
            if send_effect(ser, effect):
                time.sleep(10)  # 每5秒发送一个新效果
            
    except KeyboardInterrupt:
        print("\n程序已停止")
    except Exception as e:
        print(f"错误: {e}")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("串口已关闭")

if __name__ == "__main__":
    main()
