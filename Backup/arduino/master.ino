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
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    pinMode(BUTTON_3_PIN, INPUT_PULLUP);
    pinMode(BUTTON_4_PIN, INPUT_PULLUP);
    pinMode(BUTTON_5_PIN, INPUT_PULLUP);

    pinMode(SLIDER_1_PIN, INPUT);
    pinMode(SLIDER_2_PIN, INPUT);
    pinMode(SLIDER_3_PIN, INPUT);
    pinMode(SLIDER_4_PIN, INPUT);

    pinMode(MASTER_CONTROL, INPUT_PULLUP);
    pinMode(LAUNCH, INPUT_PULLUP);

    pinMode(NUM_PAD_1, INPUT_PULLUP);
    pinMode(NUM_PAD_2, INPUT_PULLUP);
    pinMode(NUM_PAD_3, INPUT_PULLUP);
    pinMode(NUM_PAD_4, INPUT_PULLUP);
    pinMode(NUM_PAD_5, INPUT_PULLUP);
    pinMode(NUM_PAD_6, INPUT_PULLUP);
    pinMode(NUM_PAD_7, INPUT_PULLUP);
    pinMode(NUM_PAD_8, INPUT_PULLUP);

    // 初始化输出
    pinMode(MASTER_LED_R, OUTPUT);
    pinMode(MASTER_LED_G, OUTPUT);
    pinMode(MASTER_LED_B, OUTPUT);

    pinMode(DIGITAL_DISPLAY_1, OUTPUT);
    pinMode(DIGITAL_DISPLAY_2, OUTPUT);

    pinMode(STEPPER_MOTOR_1, OUTPUT);
    pinMode(STEPPER_MOTOR_2, OUTPUT);
    pinMode(STEPPER_MOTOR_3, OUTPUT);

    pinMode(SERVO_MOTOR_1, OUTPUT);
    pinMode(LASER_1, OUTPUT);
    pinMode(LASER_2, OUTPUT);

    pinMode(LED_STRIP_1, OUTPUT);

    Serial.begin(9600); // 用于调试的串口
}

void loop() {
    // 读取按钮状态
    bool launchState = digitalRead(LAUNCH);
    bool button1State = digitalRead(BUTTON_1_PIN);
    bool button2State = digitalRead(BUTTON_2_PIN);
    bool button3State = digitalRead(BUTTON_3_PIN);
    bool button4State = digitalRead(BUTTON_4_PIN);
    bool button5State = digitalRead(BUTTON_5_PIN);

    // 读取滑轨状态
    int slider1Value = analogRead(SLIDER_1_PIN);
    int slider2Value = analogRead(SLIDER_2_PIN);
    int slider3Value = analogRead(SLIDER_3_PIN);
    int slider4Value = analogRead(SLIDER_4_PIN);

    // 读取数字输入状态
    bool numPad1State = digitalRead(NUM_PAD_1);
    bool numPad2State = digitalRead(NUM_PAD_2);
    bool numPad3State = digitalRead(NUM_PAD_3);
    bool numPad4State = digitalRead(NUM_PAD_4);
    bool numPad5State = digitalRead(NUM_PAD_5);
    bool numPad6State = digitalRead(NUM_PAD_6);
    bool numPad7State = digitalRead(NUM_PAD_7);
    bool numPad8State = digitalRead(NUM_PAD_8);

    if (launchState == LOW) {
        // 启动输出
        // 控制步进电机
        digitalWrite(STEPPER_MOTOR_1, HIGH);
        digitalWrite(STEPPER_MOTOR_2, HIGH);
        digitalWrite(STEPPER_MOTOR_3, HIGH);

        // 控制舵机
        digitalWrite(SERVO_MOTOR_1, HIGH);

        // 控制激光
        digitalWrite(LASER_1, HIGH);
        digitalWrite(LASER_2, HIGH);

        // 控制LED灯带
        digitalWrite(LED_STRIP_1, HIGH);

        // 根据按钮状态修改颜色
        if (button1State == LOW) {
            analogWrite(MASTER_LED_R, 255);
        } else if (button2State == LOW) {
            analogWrite(MASTER_LED_G, 255);
        } else if (button3State == LOW) {
            analogWrite(MASTER_LED_B, 255);
        } else if (button4State == LOW) {
            analogWrite(MASTER_LED_R, 128);
            analogWrite(MASTER_LED_G, 128);
        } else if (button5State == LOW) {
            analogWrite(MASTER_LED_R, 128);
            analogWrite(MASTER_LED_B, 128);
        }

        // 根据滑轨状态调整速度密度
        int speed = map(slider1Value, 0, 1023, 0, 255);
        analogWrite(STEPPER_MOTOR_1, speed);
        analogWrite(STEPPER_MOTOR_2, speed);
        analogWrite(STEPPER_MOTOR_3, speed);

        int density = map(slider2Value, 0, 1023, 0, 255);
        analogWrite(LED_STRIP_1, density);
    } else {
        // 停止输出
        digitalWrite(STEPPER_MOTOR_1, LOW);
        digitalWrite(STEPPER_MOTOR_2, LOW);
        digitalWrite(STEPPER_MOTOR_3, LOW);

        digitalWrite(SERVO_MOTOR_1, LOW);
        digitalWrite(LASER_1, LOW);
        digitalWrite(LASER_2, LOW);
        digitalWrite(LED_STRIP_1, LOW);

        analogWrite(MASTER_LED_R, 0);
        analogWrite(MASTER_LED_G, 0);
        analogWrite(MASTER_LED_B, 0);
    }

    delay(100); // 防止过于频繁的循环
}
