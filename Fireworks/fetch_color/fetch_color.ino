#include <FastLED.h>

// 灯带参数
#define LED_PIN     7          // LED 数据引脚
#define NUM_LEDS    20         // 灯珠数量
#define BRIGHTNESS  255        // 最大亮度

CRGB leds[NUM_LEDS];           // 创建 LED 数组

// Joystick 参数
#define JOY_X_PIN A0           // X 轴模拟信号
#define JOY_Y_PIN A1           // Y 轴模拟信号
#define JOY_DEADZONE 120        // 死区范围，避免抖动

// 状态变量
uint8_t lastHue = 0;           // 记录最后的颜色 Hue 值
bool inDeadzone = true;        // Joystick 是否在死区

void setup() {
  // 初始化灯带
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // 设置初始颜色为白色
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();

  // 初始化串口
  Serial.begin(9600);
  Serial.println("Joystick Color Selector Initialized");
}

void loop() {
  // 读取 Joystick 的 X 和 Y 值
  int xValue = analogRead(JOY_X_PIN) - 512; // 偏移中心值到 -512 ~ 512
  int yValue = analogRead(JOY_Y_PIN) - 512; // 偏移中心值到 -512 ~ 512

  // 检查是否在死区内
  bool currentlyInDeadzone = (abs(xValue) < JOY_DEADZONE && abs(yValue) < JOY_DEADZONE);

  if (!currentlyInDeadzone) {
    // Joystick 离开中心
    inDeadzone = false; // 标记为不在死区

    // 计算倾斜角度
    float angle = atan2(-yValue, xValue) * 180.0 / PI;
    if (angle < 0) {
      angle += 360; // 将角度范围调整到 0 ~ 360
    }

    // 将角度映射到 HSV 色环
    lastHue = (uint8_t)(angle / 360.0 * 255); // 更新最后的颜色

    // 设置 LED 颜色
    fill_solid(leds, NUM_LEDS, CHSV(lastHue, 255, BRIGHTNESS)); // 使用 HSV 设置颜色
    FastLED.show();

    // 打印调试信息
    Serial.print("Joystick Tilted. X: ");
    Serial.print(xValue);
    Serial.print(" Y: ");
    Serial.print(yValue);
    Serial.print(" | Angle: ");
    Serial.print(angle);
    Serial.print("° | Hue: ");
    Serial.println(lastHue);

  } else if (!inDeadzone) {
    // Joystick 回到中心，保持最后的颜色
    inDeadzone = true; // 标记为在死区
    Serial.println("Joystick Released. Keeping last color.");
  }

  delay(50); // 更新频率
}
