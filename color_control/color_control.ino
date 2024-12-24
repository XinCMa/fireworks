#include <FastLED.h>

// 灯带参数
#define LED_PIN     7          // LED 数据引脚
#define NUM_LEDS    20         // 灯珠数量
#define BRIGHTNESS  255        // 最大亮度

CRGB leds[NUM_LEDS];           // 创建 LED 数组

// 按钮引脚
#define RED_BUTTON   2         // 红按钮引脚
#define YELLOW_BUTTON 3        // 黄按钮引脚
#define BLUE_BUTTON  4         // 蓝按钮引脚
#define GREEN_BUTTON 5         // 绿按钮引脚
#define PURPLE_BUTTON 6        // 紫按钮引脚

// 按钮颜色和浓度等级
int redLevel = 0, yellowLevel = 0, blueLevel = 0, greenLevel = 0, purpleLevel = 0;

// 定义浓度值（0 表示未按，1-3 表示浓度）
const int maxLevel = 3;

// 检测按钮状态
bool lastStateRed = LOW;
bool lastStateYellow = LOW;
bool lastStateBlue = LOW;
bool lastStateGreen = LOW;
bool lastStatePurple = LOW;

void setup() {
  // 初始化灯带
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // 清空灯带状态
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(100);

  // 设置初始颜色为白色
  fill_solid(
  );
  FastLED.show();

  // 设置按钮为输入
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(PURPLE_BUTTON, INPUT_PULLUP);
}

void loop() {
  // 检测按钮按下，切换等级
  checkButton(RED_BUTTON, redLevel, lastStateRed);
  checkButton(YELLOW_BUTTON, yellowLevel, lastStateYellow);
  checkButton(BLUE_BUTTON, blueLevel, lastStateBlue);
  checkButton(GREEN_BUTTON, greenLevel, lastStateGreen);
  checkButton(PURPLE_BUTTON, purpleLevel, lastStatePurple);

  // 如果所有颜色等级都为 0，保持白色
  if (redLevel == 0 && yellowLevel == 0 && blueLevel == 0 && greenLevel == 0 && purpleLevel == 0) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
  } else {
    // 计算叠加颜色
    CRGB finalColor = calculateFinalColor();
    fill_solid(leds, NUM_LEDS, finalColor);
  }

  // 显示灯光
  FastLED.show();
  delay(50); // 防止按键抖动
}

// 检测按钮按下并更新等级
void checkButton(int pin, int &level, bool &lastState) {
  bool currentState = digitalRead(pin) == LOW; // 按钮被按下时为 LOW
  if (currentState && !lastState) {           // 检测按下的状态变化
    level = (level + 1) % (maxLevel + 1);     // 切换浓度等级
  }
  lastState = currentState;                   // 更新按钮状态
}

// 根据各颜色等级计算最终颜色
CRGB calculateFinalColor() {
  int redValue = map(redLevel, 0, maxLevel, 0, 255);      // 红色浓度
  int yellowValue = map(yellowLevel, 0, maxLevel, 0, 255); // 黄色浓度
  int blueValue = map(blueLevel, 0, maxLevel, 0, 255);    // 蓝色浓度
  int greenValue = map(greenLevel, 0, maxLevel, 0, 255);  // 绿色浓度
  int purpleValue = map(purpleLevel, 0, maxLevel, 0, 255);// 紫色浓度

  // 分解黄色和紫色为 RGB 成分
  int r = redValue + yellowValue + purpleValue / 2; // 红色分量
  int g = greenValue + yellowValue;                // 绿色分量
  int b = blueValue + purpleValue;                 // 蓝色分量

  // 确保 RGB 值不超出范围
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);

  return CRGB(r, g, b); // 返回叠加后的颜色
}
