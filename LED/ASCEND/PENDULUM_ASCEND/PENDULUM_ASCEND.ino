#include <FastLED.h>

// ========== 根据你的实际情况修改 ========== //
#define LED_PIN     6           // 输出到灯带的数据引脚
#define NUM_LEDS    280         // 你的灯带总数
// =========================================== //

// 创建 LED 数组
CRGB leds[NUM_LEDS];

// 用来控制“普通上升”的核心参数
struct AscendParams {
  uint8_t maxBrightness;  // 头部(大索引)的最大亮度
  uint8_t speedDelay;     // 条带移动时的延时(毫秒)，数值越小移动越快
  int     stripLen;       // 条带长度 (30)
};

AscendParams normalAscendCfg = {
  255, // maxBrightness (头最亮)
  20,  // speedDelay
  30   // stripLen
};

// 函数声明
void movePos(int startPos, int endPos, int step, const AscendParams &params);
void drawAscendFrame(int pos, const AscendParams &params);

void setup() {
  Serial.begin(115200);
  Serial.println("==== Move +50, -10 repeatedly until top ====");

  // 初始化 FastLED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  // 全局亮度可设 0~255
  FastLED.setBrightness(255);

  // 清空灯带
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void loop() {
  // 一次演示：条带从0开始，每次 +50, -10，直到最顶
  int topLimit = NUM_LEDS - normalAscendCfg.stripLen; // 280-30=250
  int pos = 0;

  while(pos < topLimit) {
    // (1) 往前走 50
    int forwardTarget = pos + 50;
    // 如果超出顶端，就只走到顶
    if (forwardTarget > topLimit) {
      forwardTarget = topLimit;
    }
    // 移动
    movePos(pos, forwardTarget, +1, normalAscendCfg);
    pos = forwardTarget;

    // 如果已经到顶了，退出循环
    if (pos >= topLimit) {
      break;
    }

    // (2) 往回走 10
    int backwardTarget = pos - 10;
    // 正常来说不会小于0，但如果要保护，可加 max
    if (backwardTarget < 0) {
      backwardTarget = 0;
    }
    movePos(pos, backwardTarget, -1, normalAscendCfg);
    pos = backwardTarget;
  }

  // 到达顶端后清空
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(2000);

  // 如果你想反复来回，可在此处再把 pos=0 重置，或者写其他逻辑，让它不断循环
}

/**
 * @brief 从 startPos 移动到 endPos，每步 step=+1(上升) 或 step=-1(下降)，
 *        每一步都绘制条带并延时
 */
void movePos(int startPos, int endPos, int step, const AscendParams &params) {
  if (step > 0) {
    // 向上移动
    for (int p = startPos; p <= endPos; p++) {
      drawAscendFrame(p, params);
      FastLED.show();
      delay(params.speedDelay);
    }
  } else {
    // 向下移动
    for (int p = startPos; p >= endPos; p--) {
      drawAscendFrame(p, params);
      FastLED.show();
      delay(params.speedDelay);
    }
  }
}

/**
 * @brief 将灯带清空，然后在 [pos, pos+stripLen-1] 画一个“尾部暗、头部亮”的条带
 *        i=0 → ratio=0 → 最暗, i=stripLen-1 → ratio=1 → 最亮
 */
void drawAscendFrame(int pos, const AscendParams &params) {
  // 清空
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  // 绘制尾暗头亮
  for (int i = 0; i < params.stripLen; i++) {
    int ledIndex = pos + i;
    if (ledIndex < 0 || ledIndex >= NUM_LEDS) continue;

    // i=0 → ratio=0 → 最暗； i=stripLen-1 → ratio=1 → 最亮
    float ratio = float(i) / float(params.stripLen - 1);
    uint8_t pixelBrightness = uint8_t(params.maxBrightness * ratio);

    // 固定白色，也可改成别的颜色
    leds[ledIndex] = CRGB::White;
    // 再做亮度缩放
    leds[ledIndex].nscale8(pixelBrightness);
  }
}
