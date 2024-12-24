#define DIR_PIN 3  // 方向引脚
#define PUL_PIN 4  // 脉冲信号引脚
#define ENA_PIN 5  // 使能引脚（可选）

// 限位步数
const int MAX_STEPS = 3200; // 最大步数（设定为 1 转，1.8° 步距电机 1/16 细分需要 3200 步）
const int MIN_STEPS = 0;    // 最小步数（通常为 0）

int currentStep = 0; // 当前步数记录

void setup() {
  pinMode(DIR_PIN, OUTPUT); // 设置方向引脚为输出
  pinMode(PUL_PIN, OUTPUT); // 设置脉冲引脚为输出
  pinMode(ENA_PIN, OUTPUT); // 设置使能引脚为输出

  digitalWrite(ENA_PIN, LOW);  // 启用驱动器
  digitalWrite(DIR_PIN, HIGH); // 默认方向为正转

  Serial.begin(9600); // 初始化串口通信
  Serial.println("输入 '1' 开始正转，'2' 开始反转，'s' 停止电机。");
}

void loop() {
  if (Serial.available() > 0) {
    char input = Serial.read();

    if (input == '1') { // 正转
      Serial.println("电机正转...");
      digitalWrite(DIR_PIN, HIGH);
      runWithLimits();
    } else if (input == '2') { // 反转
      Serial.println("电机反转...");
      digitalWrite(DIR_PIN, LOW);
      runWithLimits();
    } else if (input == 's') { // 停止
      Serial.println("电机停止。");
    } else {
      Serial.println("无效输入，请输入 '1'、'2' 或 's'。");
    }
  }
}

// 函数：带逻辑限位的持续运行
void runWithLimits() {
  while (true) {
    // 根据方向判断是否超过步数限制
    if (digitalRead(DIR_PIN) == HIGH && currentStep >= MAX_STEPS) {
      Serial.println("达到正向限位，停止电机！");
      break; // 达到最大步数限制，停止
    }
    if (digitalRead(DIR_PIN) == LOW && currentStep <= MIN_STEPS) {
      Serial.println("达到反向限位，停止电机！");
      break; // 达到最小步数限制，停止
    }

    // 发送脉冲信号
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(500); // 高电平持续时间
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(500); // 低电平持续时间

    // 更新步数记录
    if (digitalRead(DIR_PIN) == HIGH) {
      currentStep++; // 正转，步数增加
    } else {
      currentStep--; // 反转，步数减少
    }

    // 检查是否有停止命令
    if (Serial.available() > 0) {
      char stopInput = Serial.read();
      if (stopInput == 's') {
        Serial.println("用户输入停止命令。");
        break;
      }
    }
  }
}
