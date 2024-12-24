#include <FastLED.h>

// ========== 根据你的实际情况修改 ========== //
#define LED_PIN     6           // 输出到灯带的数据引脚
#define TOTAL_LED_COUNT    280     // 你的灯带总数
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
void drawFlickeringBar(int pos, const ExplosionParams &params);

void setup() {
  Serial.begin(115200);
  Serial.println("==== Explosion Effect: Random Flickering Color-Switching Bar ====");

  // 初始化 FastLED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, TOTAL_LED_COUNT);
  FastLED.setBrightness(255); // 设置全局亮度上限

  // 清空灯带
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
  FastLED.show();

  // 初始化随机种子
  randomSeed(analogRead(0));
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
 *        条带颜色在 color1 和 color2 之间切换，长度 stripLen
 *        移动范围限制为 moveRange，没有回退。
 */
void explosionEffect(const ExplosionParams &params) {
  // 限制条带移动范围
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos >= endPos; pos--) {
    drawFlickeringBar(pos, params);
    FastLED.show();
    delay(params.speedDelay);
  }
}

/**
 * @brief 绘制一个随机闪烁的条带
 *        条带颜色在 color1 和 color2 之间切换
 *        每个LED有50%的概率显示或不显示
 */
void drawFlickeringBar(int pos, const ExplosionParams &params) {
  // 清空灯带
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

  // 使用 millis() 来决定当前显示哪种颜色
  // 每500毫秒(0.5秒)切换一次颜色
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
} 