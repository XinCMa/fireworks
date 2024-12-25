#include "A_GLOBAL.h"

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
void cycleGradientMode();
void cycleExpldeMode();

// 读取交互输入的占位函数
CRGB getColorFromJoystick(int joyPinX, int joyPinY); // 用您自己的方式实现
uint8_t getBrightnessFromSlider();
int getMirrorAngleFromSlider();
int getExplosionLEDCountFromSlider();
int getSpeedFromSlider();

// 具体发射逻辑的子函数 (上升 & 爆炸)
void launchFirework(const FireworkEffect &effect);
void explodeFirework(const FireworkEffect &effect);

// 监听 numpad 点播功能（示例占位）
void checkNumpadInput();

// 添加串口命令处理函数
void processSerialCommand();

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
  pinMode(CUSTOMIZE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PREVIEW_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SAVE_BUTTON_PIN, INPUT_PULLUP);

  pinMode(ASCEND_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LASER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(EXPLODE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(GRADIENT_BUTTON_PIN, INPUT_PULLUP);

  // 初始化 LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("System Idle");
  Serial.println("System Idle");

  // 进入空闲模式
  enterIdleMode();
}

/************************************************** 
 *                   loop()
 **************************************************/

void loop() {
  // 处理串口命令
  processSerialCommand();
          
  switch (currentState) {
    case STATE_IDLE:{
      Serial.println("Entererd State of IDLE IN LOOP");
      if (digitalRead(CUSTOMIZE_BUTTON_PIN) == HIGH) {
        enterCustomizeMode();
      }

      // 检查numpad输入（如果有的话）
      checkNumpadInput();
      break;
    }


    case STATE_CUSTOMIZE:{
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

      // 读取当前按钮状态
      bool currentExplodeState = digitalRead(EXPLODE_BUTTON_PIN);
      bool currentGradientState = digitalRead(GRADIENT_BUTTON_PIN);
      bool currentAscendState = digitalRead(ASCEND_BUTTON_PIN);
      bool currentLaserState = digitalRead(LASER_BUTTON_PIN);
      bool currentPreviewState = digitalRead(PREVIEW_BUTTON_PIN);
      bool currentSaveState = digitalRead(SAVE_BUTTON_PIN);


      if (currentExplodeState != defaultExplodeButtonState) {
        cycleExplodeMode();
        lcd.clear();
        lcd.print("Explode: ");
        lcd.print(currentEffect.explodeMode);
        Serial.print("Explode: ");
        Serial.println(currentEffect.explodeMode);
        delay(500);
      } 

      if (currentGradientState != defaultGradientButtonState) {
        cycleGradientMode();
        lcd.clear();
        lcd.print("Gradient: ");
        lcd.print(currentEffect.gradientMode);
        Serial.print("Gradient: ");
        Serial.println(currentEffect.gradientMode);
        delay(500);
      }

      // 检测上升模式按钮状态变化
      if (currentAscendState != defaultAscendButtonState) {
        cycleLaunchMode();
        lcd.clear();
        lcd.print("Mode: ");
        lcd.print(modeNames[currentEffect.launchMode]);
        Serial.print("Mode: ");
        Serial.println(modeNames[currentEffect.launchMode]);
        delay(500);
      }

      // 检测激光按钮状态变化
      if (currentLaserState != defaultLaserButtonState) {
        cycleLaserColor();
        lcd.clear();
        if (currentEffect.laserColor == LASER_GREEN) {
          lcd.print("Laser: Green");
          Serial.println("Laser: Green");
        } else if (currentEffect.laserColor == LASER_RED) {
          lcd.print("Laser: Red");
          Serial.println("Laser: Red");
        } else {
          lcd.print("Laser: None");
          Serial.println("Laser: None");
        }
        delay(500);
      }

      // 检测预览按钮状态变化
      if (currentPreviewState != defaultPreviewButtonState) {
        Serial.println("Entering Preview State");
        enterPreviewMode();
      }

      // 检测保存按钮状态变化
      if (currentSaveState != defaultSaveButtonState) {  // 修改这里，使用正确的变量名
        currentState = STATE_SAVE;
        Serial.println("Entering Save State");
      }

      // 读取滑杆 - 这些不需要状态检测，直接读取当前值
      currentEffect.maxBrightness = getBrightnessFromSlider();
      currentEffect.mirrorAngle = getMirrorAngleFromSlider();
      currentEffect.explosionLEDCount = getExplosionLEDCountFromSlider();
      currentEffect.speedDelay = getSpeedFromSlider();

      // 此处还可添加“(3)条带的颜色（渐变）效果”与“(4)条带的闪烁方式”的输入逻辑，留待后续完善
      delay(50);

      break;
    }

    case STATE_PREVIEW:{
      // 执行一次发射，但不保存
      Serial.println("Entererd State of Preview in LOOP");
      playEffect(currentEffect);

      enterCustomizeMode();
      break;
    }

    case STATE_SAVE:{
      Serial.println("Entererd State of Save in LOOP");
      saveCurrentEffect();

      enterIdleMode();
      break;
    }

    case STATE_PLAY_SAVED:{
      Serial.println("Entererd State of Preview in LOOP");
      // 如果有 numpad 输入，选定播放
      // 放到一个函数里去做即可
      break;
    }

    default:{
      Serial.println("No such state");
      break;
    }
  }
}







