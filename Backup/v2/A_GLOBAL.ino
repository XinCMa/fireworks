#include <FastLED.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

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
#define ASCEND_BUTTON_PIN 4
#define GRADIENT_BUTTON_PIN 5
#define CUSTOMIZE_BUTTON_PIN  10
#define PREVIEW_BUTTON_PIN 5
#define SAVE_BUTTON_PIN 8

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

// 消息类型定义
#define MSG_SAVE_EFFECT    'S'
#define MSG_PLAY_EFFECT    'P'
#define MSG_PREVIEW        'V'
#define MSG_TEST_DATA      'T'

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
  NORMAL_ASCEND,
  STEP_ASCEND,
  PENDULUM_ASCEND
};

// 爆炸模式
enum ExplodeMode {
  NORMAL,
  SHATTER,
  STAR
};

// 激光颜色
enum LaserColor {
  LASER_NONE,
  LASER_GREEN,
  LASER_RED
};

// 存储一次烟花设置的所有信息
struct FireworkEffect {
  CRGB color1;
  CRGB color2;
  uint8_t maxBrightness;
  LaunchMode launchMode;
  LaserColor laserColor;
  int mirrorAngle;
  int explosionLEDCount;
  int speedDelay;
};

/************************************************** 
 *                   全局对象和变量
 **************************************************/
CRGB leds[TOTAL_LED_COUNT];
Servo laserServo;
LiquidCrystal_I2C lcd;

SystemState currentState;
FireworkEffect currentEffect;
FireworkEffect savedEffects[MAX_SAVED_EFFECT];
int savedCount;
int laserPressCount;

const char* modeNames[];

// 添加按钮状态追踪变量
bool lastUpButtonState;
bool lastLaserButtonState;
bool lastPreviewButtonState;
bool lastSaveButtonState;
bool lastBigButtonState;
