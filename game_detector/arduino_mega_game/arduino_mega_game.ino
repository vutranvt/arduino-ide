#include <String.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
#include "bmt_config.h"

#define IN_PIN_1 22  
#define OUT_PIN_1 23 

#define IN_PIN_2 24  
#define OUT_PIN_2 25 

#define IN_PIN_3 26  
#define OUT_PIN_3 27 

#define IN_PIN_4 28
#define OUT_PIN_4 29 

#define IN_PIN_5 30  
#define OUT_PIN_5 31

#define IN_PIN_6 32  
#define OUT_PIN_6 33 
  
#define IN_PIN_7 34 
#define OUT_PIN_7 35

#define IN_PIN_8 36 
#define OUT_PIN_8 37

#define TEST_IN_PIN 4
#define TEST_OUT_PIN 5

int inState = LOW;   		//Trạng thái các pin ngõ vào đọc được 
int previousInState = LOW;	//Trạng thái các pin ngõ ra trước đó 

int outState = LOW;			//Trạng thái các pin ngõ ra đọc được
int previousOutState = LOW;	//Trạng thái các pin ngõ ra trước đó

//Biến lưu trạng thái của pin ngõ vào
int preIn1 = HIGH;	
int preIn2 = HIGH;	
int preIn3 = HIGH;
int preIn4 = HIGH;
int preIn5 = HIGH;
int preIn6 = HIGH;
int preIn7 = HIGH;
int preIn8 = HIGH;

//Biến lưu trạng thái của pin ngõ ra
int preOut1 = HIGH;
int preOut2 = HIGH;
int preOut3 = HIGH;
int preOut4 = HIGH;
int preOut5 = HIGH;
int preOut6 = HIGH;
int preOut7 = HIGH;
int preOut8 = HIGH;

unsigned long currentOutLevelMillis = 0;
unsigned long rxPulse = 5;	//xung (ms) nhận các pin ngõ vào
unsigned long txPulse = 10;	//xung (ms) truyền các pin ngõ ra

const byte RX = 2;          // Chân 2 được dùng làm chân RX
const byte TX = 3;          // Chân 3 được dùng làm chân TX
SoftwareSerial mySerial = SoftwareSerial(RX, TX);

SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

void setup() {
  
    pinMode(OUT_PIN_1, OUTPUT);
    digitalWrite(OUT_PIN_1, HIGH);
    pinMode(IN_PIN_1, INPUT_PULLUP);

    pinMode(OUT_PIN_2, OUTPUT);
    digitalWrite(OUT_PIN_2, HIGH);
    pinMode(IN_PIN_2, INPUT_PULLUP);

    pinMode(OUT_PIN_3, OUTPUT);
    digitalWrite(OUT_PIN_3, HIGH);
    pinMode(IN_PIN_3, INPUT_PULLUP);

    pinMode(OUT_PIN_4, OUTPUT);
    digitalWrite(OUT_PIN_4, HIGH);
    pinMode(IN_PIN_4, INPUT_PULLUP);

    pinMode(OUT_PIN_5, OUTPUT);
    digitalWrite(OUT_PIN_5, HIGH);
    pinMode(IN_PIN_5, INPUT_PULLUP);

    pinMode(OUT_PIN_6, OUTPUT);
    digitalWrite(OUT_PIN_6, HIGH);
    pinMode(IN_PIN_6, INPUT_PULLUP);

    pinMode(OUT_PIN_7, OUTPUT);
    digitalWrite(OUT_PIN_7, HIGH);
    pinMode(IN_PIN_7, INPUT_PULLUP);

    pinMode(OUT_PIN_8, OUTPUT);
    digitalWrite(OUT_PIN_8, HIGH);
    pinMode(IN_PIN_8, INPUT_PULLUP);

    pinMode(TEST_IN_PIN, INPUT);

    pinMode(TEST_OUT_PIN, OUTPUT);
    digitalWrite(TEST_OUT_PIN, HIGH);
    // Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
    Serial.begin(115200);
    // Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
    mySerial.begin(57600);

    // Một số hàm trong thư viện Serial Command
    sCmd.addDefaultHandler(defaultCommand);

}
    
void loop() {
    sCmd.readSerial();
//    Serial.println("Start program");
    int in1 = digitalRead(IN_PIN_1);
    int in2 = digitalRead(IN_PIN_2);
    int in3 = digitalRead(IN_PIN_3);
    int in4 = digitalRead(IN_PIN_4);
    int in5 = digitalRead(IN_PIN_5);
    int in6 = digitalRead(IN_PIN_6);
    int in7 = digitalRead(IN_PIN_7);
    int in8 = digitalRead(IN_PIN_8);

    // int tempState = (in4<<3) | (in3<<2) | (in2<<1) | in1;
    int tempState = (in8<<7) | (in7<<6) | (in6<<5) | (in5<<4) | (in4<<3) | (in3<<2) | (in2<<1) | in1;

    Serial.print("tempState: ");
    Serial.println(tempState);

    // Kiểm tra Pin có thật sự xuống GND hay không
    // Nếu Pin được kích: true, không được kích: false
    if (pinIsPressed(tempState)==true) {    
        inState = tempState;
        
        // Có PIN -> LOW
        // if (inState!=0xF) {   
        if (inState!=0xFF) {   
            // Kiểm tra trạng thái Pin: 
            // Có Pin thay đổi: 0->1 hoặc 1->0
            if (inState!=previousInState) {
                StaticJsonBuffer<200> jsonBuffer;
                JsonObject& root = jsonBuffer.createObject();

                if (in1==LOW && preIn1==HIGH) {         //Trạng thái pin: 1->0 (preIn -> in) 
                    pinMode(OUT_PIN_1, OUTPUT);
                    digitalWrite(OUT_PIN_1, LOW);    
                    preIn1 = LOW;
                    root[bc01] = "on";
                } 
                else if (in1==HIGH && preIn1==LOW) {    //Trạng thái pin: 0->1
                    preIn1 = HIGH;
                }
                if (in2==LOW && preIn2==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_2, OUTPUT);
                    digitalWrite(OUT_PIN_2, LOW);
                    preIn2 = LOW;
                    root[bc02] = "on";
                } 
                else if (in2==HIGH && preIn2==LOW) {    //Trạng thái pin: 0->1
                    preIn2 = HIGH;
                }            
                if (in3==LOW && preIn3==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_3, OUTPUT);
                    digitalWrite(OUT_PIN_3, LOW);
                    preIn3 = LOW;
                    root[bc03] = "on";
                } 
                else if (in3==HIGH && preIn3==LOW) {    //Trạng thái pin: 0->1
                    preIn3 = HIGH;
                }
                if (in4==LOW && preIn4==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_4, OUTPUT);
                    digitalWrite(OUT_PIN_4, LOW);
                    preIn4 = LOW;
                    root[bc04] = "on";
                } 
                else if (in4==HIGH && preIn4==LOW) {    //Trạng thái pin: 0->1
                    preIn4 = HIGH;
                }
                if (in5==LOW && preIn5==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_5, OUTPUT);
                    digitalWrite(OUT_PIN_5, LOW);
                    preIn5 = LOW;
                    root[bc05] = "on";
                } 
                else if (in5==HIGH && preIn5==LOW) {    //Trạng thái pin: 0->1
                    preIn5 = HIGH;
                }
                if (in6==LOW && preIn6==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_6, OUTPUT);
                    digitalWrite(OUT_PIN_6, LOW);
                    preIn6 = LOW;
                    root[bc06] = "on";
                } 
                else if (in6==HIGH && preIn6==LOW) {    //Trạng thái pin: 0->1
                    preIn6 = HIGH;
                }
                if (in7==LOW && preIn7==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_7, OUTPUT);
                    digitalWrite(OUT_PIN_7, LOW);
                    preIn7 = LOW;
                    root[bc07] = "on";
                } 
                else if (in7==HIGH && preIn7==LOW) {    //Trạng thái pin: 0->1
                    preIn7 = HIGH;
                }
                if (in8==LOW && preIn8==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_8, OUTPUT);
                    digitalWrite(OUT_PIN_8, LOW);
                    preIn8 = LOW;
                    root[bc08] = "on";
                } 
                else if (in8==HIGH && preIn8==LOW) {    //Trạng thái pin: 0->1
                    preIn8 = HIGH;
                }

                // Có tồn tại data thì mới gửi dữ liệu
//                if (root) {
                    mySerial.print('arduino');
                    mySerial.print('\r');               
                    root.printTo(mySerial);
                    mySerial.print('\r'); 
//                } 
                delay(txPulse);
            }
            // Không có Pin thay đổi
            else {
                txOutPin();
            }
        }
        // Không có Pin -> LOW: kiểm tra trạng thái của các Pin Out
        else {
        
            txOutPin();
            // Lưu trạng thái của các Pin In trong "preIn"
            preIn1 = HIGH;
            preIn2 = HIGH;
            preIn3 = HIGH;
            preIn4 = HIGH;
            preIn5 = HIGH;
            preIn6 = HIGH;
            preIn7 = HIGH;
            preIn8 = HIGH;
        }
        previousInState = inState;  // Lưu trạng thái của các Pin In
    }
}

void rxCommand(String command) {
    char *json = sCmd.next();

    Serial.println(json);
    
}
void defaultCommand(String command) {
    char *json = sCmd.next();

    Serial.println(json);
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    unsigned int updownValue = root["updown"];
    unsigned int leftrightValue = root["leftright"];
    Serial.print("updown value: ");
    Serial.print(updownValue);
    Serial.print("----leftright value: ");
    Serial.println(leftrightValue);
    
}


bool pinIsPressed (int value) {
    delay(rxPulse);
    int in1 = digitalRead(IN_PIN_1);
    int in2 = digitalRead(IN_PIN_2);
    int in3 = digitalRead(IN_PIN_3);
    int in4 = digitalRead(IN_PIN_4);
    int in5 = digitalRead(IN_PIN_5);
    int in6 = digitalRead(IN_PIN_6);
    int in7 = digitalRead(IN_PIN_7);
    int in8 = digitalRead(IN_PIN_8);

    // int temp = (in4<<3) | (in3<<2) | (in2<<1) | in1;
    int temp = (in8<<7) | (in7<<6) | (in6<<5) | (in5<<4) | (in4<<3) | (in3<<2) | (in2<<1) | in1;
    
    if (value==temp) {
        return true;
    } else {
        return false;
    }
}


void txOutPin () {
    pinMode(OUT_PIN_1, INPUT);
    pinMode(OUT_PIN_2, INPUT);
    pinMode(OUT_PIN_3, INPUT);
    pinMode(OUT_PIN_4, INPUT);
    pinMode(OUT_PIN_5, INPUT);
    pinMode(OUT_PIN_6, INPUT);
    pinMode(OUT_PIN_7, INPUT);
    pinMode(OUT_PIN_8, INPUT);
    int out1 = digitalRead(OUT_PIN_1);
    int out2 = digitalRead(OUT_PIN_2);
    int out3 = digitalRead(OUT_PIN_3);
    int out4 = digitalRead(OUT_PIN_4);
    int out5 = digitalRead(OUT_PIN_5);
    int out6 = digitalRead(OUT_PIN_6);
    int out7 = digitalRead(OUT_PIN_7);
    int out8 = digitalRead(OUT_PIN_8);

    // outState = (out4<<3) | (out3<<2) | (out2<<1) | out1;
    outState = (out8<<7) | (out7<<6) | (out6<<5) | (out5<<4) | (out4<<3) | (out3<<2) | (out2<<1) | out1;
    Serial.print("outState: ");
    Serial.println(outState);
    delay(1000);

    // Nếu Pin Out -> LOW: Gửi data Pin Out theo chu kỳ "2000" (ms)
    if (outState!=0xFF && ((unsigned long)(millis() - currentOutLevelMillis) >= 2000)) {

        StaticJsonBuffer<200> jsonBuffer2;
        JsonObject& root2 = jsonBuffer2.createObject();

        if (out1==LOW) {           //pin out: 1->0
            root2[bc01] = "disconnected";
        } else {
            root2[bc01] = "connected";
        }

        if (out2==LOW) {           //pin out: 1->0
            root2[bc02] = "disconnected";
        } else {
            root2[bc02] = "connected";
        }

        if (out3==LOW) {           //pin out: 1->0
            root2[bc03] = "disconnected";
        } else {
            root2[bc03] = "connected";
        }

        if (out4==LOW) {           //pin out: 1->0
            root2[bc04] = "disconnected";
        } else {
            root2[bc04] = "connected";
        }

        if (out5==LOW) {           //pin out: 1->0
            root2[bc05] = "disconnected";
        } else {
            root2[bc05] = "connected";
        }

        if (out6==LOW) {           //pin out: 1->0
            root2[bc06] = "disconnected";
        } else {
            root2[bc06] = "connected";
        }

        if (out7==LOW) {           //pin out: 1->0
            root2[bc07] = "disconnected";
        } else {
            root2[bc07] = "connected";
        }

        if (out8==LOW) {           //pin out: 1->0
            root2[bc08] = "disconnected";
        } else {
            root2[bc08] = "connected";
        }

        mySerial.print('arduino');
        mySerial.print('\r');               
        root2.printTo(mySerial);
        mySerial.print('\r'); 

        Serial.println();
        root2.printTo(Serial);
        currentOutLevelMillis = millis(); 
    }

}
