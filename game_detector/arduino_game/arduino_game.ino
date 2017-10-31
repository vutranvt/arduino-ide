#include <String.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command


//#define UP_PIN 4        // D1
//#define DOWN_PIN 5      // D2
//#define LEFT_PIN 6     // D5
//#define RIGHT_PIN 7    // D6
//#define DETECT_PIN 8   // D7
//#define DETECT_PIN2 9   // SD3

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
  

const byte RX = 12;          // Chân 3 được dùng làm chân RX
const byte TX = 13;          // Chân 2 được dùng làm chân TX


int state = LOW;   // state mức cao: chưa phát hiện
int previousState = LOW;    

unsigned long startTime = 0;
unsigned long endTime = 5 * 100;   // thời gian (ms) mỗi lần gửi dữ liệu

int detectValue = 512;

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
//    pinMode(OUT_PIN_4, OUTPUT);
//    digitalWrite(OUT_PIN_4, LOW);
//    delay(2000);
//    pinMode(OUT_PIN_4, INPUT_PULLUP);
//    delay(2000);
    
    int sta1 = digitalRead(IN_PIN_1);
    int sta2 = digitalRead(IN_PIN_2);
    int sta3 = digitalRead(IN_PIN_3);
    int sta4 = digitalRead(IN_PIN_4);

    state = (sta1<<3) | (sta2<<2) | (sta3<<1) | sta4;
//    Serial.println(state);
    
    if (state!=0xF) {
        delay(5);
        sta1 = digitalRead(IN_PIN_1);
        sta2 = digitalRead(IN_PIN_2);
        sta3 = digitalRead(IN_PIN_3);
        sta4 = digitalRead(IN_PIN_4);

        if (state!=0xF) {

            if (state!=previousState) {
                if (sta1==LOW) {
                    pinMode(OUT_PIN_1, OUTPUT);
                    digitalWrite(OUT_PIN_1, LOW);    
                }
                if (sta2==LOW) {
                    pinMode(OUT_PIN_2, OUTPUT);
                    digitalWrite(OUT_PIN_2, LOW);
                }            
                if (sta3==LOW) {
                    pinMode(OUT_PIN_3, OUTPUT);
                    digitalWrite(OUT_PIN_3, LOW);
                }
                if (sta4==LOW) {
                    pinMode(OUT_PIN_4, OUTPUT);
                    digitalWrite(OUT_PIN_4, LOW);
                }

                StaticJsonBuffer<200> jsonBuffer;
                JsonObject& root = jsonBuffer.createObject();
                root["bc01"] = sta1;
                root["bc02"] = sta2;
                root["bc03"] = sta3;
                root["bc04"] = sta4;

                mySerial.print('arduino');
                mySerial.print('\r');               
                root.printTo(mySerial);
                mySerial.print('\r'); 
                delay(10);    
                previousState = state;
            }
        }
        else {
            readPin();
        }
    }
    else {
        readPin();
    }
}

void readPin() {
    pinMode(OUT_PIN_1, INPUT);
    pinMode(OUT_PIN_2, INPUT);
    pinMode(OUT_PIN_3, INPUT);
    pinMode(OUT_PIN_4, INPUT);
    int val1 = digitalRead(OUT_PIN_1);
    int val2 = digitalRead(OUT_PIN_2);
    int val3 = digitalRead(OUT_PIN_3);
    int val4 = digitalRead(OUT_PIN_4);

    previousState = (val1<<3) | (val2<<2) | (val3<<1) | val4;

    if (val1==LOW || val2==LOW || val3==LOW || val4==LOW) {
        if ((unsigned long)(millis() - startTime) >= endTime) {
            StaticJsonBuffer<200> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            
            root["bc01"] = val1;
            root["bc02"] = val2;
            root["bc03"] = val3;
            root["bc04"] = val4;    

            mySerial.print('arduino');
            mySerial.print('\r');               
            root.printTo(mySerial);
            mySerial.print('\r');

            startTime = millis();
        }
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


