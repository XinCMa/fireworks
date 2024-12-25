#include "A_GLOBAL.h"

const char* modeNames[] = {
    "NORMAL",
    "STEP",
    "PENDULUM"
};


bool defaultAscendButtonState = HIGH;
bool defaultLaserButtonState = HIGH;
bool defaultPreviewButtonState = HIGH;
bool defaultSaveButtonState = LOW;
bool defaultCustomizeButtonState = LOW;
bool defaultExplodeButtonState = HIGH;
bool defaultGradientButtonState = HIGH;

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



CRGB leds[TOTAL_LED_COUNT];
Servo laserServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

SystemState currentState = STATE_IDLE;
FireworkEffect currentEffect;
int laserPressCount;
