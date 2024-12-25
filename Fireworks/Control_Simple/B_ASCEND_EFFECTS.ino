#include "A_GLOBAL.h"

AscendParams normalAscendCfg = {
  255,  // maxBrightness (头最亮)
  20,   // speedDelay (可以调大或调小)
  30    // stripLen
};

BarsWithGapParams barsCfg = {
  255,  // maxBrightness
  30,   // speedDelay (可调小加快动画，可调大放慢动画)
  5,    // segCount
  10,   // segLen
  5     // gapLen
};

/**
 * @brief 普通上升：条带从下标0~(NUM_LEDS - stripLen)移动，
 *        条带内部i=0处亮度最低，i=stripLen-1处亮度最高
 *        整个条带移动时，“头”在大索引那一端，“尾”在小索引那一端。
 */
void normalAscendEffect(const AscendParams &params) {
  Serial.println("Starting normalAscendEffect:");
  Serial.print("  Brightness: "); Serial.println(params.maxBrightness);
  Serial.print("  Speed: "); Serial.println(params.speedDelay);
  Serial.print("  Strip Length: "); Serial.println(params.stripLen);

  int topLimit = TOTAL_LED_COUNT - params.stripLen; 
  // pos 递增表示从小编号往大编号移动

  for (int pos = 0; pos <= topLimit; pos++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

    // 在 [pos, pos+stripLen-1] 这段绘制"尾暗头亮"
    for (int i = 0; i < params.stripLen; i++) {
      int ledIndex = pos + i;
      // 边界检查
      if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

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
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

  // 绘制尾暗头亮
  for (int i = 0; i < params.stripLen; i++) {
    int ledIndex = pos + i;
    if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

    // i=0 → ratio=0 → 最暗； i=stripLen-1 → ratio=1 → 最亮
    float ratio = float(i) / float(params.stripLen - 1);
    uint8_t pixelBrightness = uint8_t(params.maxBrightness * ratio);

    // 固定白色，也可改成别的颜色
    leds[ledIndex] = CRGB::White;
    // 再做亮度缩放
    leds[ledIndex].nscale8(pixelBrightness);
  }
}

void pendulumAscend(const AscendParams &params) {
  Serial.println("Starting pendulumAscend:");
  Serial.print("  Brightness: "); Serial.println(params.maxBrightness);
  Serial.print("  Speed: "); Serial.println(params.speedDelay);
  Serial.print("  Strip Length: "); Serial.println(params.stripLen);

  int topLimit = TOTAL_LED_COUNT - params.stripLen;
  int pos = 0;

  while(pos < topLimit) {
    // (1) 往前走 50
    int forwardTarget = pos + 50;
    if (forwardTarget > topLimit) {
      forwardTarget = topLimit;
    }
    movePos(pos, forwardTarget, +1, params);
    pos = forwardTarget;

    if (pos >= topLimit) {
      break;
    }

    // (2) 往回走 10
    int backwardTarget = pos - 10;
    if (backwardTarget < 0) {
      backwardTarget = 0;
    }
    movePos(pos, backwardTarget, -1, params);
    pos = backwardTarget;
  }

  // 到达顶端后清空
  fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);
  FastLED.show();
  delay(2000);
}

/**
 * @brief 在灯带底部到顶部之间，移动一个包含 "5 个短条带 + 4 个间隔" 的整体
 *        1. 每个短条带 segLen=10，相邻间隔 gapLen=5
 *        2. 条带内部做 "尾暗头亮" 的线性渐变
 *        3. 间隔处始终保持熄灭 (黑色)
 *        4. 最终占用 totalSegLen = segCount*segLen + (segCount-1)*gapLen
 */
void multipleShortBarsWithGapsAscend(const BarsWithGapParams &cfg) {
  Serial.println("Starting multipleShortBarsWithGapsAscend:");
  Serial.print("  Brightness: "); Serial.println(cfg.maxBrightness);
  Serial.print("  Speed: "); Serial.println(cfg.speedDelay);
  Serial.print("  Segments: "); Serial.println(cfg.segCount);
  Serial.print("  Segment Length: "); Serial.println(cfg.segLen);
  Serial.print("  Gap Length: "); Serial.println(cfg.gapLen);

  // 1) 计算 5 条带 + 4 间隔 共占用多少颗 LED
  int totalSegLen = cfg.segCount * cfg.segLen + (cfg.segCount - 1) * cfg.gapLen; 
  // 对 5个条带 + 4处间隔 = 5*10 + 4*5 = 70

  // 2) 计算移动的最顶位置
  int topLimit = TOTAL_LED_COUNT - totalSegLen; // 若=200, 则topLimit=130

  // 3) 从 pos=0 移动到 pos=topLimit
  for (int pos = 0; pos <= topLimit; pos++) {
    // 清空灯带
    fill_solid(leds, TOTAL_LED_COUNT, CRGB::Black);

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
        if (ledIndex < 0 || ledIndex >= TOTAL_LED_COUNT) continue;

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
