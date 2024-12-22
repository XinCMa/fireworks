#include <Adafruit_NeoPixel.h>

// 定义灯条相关的参数
#define LED_PIN     6    // 定义连接到灯条数据线的 Arduino 引脚
#define NUM_LEDS    60   // 灯条的总灯珠数量
#define BRIGHTNESS  255  // 最大亮度(0-255)
#define BASE_DELAY  30   // 流星基础速度（数值越小越快）

// 初始化灯条对象
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS); // 设置灯条亮度
  strip.show(); // 确保所有灯珠初始状态为关闭
}

void loop() {
  // 创建流星效果
  createMeteor();
}

// 函数：创建流星效果
void createMeteor() {
  int steps = NUM_LEDS; // 流星的亮度变化步数
  for (int offset = 0; offset < NUM_LEDS; offset++) { // 流星流动
    for (int i = 0; i < NUM_LEDS; i++) {
      // 计算当前灯珠的动态亮度
      int distance = (i - offset + NUM_LEDS) % NUM_LEDS; // 处理循环流动
      int brightness = map(distance, 0, NUM_LEDS - 1, 255, 1); // 将亮度从255到1映射
      
      // 设置当前灯珠亮度
      strip.setPixelColor(i, strip.Color(brightness, brightness, brightness)); // 白色
    }
    strip.show(); // 更新灯条显示

    // 控制流星速度
    delay(BASE_DELAY); // 调整整体流动速度
  }
}
