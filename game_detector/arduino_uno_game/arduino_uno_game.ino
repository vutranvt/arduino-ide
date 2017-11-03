#include <String.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
#include "arduino_game.h"

#define IN_PIN_1 2  
#define OUT_PIN_1 3 

#define IN_PIN_2 4  
#define OUT_PIN_2 5 

#define IN_PIN_3 6  
#define OUT_PIN_3 7 

#define IN_PIN_4 8  
#define OUT_PIN_4 9 

#define IN_PIN_5 10  
#define OUT_PIN_5 11 
  




int inState = LOW;   // 
int previousInState = LOW;    

int outState = LOW;
int previousOutState = LOW;

int preIn1 = HIGH;
int preIn2 = HIGH;
int preIn3 = HIGH;
int preIn4 = HIGH;
int preIn5 = HIGH;
int preIn6 = HIGH;
int preIn7 = HIGH;
int preIn8 = HIGH;

int preOut1 = HIGH;
int preOut2 = HIGH;
int preOut3 = HIGH;
int preOut4 = HIGH;
int preOut5 = HIGH;
int preOut6 = HIGH;
int preOut7 = HIGH;
int preOut8 = HIGH;

unsigned long startTime = 0;
unsigned long endTime = 5 * 100;   // thời gian (ms) mỗi lần gửi dữ liệu 'pin out'
unsigned long rxPulse = 10;
unsigned long txPulse = 20;
unsigned long counter = 0;

String data1 = "";
String data2 = "";
String data3 = "";
String data4 = "";
String data5 = "";

const byte RX = 12;          // Chân 3 được dùng làm chân RX
const byte TX = 13;          // Chân 2 được dùng làm chân TX
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

    // Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
    Serial.begin(115200);
    // Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
    mySerial.begin(38400);

    // Một số hàm trong thư viện Serial Command
    sCmd.addDefaultHandler(defaultCommand);

}
    
void loop() {
    sCmd.readSerial();

    int in1 = digitalRead(IN_PIN_1);
    int in2 = digitalRead(IN_PIN_2);
    int in3 = digitalRead(IN_PIN_3);
    int in4 = digitalRead(IN_PIN_4);

    int tempState = (in1<<3) | (in2<<2) | (in3<<1) | in4;
    // Serial.print("inState: ");
    // Serial.println(state);

    // Kiểm tra Pin có thật sự xuống GND hay không
    // Nếu Pin được kích: true, không được kích: false
    if (checkPin(tempState)==true) {    
        inState = tempState;
        
        // Có PIN -> LOW
        if (inState!=0xF) {   
            // Kiểm tra trạng thái Pin: 
            // Có Pin thay đổi: 0->1 hoặc 1->0
            if (inState!=previousInState) {
                StaticJsonBuffer<200> jsonBuffer;
                JsonObject& root = jsonBuffer.createObject();

                if (in1==LOW && preIn1==HIGH) {         //Trạng thái pin: 1->0 (preIn -> in) 
                    pinMode(OUT_PIN_1, OUTPUT);
                    digitalWrite(OUT_PIN_1, LOW);    
                    preIn1 = LOW;
                    root[bc01] = "coin on";
                } 
                else if (in1==HIGH && preIn1==LOW) {    //Trạng thái pin: 0->1
                    preIn1 = HIGH;
                }
                if (in2==LOW && preIn2==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_2, OUTPUT);
                    digitalWrite(OUT_PIN_2, LOW);
                    preIn2 = LOW;
                    root[bc02] = "coin on";
                } 
                else if (in2==HIGH && preIn2==LOW) {    //Trạng thái pin: 0->1
                    preIn2 = HIGH;
                }            
                if (in3==LOW && preIn3==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_3, OUTPUT);
                    digitalWrite(OUT_PIN_3, LOW);
                    preIn3 = LOW;
                    root[bc03] = "coin on";
                } 
                else if (in3==HIGH && preIn3==LOW) {    //Trạng thái pin: 0->1
                    preIn3 = HIGH;
                }
                if (in4==LOW && preIn4==HIGH) {         //Trạng thái pin: 1->0
                    pinMode(OUT_PIN_4, OUTPUT);
                    digitalWrite(OUT_PIN_4, LOW);
                    preIn4 = LOW;
                    root[bc04] = "coin on";
                } 
                else if (in4==HIGH && preIn4==LOW) {    //Trạng thái pin: 0->1
                    preIn4 = HIGH;
                }

                // Có tồn tại data thì mới gửi dữ liệu
//                if (root) {
                    mySerial.print('arduino');
                    mySerial.print('\r');               
                    root.printTo(mySerial);
                    mySerial.print('\r'); 
//                } 
                delay(20);
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


bool checkPin (int value) {
    delay(rxPulse);
    int in1 = digitalRead(IN_PIN_1);
    int in2 = digitalRead(IN_PIN_2);
    int in3 = digitalRead(IN_PIN_3);
    int in4 = digitalRead(IN_PIN_4);

    int temp = (in1<<3) | (in2<<2) | (in3<<1) | in4;
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
    int out1 = digitalRead(OUT_PIN_1);
    int out2 = digitalRead(OUT_PIN_2);
    int out3 = digitalRead(OUT_PIN_3);
    int out4 = digitalRead(OUT_PIN_4);

    outState = (out1<<3) | (out2<<2) | (out3<<1) | out4;
    // Serial.print("outState: ");
    // Serial.println(outState);

    // Nếu Pin Out -> LOW: Gửi data Pin Out theo chu kỳ "endTime" (ms)
    if (outState!=0xF && ((unsigned long)(millis()-startTime)>=endTime)) {

        StaticJsonBuffer<200> jsonBuffer2;
        JsonObject& root2 = jsonBuffer2.createObject();

        if (out1==LOW) {           //pin out: 1->0
            root2[bc01] = "board off";
        }
        if (out2==LOW) {           //pin out: 1->0
            root2[bc02] = "board off";
        }
        if (out3==LOW) {           //pin out: 1->0
            root2[bc03] = "board off";
        }
        if (out4==LOW) {           //pin out: 1->0
            root2[bc04] = "board off";
        }

        mySerial.print('arduino');
        mySerial.print('\r');               
        root2.printTo(mySerial);
        mySerial.print('\r'); 

        startTime = millis();        
    }

}
