#include <Servo.h>  // 包含舵机库

Servo myServo;  // 创建一个Servo对象

void setup() {
  myServo.attach(9);  // 将舵机控制引脚连接到Arduino的9号引脚
}

void loop() {
  myServo.write(0);    // 将舵机转到0度
  delay(1000);         // 等待1秒

  myServo.write(90);   // 将舵机转到90度
  delay(1000);         // 等待1秒

  myServo.write(180);  // 将舵机转到180度
  delay(1000);         // 等待1秒
}