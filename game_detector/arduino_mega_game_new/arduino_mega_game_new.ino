#include <String.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
#include "bmt_config.h"

#define IN_PIN_1 23 
#define IN_PIN_2 25 
#define IN_PIN_3 27 
#define IN_PIN_4 29 
#define IN_PIN_5 31
#define IN_PIN_6 33 
#define IN_PIN_7 35
#define IN_PIN_8 37

#define TEST_INPIN 4
#define TEST_OUTPIN 5

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

unsigned long rxPulse = 12;	//xung (ms) nhận các pin ngõ vào
int state = 0xFF;

int fakePressedCounter = 0;
int realPressedCounter = 0;
bool pressEnable = true;
bool line = true;

const byte RX = 2;          // Chân 2 được dùng làm chân RX
const byte TX = 3;          // Chân 3 được dùng làm chân TX
SoftwareSerial mySerial = SoftwareSerial(RX, TX);

SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

void setup() {
  
    delay(2000);
    pinMode(IN_PIN_1, INPUT);
    pinMode(IN_PIN_2, INPUT);
    pinMode(IN_PIN_3, INPUT);
    pinMode(IN_PIN_4, INPUT);
    pinMode(IN_PIN_5, INPUT);
    pinMode(IN_PIN_6, INPUT);
    pinMode(IN_PIN_7, INPUT);
    pinMode(IN_PIN_8, INPUT);

    pinMode(TEST_INPIN, INPUT);

    pinMode(TEST_OUTPIN, OUTPUT);
    digitalWrite(TEST_OUTPIN, LOW);
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
    // testPinPulse();
//    txOutPin();
    


    int pinState = pinIsPressed();
//    Serial.println(pinState);
    //low -> low
    if (pinState == 0) {
//        Serial.print("pressEnable: ");
//        Serial.println(pressEnable);
//        Serial.print("line: ");
//        Serial.println(line);
        if (pressEnable == true && line == true) {
            sendData();
            pressEnable = false;
            fakePressedCounter = 0;
        }
    }
    //low -> high
    else if (pinState == 1) {
        fakePressedCounter++;
//        Serial.println(pinState);
        if (fakePressedCounter == 200) {
            if (line == true) {
                line = false;
                Serial.print("line: ");
                Serial.println(line);
            }
            fakePressedCounter = 0;
            realPressedCounter = 0;
        }
    }
    //high -> low
    else if (pinState == 2) {
    }
    //high -> high
    else if (pinState == 3) {
        realPressedCounter++;
        if ( realPressedCounter == 300) {
            if (line == false) {
                line = true;
                Serial.print("line: ");
                Serial.println(line);
            }
            realPressedCounter = 0;
        }
        fakePressedCounter = 0;
        pressEnable = true;
    }
    
}

void sendData () {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    root[bc01] = "on";

    mySerial.print('arduino');
    mySerial.print('\r');               
    root.printTo(mySerial);
    mySerial.print('\r'); 
    
    Serial.println("LOW");

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


int pinIsPressed () {
    unsigned long currentMillis = millis();
    int in1 = digitalRead(IN_PIN_1);
//    delay(rxPulse);
    while (1) {
        if (digitalRead(IN_PIN_1) != in1) {
            return (in1 << 1) | (~in1 & 0x1);
        } 
        else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {
            return (in1 <<1) | in1; 
        }
        
    }
}

void testPinPulse () {
  unsigned long temp1 = millis();
  if (digitalRead(IN_PIN_1)==HIGH) {
    while (digitalRead(IN_PIN_1)==HIGH) {
      
    }
    Serial.print("P1:");
    Serial.println(millis() - temp1);
  }
  unsigned long temp2 = millis();
  if (digitalRead(IN_PIN_2)==LOW) {
    while (digitalRead(IN_PIN_2)==LOW) {
      
    }
    Serial.print("P2:");
    Serial.println(millis() - temp2);
  }
  unsigned long temp3 = millis();
  if (digitalRead(IN_PIN_3)==LOW) {
    while (digitalRead(IN_PIN_3)==LOW) {
      
    }
    Serial.print("P3:");
    Serial.println(millis() - temp3);
  }
  unsigned long temp4 = millis();
  if (digitalRead(IN_PIN_4)==LOW) {
    while (digitalRead(IN_PIN_4)==LOW) {
      
    }
    Serial.print("P4:");
    Serial.println(millis() - temp4);
  }
  unsigned long temp5 = millis();
  if (digitalRead(IN_PIN_5)==LOW) {
    while (digitalRead(IN_PIN_5)==LOW) {
      
    }
    Serial.print("P5:");
    Serial.println(millis() - temp5);
  }
  unsigned long temp6 = millis();
  if (digitalRead(IN_PIN_6)==LOW) {
    while (digitalRead(IN_PIN_6)==LOW) {
      
    }
    Serial.print("P6:");
    Serial.println(millis() - temp6);
  }
  unsigned long temp7 = millis();
  if (digitalRead(IN_PIN_7)==LOW) {
    while (digitalRead(IN_PIN_7)==LOW) {
      
    }
    Serial.print("P7:");
    Serial.println(millis() - temp7);
  }
  unsigned long temp8 = millis();
  if (digitalRead(IN_PIN_8)==LOW) {
    while (digitalRead(IN_PIN_8)==LOW) {
      
    }
    Serial.print("P8:");
    Serial.println(millis() - temp8);
  }
}

