#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// 初始化 LCD1602A 屏幕，地址为 0x27，尺寸为 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // 初始化 LCD 屏幕
  lcd.init();
  lcd.backlight();

  // 在屏幕上显示初始信息
  lcd.setCursor(0, 0); // 设置光标到第一行第一个位置
  lcd.print("Testing LCD...");
  delay(2000); // 等待 2 秒

  // 清屏以准备显示新内容
  lcd.clear();
}

void loop() {
  // 显示动态信息
  lcd.setCursor(0, 0); // 第一行
  lcd.print("Row 1: Hello!");

  lcd.setCursor(0, 1); // 第二行
  lcd.print("Row 2: World!");

  // 每隔 1 秒刷新一次显示
  delay(1000);
}