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


int state = HIGH;   // state mức cao: chưa phát hiện
int previousState = HIGH;    

unsigned long startTime = 0;
unsigned long endTime = 2 * 1000;   // thời gian (ms) mỗi lần gửi dữ liệu

int detectValue = 512;

SoftwareSerial mySerial = SoftwareSerial(RX, TX);

SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

void setup() {
  
    pinMode(OUT_PIN_4, OUTPUT);
    digitalWrite(OUT_PIN_4, HIGH);
    pinMode(IN_PIN_4, INPUT_PULLUP);

    pinMode(OUT_PIN_5, OUTPUT);
    digitalWrite(OUT_PIN_5, HIGH);
    pinMode(IN_PIN_5, INPUT_PULLUP);

    // Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
    Serial.begin(115200);
    // Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
    mySerial.begin(57600);

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
    
    int status1 = digitalRead(IN_PIN_1);
    int status2 = digitalRead(IN_PIN_2);
    int status3 = digitalRead(IN_PIN_3);
    int status4 = digitalRead(IN_PIN_4);

    if(status==LOW) {
        pinMode(OUT_PIN_4, OUTPUT);
        digitalWrite(OUT_PIN_4, LOW); 
        state = LOW;
        delay(40);
    } 
    else {
        pinMode(OUT_PIN_4, INPUT);
        state = HIGH;
        int val = digitalRead(OUT_PIN_4);
        Serial.println(val);
        delay(50);
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


