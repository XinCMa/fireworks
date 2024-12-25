#include <FastLED.h>
#include <Servo.h>
#include <Stepper.h>
#include "A_GLOBAL.h"

// 全局对象
CRGB leds[TOTAL_LED_COUNT];
Servo laserServo;
FireworkEffect currentEffect;

// 先添加B_ASCEND_EFFECTS.ino和B_EXPLOSION_EFFECTS.ino中函数的声明
void normalAscendEffect(const AscendParams &params);
void pendulumAscend(const AscendParams &params);
void multipleShortBarsWithGapsAscend(const BarsWithGapParams &cfg);
void explosionGradientNormal(const ExplosionParams &params);
void explosionGradientRandom(const ExplosionParams &params);
void explosionGradientBlink(const ExplosionParams &params);
void explosionFadeNormal(const ExplosionParams &params);
void explosionFadeRandom(const ExplosionParams &params);
void explosionFadeBlink(const ExplosionParams &params);
void explosionSwitchNormal(const ExplosionParams &params);
void explosionSwitchRandom(const ExplosionParams &params);
void explosionSwitchBlink(const ExplosionParams &params);

// 函数声明需要包含参数类型
void launchFirework(const FireworkEffect &effect);
void explodeFirework(const FireworkEffect &effect);
void playEffect(const FireworkEffect &effect);
void SetMirrorAngle(int angle);

void setup() {
    Serial.begin(115200);
    pinMode(DIR_PIN, OUTPUT); // 设置方向引脚为输出
    pinMode(PUL_PIN, OUTPUT); // 设置脉冲引脚为输出
    pinMode(ENA_PIN, OUTPUT); // 设置使能引脚为输出

    digitalWrite(ENA_PIN, LOW);  // 启用驱动器
    digitalWrite(DIR_PIN, HIGH); // 默认方向为正转
    
    // 初始化LED
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, TOTAL_LED_COUNT);
    FastLED.setBrightness(255);
    FastLED.clear();
    FastLED.show();
    
    // 初始化舵机
    laserServo.attach(SERVO_PIN);
    laserServo.write(0);
}

void loop() {
    if (Serial.available() > 0) {
        char cmdType = Serial.read();
        if (cmdType == MSG_PLAY_EFFECT) {
            handlePlayEffect();
        }
    }
}

void handlePlayEffect() {
    // 从串口读取效果参数到 currentEffect
    currentEffect.color1.r = Serial.parseInt();
    currentEffect.color1.g = Serial.parseInt();
    currentEffect.color1.b = Serial.parseInt();
    
    currentEffect.color2.r = Serial.parseInt();
    currentEffect.color2.g = Serial.parseInt();
    currentEffect.color2.b = Serial.parseInt();
    
    currentEffect.maxBrightness = Serial.parseInt();
    currentEffect.launchMode = static_cast<LaunchMode>(Serial.parseInt());
    currentEffect.gradientMode = static_cast<GradientMode>(Serial.parseInt());
    currentEffect.explodeMode = static_cast<ExplodeMode>(Serial.parseInt());
    currentEffect.laserColor = static_cast<LaserColor>(Serial.parseInt());
    currentEffect.mirrorAngle = Serial.parseInt();
    currentEffect.explosionLEDCount = Serial.parseInt();
    currentEffect.speedDelay = Serial.parseInt();
    
    // 执行效果
    playEffect(currentEffect);
}

void playEffect(const FireworkEffect &effect) {
    SetMirrorAngle(effect.mirrorAngle);
    launchFirework(effect);
    explodeFirework(effect);
}

void SetMirrorAngle(int angle) {
    // 将角度转换为步数 (3200步对应180度)
    int targetStep = map(angle, 0, 180, 0, MAX_STEPS);
    
    // 确定运动方向
    if (targetStep > currentStep) {
        digitalWrite(DIR_PIN, HIGH);  // 正向运动
    } else {
        digitalWrite(DIR_PIN, LOW);   // 反向运动
    }

    // 启动运动并检查限位
    while (true) {
        // 检查是否到达目标位置
        if ((digitalRead(DIR_PIN) == HIGH && currentStep >= targetStep) ||
            (digitalRead(DIR_PIN) == LOW && currentStep <= targetStep)) {
            Serial.println("到达目标角度位置");
            break;
        }
        
        // 限位检查
        if (digitalRead(DIR_PIN) == HIGH && currentStep >= MAX_STEPS) {
            Serial.println("达到正向限位，停止电机！");
            break;
        }
        if (digitalRead(DIR_PIN) == LOW && currentStep <= MIN_STEPS) {
            Serial.println("达到反向限位，停止电机！");
            break;
        }

        // 发送脉冲信号
        digitalWrite(PUL_PIN, HIGH);
        delayMicroseconds(500);
        digitalWrite(PUL_PIN, LOW);
        delayMicroseconds(500);

        // 更新步数记录
        if (digitalRead(DIR_PIN) == HIGH) {
            currentStep++;
        } else {
            currentStep--;
        }

        // 检查停止命令
        if (Serial.available() > 0) {
            char stopInput = Serial.read();
            if (stopInput == 's') {
                Serial.println("用户输入停止命令");
                break;
            }
        }
    }

    // 输出当前位置信息
    Serial.print("当前步数: ");
    Serial.println(currentStep);
    Serial.print("当前角度: ");
    Serial.println(map(currentStep, 0, MAX_STEPS, 0, 180));
}

void launchFirework(const FireworkEffect &effect) {
    // 创建上升参数配置
    AscendParams ascendCfg = {
        effect.maxBrightness,
        effect.speedDelay,
        30  // stripLen
    };

    BarsWithGapParams barsCfg = {
        effect.maxBrightness,
        effect.speedDelay,
        5,  // segCount
        10, // segLen
        5   // gapLen
    };

    switch(effect.launchMode) {
        case NORMAL_ASCEND:
            normalAscendEffect(ascendCfg);
            break;
        case STEP_ASCEND:
            multipleShortBarsWithGapsAscend(barsCfg);
            break;
        case PENDULUM_ASCEND:
            pendulumAscend(ascendCfg);
            break;
    }
}

void explodeFirework(const FireworkEffect &effect) {
    ExplosionParams params = {
        effect.color1,
        effect.color2,
        effect.speedDelay,
        effect.explosionLEDCount,
        230  // moveRange
    };

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

// 清空串口缓冲区
void clearSerialBuffer() {
    while(Serial.available() > 0) {
        Serial.read();
    }
}
