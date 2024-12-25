#include <FastLED.h>
#include <Servo.h>
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

void setup() {
    Serial.begin(115200);
    
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
    launchFirework(effect);
    explodeFirework(effect);
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

// 注意：需要包含 B_ASCEND_EFFECTS.ino 和 B_EXPLOSION_EFFECTS.ino 中的效果函数
