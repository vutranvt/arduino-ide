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


/* variables */
unsigned long rxPulse = 12;	//xung (ms) nhận các pin ngõ vào
int fakePressedCounter[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int realPressedCounter[8] = {0, 0, 0, 0, 0, 0, 0, 0};
bool pressEnable[8] = {true, true, true, true, true, true, true, true}; //true: cho phép nhận pin, fasle: ko nhận pin
bool line[8] = {true, true, true, true, true, true, true, true};    //Trạng thái Line: true->connected, false->disconnected

/* software serial */
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

    int disconectLine = 0;
    int connectLine = 0;
    int data = 0;
    int lineState = 0;
    int pin[8];

    pinIsPressed(pin);
//    for(int i=0; i<8; i++){
//        Serial.println(pin[i]);
//    }
  
    for(int i=0; i<8; i++){
        //low-low (trạng thái thật: LOW)
        if (pin[i] == B00) {
            //Nếu line connected và pressEnable: true thì gửi data
            if (pressEnable[i] == true && line[i] == true) {
                // sendData();
                pressEnable[i] = false;
                fakePressedCounter[i] = 0;
                data = data | (1<<i);   //Nếu data != 0: có pin được kích xuống mức thấp
            }       
        }
        //high-high (Trạng thái thật: HIGH)
        else if (pin[i] == B11) {
            realPressedCounter[i]++;
            fakePressedCounter[i] = 0;
            pressEnable[i] = true;
        } 
        //low-high (Trạng thái giả: pin disconnected)
        else if (pin[i] == B01) {
            fakePressedCounter[i]++;    //đếm trạng thái giả 
        } 
        //high-low (trạng thái giả: pin disconnected)
        else if (pin[i] == B10) {
        } 
    }
    for(int i=0; i<8; i++){
        //Kiểm tra biến đếm trạng thái giả: Nếu đúng thì gửi: Line disconnected 
        if (fakePressedCounter[i] == 200) { 
            if (line[i] == true) {
                line[i] = false;
                Serial.print("line[i]: ");
                Serial.println(line[i]);
                disconectLine |= (1 << i);  //Nếu bit disconnetedLine != 0: có line disconnected
            }
            fakePressedCounter[i] = 0;
            realPressedCounter[i] = 0;
        }
        //Kiểm tra biến đếm trạng thái thật: Nếu đúng thì gửi: Line connected
        if ( realPressedCounter[i] == 300) {
            if (line[i] == false) {
                line[i] = true;
                Serial.print("line[i]: ");
                Serial.println(line[i]);
                connectLine |= (1 << i);    //Nếu bit connetedLine != 0: có line connected
            }
            realPressedCounter[i] = 0;
        }
    
    }
    //Nếu có pin kích LOW hoặc line disconnected/connected: Gửi data to server 
    if (data != 0 || disconectLine !=0 || connectLine != 0) {
        sendData(data, disconectLine, connectLine);
    }
    
}

void sendData (int stateData, int disState, int conState) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    
    if (stateData != 0) {
        for(int i=0; i<8; i++){
            if (stateData & (1 << i)) {
                root[bc[i]] = "on";
                Serial.print(i);
                Serial.println(": on");
            }
        }    
    }
    if (disState != 0 || conState) {
        for(int i=0; i<8; i++){
            if (disState & (1 << i)) {
                root[bc[i]] = "disconnected";
                Serial.print(i);
                Serial.println(": disconnected");
            }
            if (conState & (1 << i)) {
                root[bc[i]] = "connected";
                Serial.print(i);
                Serial.println(": connected");
            }
        }    
    }

    mySerial.print('arduino');
    mySerial.print('\r');               
    root.printTo(mySerial);
    mySerial.print('\r'); 

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


void pinIsPressed (int res[]) {
    unsigned long currentMillis = millis();
    
    int in[8];
    in[0] = digitalRead(IN_PIN_1);
    in[1] = digitalRead(IN_PIN_2);
    in[2] = digitalRead(IN_PIN_3);
    in[3] = digitalRead(IN_PIN_4);
    in[4] = digitalRead(IN_PIN_5);
    in[5] = digitalRead(IN_PIN_6);
    in[6] = digitalRead(IN_PIN_7);
    in[7] = digitalRead(IN_PIN_8);

    int in1 = digitalRead(IN_PIN_1);

    bool sta[8] = {true, true, true, true, true, true, true, true}; //false: đã nhận pin level, true: chưa nhận pin level

    while (1) {
        //Nếu pin thay đổi trạng thái
        if (sta[0] == true) {
            if (digitalRead(IN_PIN_1) != in[0]) {       //Nhận fake state của pin 
                res[0] = (in[0] << 1) | (~in[0] & 0x1);
                sta[0] = false;
            } 
            else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {    //Nhận state của pin trong rxPulse (ms)
                res[0] = (in[0] <<1) | in[0]; 
                sta[0] = false;
            }    
        }
        if (sta[1] == true) {
            if (digitalRead(IN_PIN_2) != in[1]) {
                res[1] = (in[1] << 1) | (~in[1] & 0x1);
                sta[1] = false;
            } 
            else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {
                res[1] = (in[1] <<1) | in[1]; 
                sta[1] = false;
            }    
        }        
        if (sta[2] == true) {
            if (digitalRead(IN_PIN_3) != in[2]) {
                res[2] = (in[2] << 1) | (~in[2] & 0x1);
                sta[2] = false;
            } 
            else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {
                res[2] = (in[2] <<1) | in[2]; 
                sta[2] = false;
            }    
        }
        if (sta[3] == true) {
            if (digitalRead(IN_PIN_4) != in[3]) {
                res[3] = (in[3] << 1) | (~in[3] & 0x1);
                sta[3] = false;
            } 
            else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {
                res[3] = (in[3] <<1) | in[3]; 
                sta[3] = false;
            }    
        }
        if (sta[4] == true) {
            if (digitalRead(IN_PIN_5) != in[4]) {
                res[4] = (in[4] << 1) | (~in[4] & 0x1);
                sta[4] = false;
            } 
            else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {
                res[4] = (in[4] <<1) | in[4]; 
                sta[4] = false;
            }    
        }
        if (sta[5] == true) {
            if (digitalRead(IN_PIN_6) != in[5]) {
                res[5] = (in[5] << 1) | (~in[5] & 0x1);
                sta[5] = false;
            } 
            else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {
                res[5] = (in[5] <<1) | in[5]; 
                sta[5] = false;
            }    
        }
        if (sta[6] == true) {
            if (digitalRead(IN_PIN_7) != in[6]) {
                res[6] = (in[6] << 1) | (~in[6] & 0x1);
                sta[6] = false;
            } 
            else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {
                res[6] = (in[6] <<1) | in[6]; 
                sta[6] = false;
            }    
        }
        if (sta[7] == true) {
            if (digitalRead(IN_PIN_8) != in[7]) {
                res[7] = (in[7] << 1) | (~in[7] & 0x1);
                sta[7] = false;
            } 
            else if ((unsigned long)(millis() - currentMillis) >= rxPulse) {
                res[7] = (in[7] <<1) | in[7]; 
                sta[7] = false;
            }    
        }
        if (!(sta[0] || sta[1] || sta[2] || sta[3] || sta[4] || sta[5] || sta[6] || sta[7])) {
            break;
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

