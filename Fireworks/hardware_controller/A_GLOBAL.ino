#include "A_GLOBAL.h"

const char* modeNames[] = {
    "NORMAL",
    "STEP",
    "PENDULUM"
};


bool defaultAscendButtonState = HIGH;
bool defaultLaserButtonState = LOW;
bool defaultPreviewButtonState = HIGH;
bool defaultSaveButtonState = HIGH;
bool defaultCustomizeButtonState = LOW;
bool defaultExplodeButtonState = LOW;
bool defaultGradientButtonState = LOW;

uint8_t lastHueA = 0;           // 记录最后的颜色 Hue 值
uint8_t lastHueB = 0;           // 记录最后的颜色 Hue 值
bool inDeadzoneA = true;        // Joystick 是否在死区
bool inDeadzoneB = true;        // Joystick 是否在死区




// 数字输入缓冲区
char inputBuffer[MAX_INPUT_DIGITS + 1] = {0};
int inputIndex = 0;

char keys[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[4] = {9, 8, 7, 6};
byte colPins[3] = {5, 4, 3};    
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);

const int minEffect = 1;
const int maxEffect = 9;

// 删除重复定义的全局变量
// CRGB leds[TOTAL_LED_COUNT];  // 移除此行
// Servo laserServo;            // 移除此行
LiquidCrystal_I2C lcd(0x27, 16, 2);

SystemState currentState = STATE_IDLE;
// FireworkEffect currentEffect;  // 移除此行，已在hardware_controller.ino中定义
int laserPressCount;
