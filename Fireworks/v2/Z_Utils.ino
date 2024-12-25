/************************************************** 
 *            各状态切换 & 核心函数实现
 **************************************************/
/// 进入空闲模式
void enterIdleMode() {
  currentState = STATE_IDLE;
  lcd.clear();
  lcd.print("IDLE MODE");
  Serial.println("IDLE MODE");
}

/// 进入自定义模式
void enterCustomizeMode() {
  currentState = STATE_CUSTOMIZE;
  lcd.clear();
  lcd.print("CUSTOMIZE MODE");
  Serial.println("CUSTOMIZE MODE");

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
  Serial.println("PREVIEW...");
}

/// 保存当前效果
void saveCurrentEffect() {
  // 构建消息字符串
  String msg = String(MSG_SAVE_EFFECT) + ","
             + String(currentEffect.color1.r) + ","
             + String(currentEffect.color1.g) + ","
             + String(currentEffect.color1.b) + ","
             + String(currentEffect.color2.r) + ","
             + String(currentEffect.color2.g) + ","
             + String(currentEffect.color2.b) + ","
             + String(currentEffect.maxBrightness) + ","
             + String(currentEffect.launchMode) + ","
             + String(currentEffect.explodeMode) + ","
             + String(currentEffect.gradientMode) + ","
             + String(currentEffect.launchMode) + ","
             + String(currentEffect.laserColor) + ","
             + String(currentEffect.mirrorAngle) + ","
             + String(currentEffect.explosionLEDCount) + ","
             + String(currentEffect.speedDelay);
  Serial.println(msg);
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
  Serial.println("Explode Firework!");

  ExplosionParams params = {
    effect.color1,
    effect.color2,
    effect.speedDelay,
    effect.explosionLEDCount,
    effect.mirrorAngle
  };

  // 根据 GradientMode 选择颜色渐变类型
  if (effect.gradientMode == GRAIDENT) {
    if (effect.explodeMode == NORMAL) {
      explosionGradientNormal(params);
    } else if (effect.explodeMode == RANDOM) {
      explosionGradientRandom(params);
    } else if (effect.explodeMode == BLINK) {
      explosionGradientBlink(params);
    }
  } else if (effect.gradientMode == FADE) {
    if (effect.explodeMode == NORMAL) {
      explosionFadeNormal(params);
    } else if (effect.explodeMode == RANDOM) {
      explosionFadeRandom(params);
    } else if (effect.explodeMode == BLINK) {
      explosionFadeBlink(params);
    }
  } else if (effect.gradientMode == SWITCH) {
    if (effect.explodeMode == NORMAL) {
      explosionSwitchNormal(params);
    } else if (effect.explodeMode == RANDOM) {
      explosionSwitchRandom(params);
    } else if (effect.explodeMode == BLINK) {
      explosionSwitchBlink(params);
    }
  }
}


/************************************************** 
 *          监听 numpad 点播功能(示例占位)
 **************************************************/

void checkNumpadInput() {
  char key = keypad.getKey(); // 获取按键输入
  if (key) { // 如果检测到按键
    Serial.print("Key pressed: ");
    Serial.println(key);

    // 检查是否是有效的数字键
    if (key >= '0' && key <= '9') {
      int keyEffect = key - '0'; // 将字符转为整数
      if (keyEffect >= minEffect && keyEffect <= maxEffect) {
        Serial.print("Playing effect: ");
        Serial.println(keyEffect);
      } else {
        Serial.println("Invalid effect number.");
      }
    } else {
      Serial.println("Non-numeric key pressed.");
    }
  }
}

/************************************************** 
 *              切换函数
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

void cycleGradientMode() {
  if (currentEffect.gradientMode == GRAIDENT) {
    currentEffect.gradientMode = FADE;
  } else if (currentEffect.gradientMode == FADE) {
    currentEffect.gradientMode = SWITCH;
  } else {
    currentEffect.gradientMode = GRAIDENT;
  }
}

void cycleExplodeMode() {
  if (currentEffect.explodeMode == NORMAL) {
    currentEffect.explodeMode = BLINK;
  } else if (currentEffect.explodeMode == BLINK) {
    currentEffect.explodeMode = RANDOM;
  } else {
    currentEffect.explodeMode = NORMAL;
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
  // 读取 Joystick 的 X 和 Y 值
  int xValue = analogRead(joyPinX) - 512; // 偏移中心值到 -512 ~ 512
  int yValue = analogRead(joyPinY) - 512; // 偏移中心值到 -512 ~ 512
  uint8_t lastHue = 0;           // 记录最后的颜色 Hue 值
  bool inDeadzone = true;        // Joystick 是否在死区

  // 检查是否在死区内
  bool currentlyInDeadzone = (abs(xValue) < JOY_DEADZONE && abs(yValue) < JOY_DEADZONE);

  if (!currentlyInDeadzone) {
    // Joystick 离开中心
    inDeadzone = false; // 标记为不在死区

    // 计算倾斜角度
    float angle = atan2(-yValue, xValue) * 180.0 / PI;
    if (angle < 0) {
      angle += 360; // 将角度范围调整到 0 ~ 360
    }

    // 将角度映射到 HSV 色环
    lastHue = (uint8_t)(angle / 360.0 * 255); // 更新最后的颜色
  }

  // 如果在死区，维持最后的颜色
  if (currentlyInDeadzone && !inDeadzone) {
    Serial.println("Joystick Released. Keeping last color.");
    inDeadzone = true; // 标记为在死区
  }

  // 返回基于当前 Hue 的颜色
  return CHSV(lastHue, 255, currentEffect.maxBrightness);
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

// 添加串口命令处理函数
void processSerialCommand() {
  if (Serial.available() > 0) {
    char cmdType = Serial.read();
    if (cmdType == MSG_PLAY_EFFECT) {
      // 读取效果参数
      FireworkEffect effect;
      effect.color1.r = Serial.parseInt();
      effect.color1.g = Serial.parseInt();
      
      effect.color1.b = Serial.parseInt();
      // ...读取其他参数...
      
      // 执行效果
      playEffect(effect);
    }
  }
}