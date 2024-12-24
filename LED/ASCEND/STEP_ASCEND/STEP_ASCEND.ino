#include <FastLED.h>

// ========== 根据你的实际情况修改 ========== //
#define LED_PIN     6           // 输出到灯带的数据引脚
#define NUM_LEDS    200         // 你的灯带总数
// =========================================== //

// 创建 LED 数组
CRGB leds[NUM_LEDS];

// 相关参数结构
struct BarsWithGapParams {
  uint8_t  maxBrightness;  // 条带内头部最亮的亮度
  uint8_t  speedDelay;     // 每次移动一步后的延时 (毫秒)
  int      segCount;       // 短条带的数量 (5)
  int      segLen;         // 每个短条带的长度 (10)
  int      gapLen;         // 相邻条带之间的黑灯数 (5)
};

BarsWithGapParams barsCfg = {
  255,  // maxBrightness
  30,   // speedDelay (可调小加快动画，可调大放慢动画)
  5,    // segCount
  10,   // segLen
  5     // gapLen
};

// 函数声明
void multipleShortBarsWithGapsAscend(const BarsWithGapParams &cfg);

void setup() {
  Serial.begin(115200);
  Serial.println("==== 5 Short Bars with 5-Leds Gap Ascend Demo ====");

  // 初始化 LED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  // 全局亮度 (0~255)
  FastLED.setBrightness(255);

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void loop() {
  // 播放一次 "5个短条带 + 条带间空5颗" 效果
  multipleShortBarsWithGapsAscend(barsCfg);

  // 播放完成后清空灯带，等待1秒
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(1000);
}

/**
 * @brief 在灯带底部到顶部之间，移动一个包含 "5 个短条带 + 4 个间隔" 的整体
 *        1. 每个短条带 segLen=10，相邻间隔 gapLen=5
 *        2. 条带内部做 "尾暗头亮" 的线性渐变
 *        3. 间隔处始终保持熄灭 (黑色)
 *        4. 最终占用 totalSegLen = segCount*segLen + (segCount-1)*gapLen
 */
void multipleShortBarsWithGapsAscend(const BarsWithGapParams &cfg) {
  // 1) 计算 5 条带 + 4 间隔 共占用多少颗 LED
  int totalSegLen = cfg.segCount * cfg.segLen + (cfg.segCount - 1) * cfg.gapLen; 
  // 对 5个条带 + 4处间隔 = 5*10 + 4*5 = 70

  // 2) 计算移动的最顶位置
  int topLimit = NUM_LEDS - totalSegLen; // 若=200, 则topLimit=130

  // 3) 从 pos=0 移动到 pos=topLimit
  for (int pos = 0; pos <= topLimit; pos++) {
    // 清空灯带
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    // 4) 绘制 5 个短条带 (每个段内部做尾暗头亮)
    for (int segIndex = 0; segIndex < cfg.segCount; segIndex++) {
      // 计算此条带在灯带上的起始索引
      // 第0段 = pos + 0*(segLen + gapLen)
      // 第1段 = pos + 1*(segLen + gapLen)
      // ...
      int barStart = pos + segIndex * (cfg.segLen + cfg.gapLen);
      int barEnd   = barStart + cfg.segLen - 1;

      // 条带内部渐变：i=0 → 尾部(亮度最低), i=segLen-1 → 头部(亮度最高)
      for (int i = 0; i < cfg.segLen; i++) {
        int ledIndex = barStart + i;
        if (ledIndex < 0 || ledIndex >= NUM_LEDS) continue;

        // 线性渐变: ratio= i/(segLen-1)
        float ratio = float(i) / float(cfg.segLen - 1);
        uint8_t pixelBrightness = uint8_t(cfg.maxBrightness * ratio);

        leds[ledIndex] = CRGB::White; // 你可改成别的颜色
        leds[ledIndex].nscale8(pixelBrightness);
      }

      // 间隔部分自动保持黑色，因为上面 fill_solid 清空了，且不去填这几颗
    }

    // 显示
    FastLED.show();
    // 速度控制
    delay(cfg.speedDelay);
  }
}
