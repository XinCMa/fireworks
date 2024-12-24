#include <FastLED.h>

// ========== 根据你的实际情况修改 ========== //
#define LED_PIN     6           // 输出到灯带的数据引脚
#define TOTAL_LED_COUNT    280     // 你的灯带总数
#define BLINK_INTERVAL 50       // 闪烁间隔（毫秒）
// =========================================== //

// 创建 LED 数组
CRGB leds[TOTAL_LED_COUNT];

// 上升效果参数
struct AscendParams {
  uint8_t speedDelay;   // 移动时的延时(毫秒)，数值越小移动越快
  int stripLen;         // 条带长度 (如 30)
  int volume;           // 移动距离 (如 50)
};

// 默认配置
AscendParams defaultConfig = {
  20,                  // speedDelay: 每步延时
  30,                  // stripLen: 条带长度
  50                   // volume: 移动50个LED的距离
};

// ===== 三种上升效果 =====

// 普通上升效果：平滑移动
void ascendNormal(const AscendParams &params) {
  int startPos = TOTAL_LED_COUNT - params.volume;
  int endPos = TOTAL_LED_COUNT - 1;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 在 [startPos, startPos+growLen-1] 范围内绘制白色条带
    for (int i = 0; i < growLen; i++) {
      int ledIndex = startPos + i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;
      leds[ledIndex] = CRGB::White;
    }
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos <= endPos - params.stripLen + 1; pos++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 绘制固定长度的白色条带
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos + i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;
      leds[ledIndex] = CRGB::White;
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// 随机闪烁上升效果：每个LED随机显示
void ascendRandom(const AscendParams &params) {
  int startPos = TOTAL_LED_COUNT - params.volume;
  int endPos = TOTAL_LED_COUNT - 1;

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos <= endPos - params.stripLen + 1; pos++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 绘制随机闪烁的白色条带
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos + i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      // 50% 概率决定是否点亮这个LED
      if (random(0, 100) < 50) {
        leds[ledIndex] = CRGB::White;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

// 定时闪烁上升效果：整体闪烁
void ascendBlink(const AscendParams &params) {
  int startPos = TOTAL_LED_COUNT - params.volume;
  int endPos = TOTAL_LED_COUNT - 1;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    // 使用 millis() 来控制闪烁
    bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
    
    if (shouldLight) {
      // 在 [startPos, startPos+growLen-1] 范围内绘制白色条带
      for (int i = 0; i < growLen; i++) {
        int ledIndex = startPos + i;
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;
        leds[ledIndex] = CRGB::White;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos <= endPos - params.stripLen + 1; pos++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
    
    bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
    
    if (shouldLight) {
      // 绘制固定长度的白色条带
      for (int i = 0; i < params.stripLen; i++) {
        int ledIndex = pos + i;
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;
        leds[ledIndex] = CRGB::White;
      }
    }
    FastLED.show();
    delay(params.speedDelay);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("==== Ascend Effects Demo ====");

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
  ascendNormal(defaultConfig);   delay(1000);
  ascendRandom(defaultConfig);   delay(1000);
  ascendBlink(defaultConfig);    delay(1000);
} 