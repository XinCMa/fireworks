
// input
#define BUTTON_1_PIN 2   
#define BUTTON_2_PIN 3   
#define BUTTON_3_PIN 4   
#define BUTTON_4_PIN 5   
#define BUTTON_5_PIN 6   

#define SLIDER_1_PIN A1    
#define SLIDER_2_PIN A2   
#define SLIDER_3_PIN A3   
#define SLIDER_4_PIN A4

#define MASTER_CONTROL 13   
#define LAUNCH 14 

#define NUM_PAD_1 15
#define NUM_PAD_2 16
#define NUM_PAD_3 17
#define NUM_PAD_4 18
#define NUM_PAD_5 19
#define NUM_PAD_6 20
#define NUM_PAD_7 21
#define NUM_PAD_8 22


// display
#define MASTER_LED_R 7
#define MASTER_LED_G 8
#define MASTER_LED_B 9

#define DIGITAL_DISPLAY_1 25
#define DIGITAL_DISPLAY_2 26

// output
#define STEPPER_MOTOR_1 27
#define STEPPER_MOTOR_2 28
#define STEPPER_MOTOR_3 29

#define SERVO_MOTOR_1 30
#define LASER_1 31
#define LASER_2 32

#define LED_STRIP_1 33


void setup() {
    // 初始化输入
    pinMode(SENSOR_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // 初始化输出
    pinMode(LED_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);

    Serial.begin(9600); // 用于调试的串口
}

void loop() {
    // 1. 读取传感器输入
    int sensorValue = analogRead(SENSOR_PIN);
    Serial.print("Sensor Value: ");
    Serial.println(sensorValue);

    // 2. 读取按钮状态
    bool buttonState = digitalRead(BUTTON_PIN);

    // 3. 根据输入执行逻辑
    if (sensorValue > 500 || buttonState == LOW) {
        digitalWrite(LED_PIN, HIGH);  // 打开LED
        digitalWrite(RELAY_PIN, HIGH); // 触发继电器
    } else {
        digitalWrite(LED_PIN, LOW);   // 关闭LED
        digitalWrite(RELAY_PIN, LOW); // 关闭继电器
    }

    delay(100); // 防止过于频繁的循环
}
