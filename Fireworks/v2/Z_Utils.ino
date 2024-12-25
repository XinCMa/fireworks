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
  Serial.println("PREVIEW MODE");
}

void enterSaveMode() {
  currentState = STATE_SAVE;
  lcd.clear();
  lcd.print("SAVE MODE");
  Serial.println("SAVE MODE");
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
  // 创建上升参数配置
  AscendParams ascendCfg = {
    effect.maxBrightness,  // maxBrightness
    effect.speedDelay,     // speedDelay
    30                     // stripLen
  };

  // 创建多段上升参数配置
  BarsWithGapParams barsCfg = {
    effect.maxBrightness,  // maxBrightness
    effect.speedDelay,     // speedDelay
    5,                     // segCount
    10,                    // segLen
    5                      // gapLen
  };

  switch (effect.launchMode) {
    case NORMAL_ASCEND:
      Serial.println(">> Normal Ascend");
      normalAscendEffect(ascendCfg);
      break;

    case STEP_ASCEND:
      Serial.println(">> Step Ascend");
      multipleShortBarsWithGapsAscend(barsCfg);
      break;

    case PENDULUM_ASCEND:
      Serial.println(">> Pendulum Ascend");
      pendulumAscend(ascendCfg);
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
  char key = keypad.getKey();
  if (!key) return;  // 没有按键输入
  
  lcd.setCursor(0, 1);  // 使用LCD第二行显示输入
  
  switch (key) {
    case '*':  // 播放键
      if (inputIndex > 0) {
        // 将输入buffer转换为数字
        int effectNumber = atoi(inputBuffer);
        Serial.print("Request to play effect: ");
        Serial.println(effectNumber);
        
        // 发送播放请求到上位机
        String playCmd = String("R,") + String(effectNumber) + "\n";
        Serial.print(playCmd);  // R,表示请求播放
        
        // 清空输入
        clearInput();
        lcd.setCursor(0, 1);
        lcd.print("                ");  // 清空LCD第二行
      }
      break;
      
    case '#':  // 清除键
      clearInput();
      lcd.setCursor(0, 1);
      lcd.print("                ");  // 清空LCD第二行
      break;
      
    default:  // 数字键
      if (key >= '0' && key <= '9' && inputIndex < MAX_INPUT_DIGITS) {
        inputBuffer[inputIndex++] = key;
        inputBuffer[inputIndex] = '\0';  // 确保字符串正确终止
        
        // 显示当前输入
        lcd.setCursor(0, 1);
        lcd.print("Effect: ");
        lcd.print(inputBuffer);
      }
      break;
  }
}

// 清空输入缓冲区
void clearInput() {
  memset(inputBuffer, 0, sizeof(inputBuffer));
  inputIndex = 0;
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
  return map(val, 0, 1023, TOTAL_LED_COUNT-50, TOTAL_LED_COUNT);
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
      // 读取完整的效果参数
      FireworkEffect effect;
      
      // 1. 读取color1
      effect.color1.r = Serial.parseInt();
      effect.color1.g = Serial.parseInt();
      effect.color1.b = Serial.parseInt();
      
      // 2. 读取color2
      effect.color2.r = Serial.parseInt();
      effect.color2.g = Serial.parseInt();
      effect.color2.b = Serial.parseInt();
      
      // 3. 读取其他参数
      effect.maxBrightness = Serial.parseInt();
      effect.launchMode = static_cast<LaunchMode>(Serial.parseInt());
      effect.gradientMode = static_cast<GradientMode>(Serial.parseInt());
      effect.explodeMode = static_cast<ExplodeMode>(Serial.parseInt());
      effect.laserColor = static_cast<LaserColor>(Serial.parseInt());
      effect.mirrorAngle = Serial.parseInt();
      effect.explosionLEDCount = Serial.parseInt();
      effect.speedDelay = Serial.parseInt();

      // 打印调试信息
      Serial.println("Received effect parameters:");
      Serial.print("Launch Mode: ");
      Serial.println(modeNames[effect.launchMode]);
      Serial.print("Explosion LED Count: ");
      Serial.println(effect.explosionLEDCount);
      Serial.print("Speed Delay: ");
      Serial.println(effect.speedDelay);
      
      // 执行效果
      playEffect(effect);
    }
    
    // 清空剩余的串口缓冲区
    while(Serial.available() > 0) {
      Serial.read();
    }
  }
}