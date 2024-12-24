#ifndef A_GLOBAL_H
#define A_GLOBAL_H

#include <FastLED.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

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
  SHATTER,
  STAR
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
  LaserColor laserColor;
  int mirrorAngle;
  int explosionLEDCount;
  int speedDelay;
};

// 全局变量声明
extern CRGB leds[TOTAL_LED_COUNT];
extern Servo laserServo;
extern LiquidCrystal_I2C lcd;

extern SystemState currentState;
extern FireworkEffect currentEffect;
extern FireworkEffect savedEffects[MAX_SAVED_EFFECT];
extern int savedCount;
extern int laserPressCount;


bool lastAscendButtonState = HIGH;
bool lastLaserButtonState = HIGH;
bool lastPreviewButtonState = HIGH;
bool lastSaveButtonState = HIGH;
bool lastCustomizeButtonState = HIGH;

#endif