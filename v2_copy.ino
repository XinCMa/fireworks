#include <FastLED.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h> // 使用 I2C LCD 库

/************************************************** 
 *                   常量与引脚定义
 **************************************************/
#define LED_PIN          7
#define TOTAL_LED_COUNT  150
#define PREVIEW_LED_COUNT 20
#define MAX_SAVED_EFFECT 10

// 其它硬件
#define SERVO_PIN       9
#define UP_BUTTON_PIN   2
#define LASER_BUTTON_PIN 3
#define BIG_BUTTON_PIN  12
#define PREVIEW_BUTTON_PIN 4
#define SAVE_BUTTON_PIN 5

// 滑杆引脚定义
#define BRIGHTNESS_SLIDER_PIN A12
#define MIRROR_SLIDER_PIN     A13
#define SIZE_SLIDER_PIN       A14
#define SPEED_SLIDER_PIN      A15

// 摇杆引脚定义
#define JOYSTICK1_X_PIN A0
#define JOYSTICK1_Y_PIN A1
#define JOYSTICK2_X_PIN A2
#define JOYSTICK2_Y_PIN A3

/************************************************** 
 *                   枚举与数据结构
 **************************************************/
// 系统状态
enum SystemState {
  STATE_IDLE,
  STATE_CUSTOMIZE,
  STATE_PREVIEW,
  STATE_SAVE,
  STATE_PLAY_SAVED
};

// 上升模式
enum LaunchMode {
  NORMAL_ASCEND,   // 普通上升
  STEP_ASCEND,     // 断续上升
  PENDULUM_ASCEND  // 往复上升
};

// 激光颜色
enum LaserColor {
  LASER_NONE,
  LASER_GREEN,
  LASER_RED
};

// 存储一次烟花设置的所有信息
struct FireworkEffect {
  CRGB color1;             // 前 10 个灯珠颜色
  CRGB color2;             // 后 10 个灯珠颜色
  uint8_t maxBrightness;   // 最大亮度
  LaunchMode launchMode;   // 上升方式
  LaserColor laserColor;   // 激光颜色
  int mirrorAngle;         // 镜子角度(由步进或丝杆决定)
  int explosionLEDCount;   // 爆炸时激活的LED数量
  int speedDelay;          // 升空/爆炸速度 (ms)
  // 以下为尚未详细描述的部分:
  // 渐变 / 闪烁等设置的占位
  // e.g. byte colorTransitionType;
  // e.g. byte strobeType;
};

// 全局数组，用于存储用户保存的效果
FireworkEffect savedEffects[MAX_SAVED_EFFECT];
int savedCount = 0; // 已经保存的数量

/************************************************** 
 *                   全局对象和变量
 **************************************************/
CRGB leds[TOTAL_LED_COUNT];  // 整个烟花灯带
Servo laserServo;            // 激光用舵机
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C 地址 0x27，16x2 LCD

// 当前系统状态
SystemState currentState = STATE_IDLE;

// 用于临时存储自定义的烟花参数
FireworkEffect currentEffect;

// 记录当前激光按钮的连按次数以切换颜色
int laserPressCount = 0;

// 上升模式名称，用于 LCD 显示
const char* modeNames[] = {
  "NORMAL_ASCEND",
  "STEP_ASCEND",
  "PENDULUM_ASCEND"
};

/************************************************** 
 *                   函数声明
 **************************************************/
void enterIdleMode();
void enterCustomizeMode();
void enterPreviewMode();
void saveCurrentEffect();
void playEffect(const FireworkEffect &effect);
void cycleLaunchMode();
void cycleLaserColor();

// 读取交互输入的占位函数
CRGB getColorFromJoystick(int joyPinX, int joyPinY); // 用您自己的方式实现
uint8_t getBrightnessFromSlider();
int getMirrorAngleFromSlider();
int getExplosionLEDCountFromSlider();
int getSpeedFromSlider();

// 具体发射逻辑的子函数 (上升 & 爆炸)
void launchFirework(const FireworkEffect &effect);
void explodeFirework(const FireworkEffect &effect);

// 无人操作时循环播放
void playAllSavedEffectsInLoop();

// 监听 numpad 点播功能（示例占位）
void checkNumpadInput();

/************************************************** 
 *                   setup()
 **************************************************/
void setup() {
  // 初始化串口
  Serial.begin(115200);

  // 初始化 LED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, TOTAL_LED_COUNT);
  FastLED.setBrightness(255);
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
  FastLED.show();

  // 初始化舵机
  laserServo.attach(SERVO_PIN);
  // 假设激光舵机 0 度对准“关闭”状态
  laserServo.write(0);

  // 初始化各按钮
  pinMode(BIG_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PREVIEW_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SAVE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LASER_BUTTON_PIN, INPUT_PULLUP);

  // 初始化 LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("System Idle");

  // 进入空闲模式
  enterIdleMode();
}

/************************************************** 
 *                   loop()
 **************************************************/
void loop() {
  switch (currentState) {
    case STATE_IDLE:
      // 无人操作时循环播放保存的效果
      playAllSavedEffectsInLoop();

      // 检查有没有按下“大开关”来进入定制模式
      if (digitalRead(BIG_BUTTON_PIN) == LOW) {
        enterCustomizeMode();
      }

      // 检查numpad输入（如果有的话）
      checkNumpadInput();
      break;

    case STATE_CUSTOMIZE:
      // 只点亮前 20 颗灯珠做预览
      // color1 由前 10 颗, color2 由后 10 颗
      currentEffect.color1 = getColorFromJoystick(JOYSTICK1_X_PIN, JOYSTICK1_Y_PIN); // 占位：用实际引脚
      currentEffect.color2 = getColorFromJoystick(JOYSTICK2_X_PIN, JOYSTICK2_Y_PIN); // 占位：用实际引脚

      // 设置预览亮度
      currentEffect.maxBrightness = getBrightnessFromSlider();

      // 展示这 20 颗灯珠
      for (int i = 0; i < PREVIEW_LED_COUNT; i++) {
        if (i < 10) {
          leds[i] = currentEffect.color1;
        } else {
          leds[i] = currentEffect.color2;
        }
      }
      FastLED.setBrightness(currentEffect.maxBrightness);
      FastLED.show();

      // 监听上升模式切换按钮
      if (digitalRead(UP_BUTTON_PIN) == LOW) {
        cycleLaunchMode();
        // 在 LCD 显示当前模式名称
        lcd.clear();
        lcd.print("Mode: ");
        lcd.print(modeNames[currentEffect.launchMode]);
        delay(300); // 简单防抖
      }

      // 监听激光按钮
      if (digitalRead(LASER_BUTTON_PIN) == LOW) {
        cycleLaserColor();
        // 简单提示激光颜色
        lcd.clear();
        if (currentEffect.laserColor == LASER_GREEN) {
          lcd.print("Laser: Green");
        } else if (currentEffect.laserColor == LASER_RED) {
          lcd.print("Laser: Red");
        } else {
          lcd.print("Laser: None");
        }
        delay(300); // 防抖
      }

      // 读取另三个滑杆
      currentEffect.mirrorAngle       = getMirrorAngleFromSlider();
      currentEffect.explosionLEDCount = getExplosionLEDCountFromSlider();
      currentEffect.speedDelay        = getSpeedFromSlider();

      // 此处还可添加“(3)条带的颜色（渐变）效果”与“(4)条带的闪烁方式”的输入逻辑，留待后续完善

      // 检查预览按钮
      if (digitalRead(PREVIEW_BUTTON_PIN) == LOW) {
        enterPreviewMode();
      }

      // 检查保存按钮
      if (digitalRead(SAVE_BUTTON_PIN) == LOW) {
        currentState = STATE_SAVE;
      }
      break;

    case STATE_PREVIEW:
      // 执行一次发射，但不保存
      playEffect(currentEffect);

      // 发射完毕后，回到 CUSTOMIZE 状态，以便继续修改
      enterCustomizeMode();
      break;

    case STATE_SAVE:
      // 保存当前设置
      saveCurrentEffect();
      // 立刻播放一次
      playEffect(currentEffect);
      // 然后回到 CUSTOMIZE 继续定制或退出
      enterCustomizeMode();
      break;

    case STATE_PLAY_SAVED:
      // 如果有 numpad 输入，选定播放
      // 放到一个函数里去做即可
      break;

    default:
      break;
  }
}

/************************************************** 
 *            各状态切换 & 核心函数实现
 **************************************************/
/// 进入空闲模式
void enterIdleMode() {
  currentState = STATE_IDLE;
  lcd.clear();
  lcd.print("IDLE MODE");
}

/// 进入自定义模式
void enterCustomizeMode() {
  currentState = STATE_CUSTOMIZE;
  lcd.clear();
  lcd.print("CUSTOMIZE MODE");

  // 为确保只点亮前 20 颗LED，可先清空
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
  FastLED.show();

  // 默认先置一个launchMode
  currentEffect.launchMode = NORMAL_ASCEND;
  currentEffect.laserColor = LASER_NONE;
}

/// 进入预览模式
void enterPreviewMode() {
  currentState = STATE_PREVIEW;
  lcd.clear();
  lcd.print("PREVIEW...");
}

/// 保存当前效果
void saveCurrentEffect() {
  if (savedCount < MAX_SAVED_EFFECT) {
    savedEffects[savedCount] = currentEffect;
    savedCount++;
    Serial.println("Effect saved!");
  } else {
    Serial.println("Save full!");
  }
}

/************************************************** 
 *        播放单个烟花效果 (发射 + 爆炸)
 **************************************************/
void playEffect(const FireworkEffect &effect) {
  // 1. 发射
  launchFirework(effect);

  // 2. 爆炸
  explodeFirework(effect);

  // 完成后的处理……
}

/************************************************** 
 *   launchFirework & explodeFirework 仅示例
 **************************************************/
// 上升
void launchFirework(const FireworkEffect &effect) {
  switch (effect.launchMode) {
    case NORMAL_ASCEND:
      // (1)普通上升：白头亮，尾巴暗的条带，上升到顶部
      // 实际上可用循环控制LED从下往上点亮
      // 这里只留个伪代码
      Serial.println(">> Normal Ascend");
      // TODO: 在这里根据 MAX_BRIGHTNESS、UP_STRIP_LENGTH = 30 等进行条带移动
      break;

    case STEP_ASCEND:
      // (2)断续上升：30个灯珠分成5小段
      Serial.println(">> Step Ascend");
      // TODO: 同理留伪代码或函数
      break;

    case PENDULUM_ASCEND:
      // (3)往复上升：每上升 50 个灯珠再向下 20 个，如此往复
      Serial.println(">> Pendulum Ascend");
      break;
  }

  // 激光部分：根据 effect.laserColor 转动舵机
  // 绿色激光 / 红色激光 / 无
  // 示例：
  if (effect.laserColor == LASER_GREEN) {
    // 让舵机转到绿色激光所在位置(比如 90 度)，并打开激光
    Serial.println("Laser: Green On");
    laserServo.write(90);
    // TODO: 同时可以让舵机做一点旋转动画
  } else if (effect.laserColor == LASER_RED) {
    Serial.println("Laser: Red On");
    laserServo.write(180);
  } else {
    Serial.println("Laser: Off");
    laserServo.write(0);
  }

  // 镜子角度(步进电机控制)，这里仅打印
  Serial.print("Mirror angle: ");
  Serial.println(effect.mirrorAngle);

  // TODO: 在上升过程中考虑 delay / speedDelay
}

/************************************************** 
 *    爆炸
 **************************************************/
void explodeFirework(const FireworkEffect &effect) {
  // 爆炸 LED 数量 effect.explosionLEDCount
  // 爆炸速度 effect.speedDelay
  // 这里也需要配合渐变色或闪烁方式(后续再补)
  Serial.println("Explode Firework!");
  // 示例：点亮指定数量的 LED
  // ...
}

/************************************************** 
 *      无用户操作时循环播放所有已保存效果
 **************************************************/
void playAllSavedEffectsInLoop() {
  if (savedCount == 0) {
    // 如果没有已保存的，就保持全黑或做一些默认动画
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    FastLED.show();
    return;
  }

  // 循环播放
  static unsigned long lastPlayTime = 0;
  static int effectIndex = 0;

  unsigned long now = millis();
  if (now - lastPlayTime > 5000) { // 每隔5秒播放下一个（示例）
    // 播放当前效果
    playEffect(savedEffects[effectIndex]);

    // 下一个
    effectIndex = (effectIndex + 1) % savedCount;
    lastPlayTime = now;
  }
}

/************************************************** 
 *          监听 numpad 点播功能(示例占位)
 **************************************************/
void checkNumpadInput() {
  // 如果检测到用户输入了某个数字n
  // 并且 n 在已保存效果的范围内，就播放
  // 这里只是示例占位
}

/************************************************** 
 *        上升模式/激光颜色 切换函数
 **************************************************/
void cycleLaunchMode() {
  if (currentEffect.launchMode == NORMAL_ASCEND) {
    currentEffect.launchMode = STEP_ASCEND;
  } else if (currentEffect.launchMode == STEP_ASCEND) {
    currentEffect.launchMode = PENDULUM_ASCEND;
  } else {
    currentEffect.launchMode = NORMAL_ASCEND;
  }
}

void cycleLaserColor() {
  // 每按一次递增
  laserPressCount++;
  // 绿色(1), 红色(2), 无(0), 循环
  int modResult = laserPressCount % 3;
  switch (modResult) {
    case 1: currentEffect.laserColor = LASER_GREEN; break;
    case 2: currentEffect.laserColor = LASER_RED;   break;
    default: currentEffect.laserColor = LASER_NONE; break;
  }
}

/************************************************** 
 *    下面是一些占位函数，供您后续实现
 **************************************************/
// 从摇杆获取颜色
CRGB getColorFromJoystick(int joyPinX, int joyPinY) {
  // 可以移植您最后贴出的“多按钮混色”代码，或真正用(joyPinX, joyPinY)计算HUE/SAT
  // 这里演示：先返回一个随机颜色
  // 实际请替换成您自己的摇杆取色逻辑
  CRGB result = CHSV(random8(), 255, 255);
  return result;
}

// 滑杆读取最大亮度
uint8_t getBrightnessFromSlider() {
  int sliderValue = analogRead(BRIGHTNESS_SLIDER_PIN);
  // 映射到 0-255
  return map(sliderValue, 0, 1023, 0, 255);
}

// 滑杆控制镜子角度 (步进电机)
int getMirrorAngleFromSlider() {
  int val = analogRead(MIRROR_SLIDER_PIN);
  // 比如映射到 0-180 度
  return map(val, 0, 1023, 0, 180);
}

// 滑杆控制爆炸时LED数量
int getExplosionLEDCountFromSlider() {
  int val = analogRead(SIZE_SLIDER_PIN);
  // 您可根据总LED数量做映射
  return map(val, 0, 1023, 0, TOTAL_LED_COUNT);
}

// 滑杆控制升空与爆炸速度
int getSpeedFromSlider() {
  int val = analogRead(SPEED_SLIDER_PIN);
  // 映射成毫秒级别
  return map(val, 0, 1023, 10, 500); // 举例
}
