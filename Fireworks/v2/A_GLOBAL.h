#ifndef A_GLOBAL_H
#define A_GLOBAL_H

#include <FastLED.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// 常量与引脚定义
#define LED_PIN          7
#define TOTAL_LED_COUNT  280
#define PREVIEW_LED_COUNT 20

#define SERVO_PIN       9
#define ASCEND_BUTTON_PIN   2
#define LASER_BUTTON_PIN 3
#define CUSTOMIZE_BUTTON_PIN  10
#define PREVIEW_BUTTON_PIN 5
#define SAVE_BUTTON_PIN 8
#define GRADIENT_BUTTON_PIN 11
#define EXPLIDE_BUTTON_PIN 12

#define BRIGHTNESS_SLIDER_PIN A12
#define MIRROR_SLIDER_PIN     A13
#define SIZE_SLIDER_PIN       A14
#define SPEED_SLIDER_PIN      A15



#define JOYSTICK1_X_PIN A0
#define JOYSTICK1_Y_PIN A1
#define JOYSTICK2_X_PIN A2
#define JOYSTICK2_Y_PIN A3 
#define MSG_SAVE_EFFECT    'S'
#define MSG_PLAY_EFFECT    'P'
#define MSG_PREVIEW        'V'
#define MSG_TEST_DATA      'T'

// 在枚举定义后添加 modeNames 数组声明
extern const char* modeNames[];

// 枚举与数据结构
enum SystemState {
  STATE_IDLE,
  STATE_CUSTOMIZE,
  STATE_PREVIEW,
  STATE_SAVE,
  STATE_PLAY_SAVED
};

enum LaunchMode {
  NORMAL_ASCEND,
  STEP_ASCEND,
  PENDULUM_ASCEND
};

enum ExplodeMode {
  NORMAL,
  BLINK,
  RANDOM
};

enum GradientMode{
  GRAIDENT,
  FADE,
  SWITCH
};

enum LaserColor {
  LASER_NONE,
  LASER_GREEN,
  LASER_RED
};

struct FireworkEffect {
  CRGB color1;
  CRGB color2;
  uint8_t maxBrightness;
  LaunchMode launchMode;
  GradientMode gradientMode;
  ExplodeMode explodeMode;
  LaserColor laserColor;
  int mirrorAngle;
  int explosionLEDCount;
  int speedDelay;
};

// 全局变量声明
extern CRGB leds[TOTAL_LED_COUNT];
extern Servo laserServo;
extern LiquidCrystal_I2C lcd;
extern Keypad keypad;

extern const int minEffect;
extern const int maxEffect;

extern SystemState currentState;
extern FireworkEffect currentEffect;
extern int laserPressCount;

extern bool defaultAscendButtonState;
extern bool defaultLaserButtonState;
extern bool defaultPreviewButtonState;
extern bool defaultSaveButtonState;
extern bool defaultCustomizeButtonState;
extern bool defaultExplodeButtonState;
extern bool defaultGradientButtonState;


struct ExplosionParams {
  CRGB color1;          // 起始颜色 (渐变的第一端)
  CRGB color2;          // 结束颜色 (渐变的第二端)
  uint8_t speedDelay;   // 条带移动时的延时(毫秒)，数值越小移动越快
  int stripLen;         // 条带长度 (如 30)
  int moveRange;        // 条带移动范围 (如 80)
};

#endif