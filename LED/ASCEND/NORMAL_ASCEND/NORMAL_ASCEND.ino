#include <FastLED.h>

// 根据你的实际情况修改
#define LED_PIN     6       // 灯带数据输出引脚
#define NUM_LEDS    280     // 你的灯带总颗数

CRGB leds[NUM_LEDS];

// 用来控制“普通上升”且“头在大索引侧、尾在小索引侧”的核心参数
struct AscendParams {
  uint8_t maxBrightness;  // 头部(大索引)的最大亮度
  uint8_t speedDelay;     // 条带移动时的延时(毫秒)，数值越小移动越快
  int stripLen;           // 条带长度
};

// 示例
AscendParams normalAscendCfg = {
  255,  // maxBrightness (头最亮)
  20,   // speedDelay (可以调大或调小)
  30    // stripLen
};

void setup() {
  Serial.begin(115200);
  Serial.println("==== Normal Ascend: Head on Larger Index ====");

  // 初始化 FastLED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  // 全局亮度可先设为 255
  FastLED.setBrightness(255);

  // 清空灯带
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void loop() {
  // 演示一次“从小索引到大索引移动，头在高索引侧”的效果
  normalAscendEffect(normalAscendCfg);

  // 动画完成后，清空并等待1秒，再循环一遍
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(1000);
}

/**
 * @brief 普通上升：条带从下标0~(NUM_LEDS - stripLen)移动，
 *        条带内部i=0处亮度最低，i=stripLen-1处亮度最高
 *        整个条带移动时，“头”在大索引那一端，“尾”在小索引那一端。
 */
void normalAscendEffect(const AscendParams &params) {
  int topLimit = NUM_LEDS - params.stripLen; 
  // pos 递增表示从小编号往大编号移动

  for (int pos = 0; pos <= topLimit; pos++) {
    // 清空灯带
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    // 在 [pos, pos+stripLen-1] 这段绘制"尾暗头亮"
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos + i;
      // 边界检查
      if (ledIndex < 0 || ledIndex >= NUM_LEDS) continue;

      // 线性渐变: i=0 → ratio=0 → 最暗, i=stripLen-1 → ratio=1 → 最亮
      float ratio = float(i) / float(params.stripLen - 1);
      uint8_t pixelBrightness = uint8_t(params.maxBrightness * ratio);

      // 固定白色，可根据需要改成其他颜色
      leds[ledIndex] = CRGB::White;
      // 再按 pixelBrightness 进行缩放
      leds[ledIndex].nscale8(pixelBrightness);
    }

    // 显示
    FastLED.show();
    // 移动速度
    delay(params.speedDelay);
  }
}
