#include "A_GLOBAL.h"

const char* modeNames[] = {
    "NORMAL",
    "STEP",
    "PENDULUM"
};


bool lastAscendButtonState = HIGH;
bool lastLaserButtonState = HIGH;
bool lastPreviewButtonState = HIGH;
bool lastSaveButtonState = LOW;
bool lastCustomizeButtonState = LOW;

CRGB leds[TOTAL_LED_COUNT];
Servo laserServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

SystemState currentState = STATE_IDLE;
FireworkEffect currentEffect;
int savedCount = 0;
int laserPressCount = 0;
