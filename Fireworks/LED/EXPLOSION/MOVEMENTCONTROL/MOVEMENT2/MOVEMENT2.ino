#include <FastLED.h>

// ========== 根据你的实际情况修改 ========== //
#define LED_PIN     6           // 输出到灯带的数据引脚
#define TOTAL_LED_COUNT    280     // 你的灯带总数
// =========================================== //

// 创建 LED 数组
CRGB leds[TOTAL_LED_COUNT];

// 爆炸效果参数
struct ExplosionParams {
  uint8_t speedDelay;   // 条带移动时的延时(毫秒)，数值越小移动越快
  int stripLen;         // 条带长度 (如 30)
  int moveRange;        // 条带移动范围 (如 80)
};

ExplosionParams explosionCfg = {
  20,                  // speedDelay: 每步延时
  30,                  // stripLen: 条带长度
  230                  // moveRange: 条带移动范围 (改为230)
};

// 函数声明
void explosionEffect(const ExplosionParams &params);
void drawFlickeringWhiteBar(int pos, const ExplosionParams &params);

void setup() {
  Serial.begin(115200);
  Serial.println("==== Explosion Effect: Flickering White Bar ====");

  // 初始化 FastLED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, TOTAL_LED_COUNT);
  FastLED.setBrightness(255); // 设置全局亮度上限

  // 清空灯带
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
  FastLED.show();

  // 初始化随机种子
  randomSeed(analogRead(0)); // 使用未接引脚作为随机种子
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
 *        条带为随机白色闪烁，长度 stripLen
 *        移动范围限制为 moveRange，没有回退。
 */
void explosionEffect(const ExplosionParams &params) {
  // 限制条带移动范围
  int startPos = TOTAL_LED_COUNT - 1;
  int endPos = params.moveRange;

  // 条带长度固定后从 startPos 移动到 endPos
  for (int pos = startPos; pos >= endPos; pos--) {
    drawFlickeringWhiteBar(pos, params);
    FastLED.show();
    delay(params.speedDelay);
  }
}

/**
 * @brief 绘制一个随机白色闪烁条带
 *        条带为纯白色，随机闪烁，长度为 stripLen
 */
void drawFlickeringWhiteBar(int pos, const ExplosionParams &params) {
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

  for (int i = 0; i < params.stripLen; i++) {
    int ledIndex = pos - i;
    if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

    if (random(0, 100) < 50) {
      leds[ledIndex] = CRGB::White;
    } else {
      leds[ledIndex] = CRGB::Black;
    }
  }
}
