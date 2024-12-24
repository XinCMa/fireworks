// 定义滑动变阻器的引脚
const int potentiometerPin = A0; // 滑动变阻器连接到模拟引脚 A0

void setup() {
  // 初始化串口通信
  Serial.begin(9600);
}

void loop() {
  // 读取滑动变阻器的模拟值
  int potentiometerValue = analogRead(potentiometerPin);

  // 将模拟值转换为电压（假设电压范围为 0-5V）
  float voltage = potentiometerValue * (5.0 / 1023.0);

  // 打印滑动变阻器的值和电压
  Serial.print("Potentiometer Value: ");
  Serial.print(potentiometerValue);
  Serial.print("\tVoltage: ");
  Serial.println(voltage, 2); // 保留两位小数

  // 延时 200 毫秒以避免刷屏
  delay(200);
}
