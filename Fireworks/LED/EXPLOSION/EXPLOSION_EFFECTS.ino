#include <FastLED.h>

// ========== 根据你的实际情况修改 ========== //
#define LED_PIN     6           // 输出到灯带的数据引脚
#define TOTAL_LED_COUNT    280     // 你的灯带总数
#define BLINK_INTERVAL 50       // 闪烁间隔（毫秒）
// =========================================== //

// 创建 LED 数组
CRGB leds[TOTAL_LED_COUNT];

// 爆炸效果参数
struct ExplosionParams {
  CRGB color1;          // 起始颜色 (渐变的第一端)
  CRGB color2;          // 结束颜色 (渐变的第二端)
  uint8_t speedDelay;   // 条带移动时的延时(毫秒)，数值越小移动越快
  int stripLen;         // 条带长度 (如 30)
  int moveRange;        // 条带移动范围 (如 80)
};

// 默认配置
ExplosionParams defaultConfig = {
  CRGB::Red,           // color1: 渐变起始颜色 (红)
  CRGB::Yellow,        // color2: 渐变结束颜色 (黄)
  20,                  // speedDelay: 每步延时
  30,                  // stripLen: 条带长度
  230                  // moveRange: 条带移动范围
};

// ===== 颜色效果1: 条带内部渐变 =====

// 普通移动 + 内部渐变
void explosionGradientNormal(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 在 [pos-growLen+1, pos] 范围内绘制渐变条带
    for (int i = 0; i < growLen; i++) {
      int ledIndex = startPos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      // 计算渐变颜色：从 color1 → color2
      float ratio = float(i) / float(params.stripLen - 1);
      CRGB gradColor = blend(params.color1, params.color2, uint8_t(ratio * 255));

      leds[ledIndex] = gradColor;
    }
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos >= endPos; pos--) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 绘制固定长度的渐变条带
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      float ratio = float(i) / float(params.stripLen - 1);
      CRGB gradColor = blend(params.color1, params.color2, uint8_t(ratio * 255));

      leds[ledIndex] = gradColor;
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// 随机闪烁 + 内部渐变
void explosionGradientRandom(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos >= endPos; pos--) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 绘制随机闪烁的渐变条带
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      // 50% 概率决定是否点亮这个LED
      if (random(0, 100) < 50) {
        float ratio = float(i) / float(params.stripLen - 1);
        CRGB gradColor = blend(params.color1, params.color2, uint8_t(ratio * 255));
        leds[ledIndex] = gradColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// 定时闪烁 + 内部渐变
void explosionGradientBlink(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 使用 millis() 来控制闪烁
    bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
    
    if (shouldLight) {
      // 在 [pos-growLen+1, pos] 范围内绘制渐变条带
      for (int i = 0; i < growLen; i++) {
        int ledIndex = startPos - i;
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

        float ratio = float(i) / float(params.stripLen - 1);
        CRGB gradColor = blend(params.color1, params.color2, uint8_t(ratio * 255));

        leds[ledIndex] = gradColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos >= endPos; pos--) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
    
    if (shouldLight) {
      // 绘制固定长度的渐变条带
      for (int i = 0; i < params.stripLen; i++) {
        int ledIndex = pos - i;
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

        float ratio = float(i) / float(params.stripLen - 1);
        CRGB gradColor = blend(params.color1, params.color2, uint8_t(ratio * 255));

        leds[ledIndex] = gradColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// ===== 颜色效果2: 整体渐变 =====

// 普通移动 + 整体渐变
void explosionFadeNormal(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    float colorProgress = 0.0;
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 计算当前的基础颜色
    CRGB baseColor = blend(params.color1, params.color2, uint8_t(colorProgress * 255));
    
    // 在 [pos-growLen+1, pos] 范围内绘制亮度渐变
    for (int i = 0; i < growLen; i++) {
      int ledIndex = startPos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      // 在基础颜色上添加亮度渐变效果
      float brightnessRatio = 1.0 - (float(i) / float(params.stripLen - 1));
      CRGB pixelColor = baseColor;
      pixelColor.nscale8(uint8_t(brightnessRatio * 255));

      leds[ledIndex] = pixelColor;
    }
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos，同时颜色从 color1 渐变到 color2
  for (int pos = startPos; pos >= endPos; pos--) {
    float colorProgress = float(startPos - pos) / float(startPos - endPos);
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 计算当前的基础颜色
    CRGB baseColor = blend(params.color1, params.color2, uint8_t(colorProgress * 255));
    
    // 在 [pos-stripLen+1, pos] 范围内绘制亮度渐变
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      // 在基础颜色上添加亮度渐变效果
      float brightnessRatio = 1.0 - (float(i) / float(params.stripLen - 1));
      CRGB pixelColor = baseColor;
      pixelColor.nscale8(uint8_t(brightnessRatio * 255));

      leds[ledIndex] = pixelColor;
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// 随机闪烁 + 整体渐变
void explosionFadeRandom(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;

  // 条带长度固定后从 startPos 移动到 endPos，同时颜色从 color1 渐变到 color2
  for (int pos = startPos; pos >= endPos; pos--) {
    float colorProgress = float(startPos - pos) / float(startPos - endPos);
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

    // 计算当前的基础颜色
    CRGB baseColor = blend(params.color1, params.color2, uint8_t(colorProgress * 255));
    
    // 在 [pos-stripLen+1, pos] 范围内绘制亮度渐变
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      // 50% 概率决定是否点亮这个LED
      if (random(0, 100) < 50) {
        // 在基础颜色上添加亮度渐变效果
        float brightnessRatio = 1.0 - (float(i) / float(params.stripLen - 1));
        CRGB pixelColor = baseColor;
        pixelColor.nscale8(uint8_t(brightnessRatio * 255));
        
        leds[ledIndex] = pixelColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// 定时闪烁 + 整体渐变
void explosionFadeBlink(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    float colorProgress = 0.0;
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

    // 使用 millis() 来控制闪烁
    bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
    
    if (shouldLight) {
      // 计算当前的基础颜色
      CRGB baseColor = blend(params.color1, params.color2, uint8_t(colorProgress * 255));
      
      // 在 [pos-growLen+1, pos] 范围内绘制亮度渐变
      for (int i = 0; i < growLen; i++) {
        int ledIndex = startPos - i;
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

        // 在基础颜色上添加亮度渐变效果
        float brightnessRatio = 1.0 - (float(i) / float(params.stripLen - 1));
        CRGB pixelColor = baseColor;
        pixelColor.nscale8(uint8_t(brightnessRatio * 255));

        leds[ledIndex] = pixelColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos，同时颜色从 color1 渐变到 color2
  for (int pos = startPos; pos >= endPos; pos--) {
    float colorProgress = float(startPos - pos) / float(startPos - endPos);
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

    bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
    
    if (shouldLight) {
      // 计算当前的基础颜色
      CRGB baseColor = blend(params.color1, params.color2, uint8_t(colorProgress * 255));
      
      // 在 [pos-stripLen+1, pos] 范围内绘制亮度渐变
      for (int i = 0; i < params.stripLen; i++) {
        int ledIndex = pos - i;
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

        // 在基础颜色上添加亮度渐变效果
        float brightnessRatio = 1.0 - (float(i) / float(params.stripLen - 1));
        CRGB pixelColor = baseColor;
        pixelColor.nscale8(uint8_t(brightnessRatio * 255));

        leds[ledIndex] = pixelColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// ===== 颜色效果3: 颜色切换 =====

// 普通移动 + 颜色切换
void explosionSwitchNormal(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 使用 millis() 来决定当前显示哪种颜色
    // 每500毫秒(0.5秒)切换一次颜色
    CRGB currentColor = ((millis() / 500) % 2 == 0) ? params.color1 : params.color2;
    
    // 在 [pos-growLen+1, pos] 范围内绘制单色条带
    for (int i = 0; i < growLen; i++) {
      int ledIndex = startPos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;
      
      leds[ledIndex] = currentColor;
    }
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos >= endPos; pos--) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 使用 millis() 来决定当前显示哪种颜色
    CRGB currentColor = ((millis() / 500) % 2 == 0) ? params.color1 : params.color2;
    
    // 在 [pos-stripLen+1, pos] 范围内绘制单色条带
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;
      
      leds[ledIndex] = currentColor;
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// 随机闪烁 + 颜色切换
void explosionSwitchRandom(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos >= endPos; pos--) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 使用 millis() 来决定当前显示哪种颜色
    CRGB currentColor = ((millis() / 500) % 2 == 0) ? params.color1 : params.color2;
    
    // 在 [pos-stripLen+1, pos] 范围内绘制随机闪烁的单色条带
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      // 50% 概率决定是否点亮这个LED
      if (random(0, 100) < 50) {
        leds[ledIndex] = currentColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// 定时闪烁 + 颜色切换
void explosionSwitchBlink(const ExplosionParams &params) {
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

    // 使用 millis() 来控制闪烁和颜色切换
    bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;  // 闪烁控制
    CRGB currentColor = ((millis() / 500) % 2 == 0) ? params.color1 : params.color2;  // 颜色切换控制
    
    if (shouldLight) {
      // 在 [pos-growLen+1, pos] 范围内绘制单色条带
      for (int i = 0; i < growLen; i++) {
        int ledIndex = startPos - i;
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;
        
        leds[ledIndex] = currentColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos >= endPos; pos--) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

    bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
    CRGB currentColor = ((millis() / 500) % 2 == 0) ? params.color1 : params.color2;
    
    if (shouldLight) {
      // 在 [pos-stripLen+1, pos] 范围内绘制单色条带
      for (int i = 0; i < params.stripLen; i++) {
        int ledIndex = pos - i;
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;
        
        leds[ledIndex] = currentColor;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("==== Explosion Effects Demo ====");

  // 初始化 FastLED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, TOTAL_LED_COUNT);
  FastLED.setBrightness(255);
  
  // 清空灯带
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
  FastLED.show();

  // 初始化随机种子
  randomSeed(analogRead(0));
}

void loop() {
  // 演示所有效果
  explosionGradientNormal(defaultConfig);  delay(1000);
  explosionGradientRandom(defaultConfig);  delay(1000);
  explosionGradientBlink(defaultConfig);   delay(1000);
  explosionFadeNormal(defaultConfig);      delay(1000);
  explosionFadeRandom(defaultConfig);      delay(1000);
  explosionFadeBlink(defaultConfig);       delay(1000);
  explosionSwitchNormal(defaultConfig);    delay(1000);
  explosionSwitchRandom(defaultConfig);    delay(1000);
  explosionSwitchBlink(defaultConfig);     delay(1000);
} 