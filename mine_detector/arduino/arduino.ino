#include <String.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command


#define UP_PIN 4        // D1
#define DOWN_PIN 5      // D2
#define LEFT_PIN 6     // D5
#define RIGHT_PIN 7    // D6
#define DETECT_PIN 8   // D7
#define DETECT_PIN2 9   // SD3


int state = LOW;   // state mức cao: chưa phát hiện
int previousState = HIGH;    

unsigned long startTime = 0;
unsigned long endTime = 2 * 100;   // thời gian (ms) mỗi lần gửi dữ liệu

int detectValue = 512;

// config device command
const char* server_arduino = "server_arduino";
const char* esp_arduino = "esp_arduino";

// config request command
#define chip_id_request "chip_id_request"
#define mac_address_request "mac_address_request"

String chipIdEsp = "";
String mac_address = "";

const byte RX = 10;          // Chân 3 được dùng làm chân RX
const byte TX = 2;          // Chân 2 được dùng làm chân TX

SoftwareSerial mySerial = SoftwareSerial(RX, TX);

SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// int red = 4, blue = 5; // led đỏ đối vô digital 4, led xanh đối vô digital 5

void setup() {

    pinMode(UP_PIN, OUTPUT);
    pinMode(DOWN_PIN, OUTPUT);
    pinMode(RIGHT_PIN, OUTPUT);
    pinMode(LEFT_PIN, OUTPUT);
    pinMode(DETECT_PIN, INPUT);
    pinMode(DETECT_PIN2, OUTPUT);

    digitalWrite(UP_PIN, HIGH);
    digitalWrite(DOWN_PIN, HIGH);
    digitalWrite(LEFT_PIN, HIGH);
    digitalWrite(RIGHT_PIN, HIGH);
    digitalWrite(DETECT_PIN2, HIGH);

    // Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
    Serial.begin(115200);
    // Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
    mySerial.begin(57600);

    // Một số hàm trong thư viện Serial Command
    // sCmd.addCommand(server_arduino, rxRequestServer);   //Khi có lệnh thì thực hiện hàm "configReadValue"
    // sCmd.addCommand(esp_arduino, rxResponseEsp);  //Khi có lệnh thì thực hiện hàm "rxResponseEsp"
    sCmd.addDefaultHandler(defaultCommand);
//     sCmd.addCommand('esp8266', rxCommand);

    // delay(3000);
    // txRequestEsp(mac_address_request);
}
    
void loop() {
    sCmd.readSerial();

    state = digitalRead(DETECT_PIN);

     
    if (state == HIGH) {
        delay(10);
        if (digitalRead(DETECT_PIN) == HIGH) {
            digitalWrite(DETECT_PIN2, LOW);  
            previousState = HIGH;
            if ((unsigned long)(millis() - startTime) > endTime) // send data in "endTime"
            {
                StaticJsonBuffer<200> jsonBuffer;
                JsonObject& root = jsonBuffer.createObject();
                root["detect"] = "ON";

                mySerial.print('arduino');
                mySerial.print('\r');               
                root.printTo(mySerial);
                mySerial.print('\r'); 

                root.printTo(Serial);   // in ra trên cổng Serial

                startTime = millis();
            }
        }
    }
    else if (state == LOW) {
        delay(10);
        if (digitalRead(DETECT_PIN) == LOW && state != previousState) {
            digitalWrite(DETECT_PIN2, HIGH);  
            previousState = LOW;
            StaticJsonBuffer<200> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            root["detect"] = "OFF";
           
            mySerial.print('arduino');
            mySerial.print('\r');
            root.printTo(mySerial);
            mySerial.print('\r'); 

            root.printTo(Serial);
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

    if (updownValue < 1000) {
        // set "down"
        digitalWrite(UP_PIN, HIGH);
        digitalWrite(DOWN_PIN, LOW);
        Serial.print("DOWN");
    } else if (updownValue > 1000 && updownValue < 3000) {
        // set "đứng yên"
        digitalWrite(UP_PIN, HIGH);
        digitalWrite(DOWN_PIN, HIGH);
    } else if (updownValue > 3000) {
        // set "up"
        digitalWrite(UP_PIN, LOW);
        digitalWrite(DOWN_PIN, HIGH);
        Serial.print("UP");
    }

    if (leftrightValue < 1000) {
        // set "RIGHT"
        digitalWrite(LEFT_PIN, HIGH);
        digitalWrite(RIGHT_PIN, LOW);
        Serial.println("-RIGHT");
    } else if (leftrightValue > 1000 && leftrightValue < 3000) {
        // set "đứng yên"
        digitalWrite(LEFT_PIN, HIGH);
        digitalWrite(RIGHT_PIN, HIGH);
        Serial.println("-");
    } else if (leftrightValue > 3000) {
        // set "LEFT"
        digitalWrite(LEFT_PIN, LOW);
        digitalWrite(RIGHT_PIN, HIGH);
        Serial.println("-LEFT");
    }

}


