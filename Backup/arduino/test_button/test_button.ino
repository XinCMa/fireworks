// 定义按钮的引脚
const int buttonPin = 2; // 假设按钮连接到数字引脚 2

void setup() {
  // 初始化串口通信
  Serial.begin(9600);

  // 设置按钮引脚为输入模式
  pinMode(buttonPin, INPUT_PULLUP); // 使用内置上拉电阻
}

void loop() {
  // 读取按钮状态
  int buttonState = digitalRead(buttonPin);

  // 检查按钮是否被按下
  if (buttonState == LOW) { // 按下时，状态为 LOW
    Serial.println("Button Pressed");
  } else {
    Serial.println("Button Not Pressed");
  }

  // 为了避免刷屏，可以稍作延时
  delay(200);
}
