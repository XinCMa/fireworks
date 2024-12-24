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

ExplosionParams explosionCfg = {
  CRGB::Red,           // color1: 渐变起始颜色 (红)
  CRGB::Yellow,        // color2: 渐变结束颜色 (黄)
  20,                  // speedDelay: 每步延时
  30,                  // stripLen: 条带长度
  230                  // moveRange: 条带移动范围
};

// 函数声明
void explosionEffect(const ExplosionParams &params);
void drawGrowingGradientBar(int pos, const ExplosionParams &params, int growLen, float colorProgress);

void setup() {
  Serial.begin(115200);
  Serial.println("==== Explosion Effect: Blinking Color-Changing Bar ====");

  // 初始化 FastLED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, TOTAL_LED_COUNT);
  FastLED.setBrightness(255); // 设置全局亮度上限

  // 清空灯带
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
  FastLED.show();
}

void loop() {
  // 执行爆炸效果
  explosionEffect(explosionCfg);

  // 动画结束后清空
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
  FastLED.show();
  delay(1000); // 暂停1秒后再重复
}

/**
 * @brief 执行一次爆炸效果
 *        条带颜色从 color1 → color2 渐变，长度 stripLen
 *        移动范围限制为 moveRange，没有回退。
 *        条带从第一个灯珠开始逐渐变长。
 */
void explosionEffect(const ExplosionParams &params) {
  // 限制条带移动范围
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;
  
  // 条带逐步延长的逻辑
  for (int growLen = 1; growLen <= params.stripLen; growLen++) {
    float colorProgress = 0.0;
    drawGrowingGradientBar(startPos, params, growLen, colorProgress);
    FastLED.show();
    delay(params.speedDelay);
  }

  // 条带长度固定后从 startPos 移动到 endPos，同���颜色从 color1 渐变到 color2
  for (int pos = startPos; pos >= endPos; pos--) {
    float colorProgress = float(startPos - pos) / float(startPos - endPos);
    drawGrowingGradientBar(pos, params, params.stripLen, colorProgress);
    FastLED.show();
    delay(params.speedDelay);
  }
}

/**
 * @brief 绘制一个逐渐变长的条带，带闪烁效果
 *        整个条带的基础色会随 colorProgress 从 color1 渐变到 color2
 * @param colorProgress 0.0 -> 1.0，控制整体颜色从 color1 到 color2 的渐变进度
 */
void drawGrowingGradientBar(int pos, const ExplosionParams &params, int growLen, float colorProgress) {
  // 清空灯带
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

  // 使用 millis() 来控制闪烁
  bool shouldLight = (millis() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
  
  if (shouldLight) {
    // 计算当前的基础颜色
    CRGB baseColor = blend(params.color1, params.color2, uint8_t(colorProgress * 255));
    
    // 在 [pos-growLen+1, pos] 范围内绘制亮度渐变
    for (int i = 0; i < growLen; i++) {
      int ledIndex = pos - i;
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

      // 在基础颜色上添加亮度渐变效果
      float brightnessRatio = 1.0 - (float(i) / float(params.stripLen - 1));
      CRGB pixelColor = baseColor;
      pixelColor.nscale8(uint8_t(brightnessRatio * 255));

      leds[ledIndex] = pixelColor;
    }
  }
} 