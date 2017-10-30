/*
    Basic ESP8266 MQTT example

    This sketch demonstrates the capabilities of the pubsub library in combination
    with the ESP8266 board/library.

    It connects to an MQTT server then:
    - publishes "hello world" to the topic "outTopic" every two seconds
    - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
    - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

    It will reconnect to the server if the connection is lost using a blocking
    reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
    achieve the same result without blocking the main loop.

    To install the ESP8266 board, (using Arduino 1.6.4+):
    - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
    - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
    - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

extern "C" {
#include "user_interface.h"
}

// #define UP_PIN 5        // D1
// #define DOWN_PIN 4      // D2
// #define LEFT_PIN 14     // D5
// #define RIGHT_PIN 12    // D6
// #define DETECT_PIN 13   // D7
// #define DETECT_PIN 9   // SD2
// #define DETECT_PIN2 10   // SD3
#define OUT_PIN 14   // D1
#define IN_PIN 12   // D2



// config device command
const char* server_arduino = "server_arduino";
const char* esp_arduino = "esp_arduino";

int state = HIGH;   // state mức cao: chưa phát hiện
int previousState = 0;    

unsigned long startTime = 0;
unsigned long endTime = 2 * 1000;

int detectValue = 512;

String chipIdEsp = "";
String mac_address = "";

const byte RX = 4;
const byte TX = 5;

SoftwareSerial mySerial(RX, TX, false, 256);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// Update these with values suitable for your network.
const char* SSID = "Phong Ky Thuat";
const char* PASSWORD = "123456789";
const char* MQTT_SERVER = "113.161.21.15";

const char* PUBLISH_TOPIC = "miner";
const char* SUBSCRIBE_TOPIC = "controller";

const char* get_chipID = "get_chipID";
uint32_t chipID;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

    // pinMode(UP_PIN, OUTPUT);
    // pinMode(DOWN_PIN, OUTPUT);
    // pinMode(RIGHT_PIN, OUTPUT);
    // pinMode(LEFT_PIN, OUTPUT);
    // pinMode(DETECT_PIN, INPUT_PULLUP);
    // pinMode(DETECT_PIN2, OUTPUT);

    // digitalWrite(UP_PIN, HIGH);
    // digitalWrite(DOWN_PIN, HIGH);
    // digitalWrite(LEFT_PIN, HIGH);
    // digitalWrite(RIGHT_PIN, HIGH);
    // digitalWrite(DETECT_PIN2, HIGH);

    pinMode(OUT_PIN, OUTPUT);
    digitalWrite(OUT_PIN, HIGH);

    pinMode(IN_PIN, INPUT_PULLUP);
    

    Serial.begin(115200);
    mySerial.begin(57600); //Bật software serial để giao tiếp với Arduino, nhớ để baudrate trùng với software serial trên mạch arduino
    
    delay(10);

    // We start by connecting to a WiFi network
    // connectWifi();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    client.setServer(MQTT_SERVER, 1884);
    client.setCallback(callback);

    // chipID = ESP.getChipId();
    // sCmd.addDefaultHandler(defaultCommand);

    // Một số hàm trong thư viện Serial Command
    // sCmd.addCommand(server_arduino, requestResponse);   //Khi có lệnh thì thực hiện hàm "rxRequestServer"
    // sCmd.addCommand(esp_arduino, rxRequest);        //Khi có lệnh thì thực hiện hàm "rxResponseEsp"
    // sCmd.addDefaultHandler(defaultCommand);

}

void connectWifi() {
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);
    delay(500);
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Not connect");
    } else {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }

}

void callback(char* topic, byte* payload, unsigned int length) {
    String jsonStr = "";
    //    Serial.print("Message arrived [");
    //    Serial.print(topic);
    //    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        //Serial.print((char)payload[i]);
        jsonStr += (char)payload[i];
    }
    
     StaticJsonBuffer<200> jsonBuffer;
     JsonObject& root = jsonBuffer.parseObject(jsonStr);

     unsigned int updownValue = root["updown"];
     unsigned int leftrightValue = root["leftright"];
     Serial.print("updown value: ");
     Serial.print(updownValue);
     Serial.print("----leftright value: ");
     Serial.println(leftrightValue);
      
      mySerial.print('esp8266');
      mySerial.print('\r');
      
//      root.printTo(mySerial);
       mySerial.print(jsonStr);
//      mySerial.print(payload);
       mySerial.print('\r');

//       root.printTo(Serial);
      Serial.println(jsonStr);

}



void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266Game", "esp32", "mtt@23377")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish(PUBLISH_TOPIC, "hello world");
            // ... and resubscribe
            client.subscribe(SUBSCRIBE_TOPIC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
void loop() {
    // Kiểm tra kết nối Wifi
    while (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    // Kiểm tra kết nối Mqtt Server
    if (!client.connected()) {
        reconnect();
    }
    client.loop();  //

    // sCmd.readSerial();  //

    int status = digitalRead(IN_PIN);

    if(status==LOW) {
        pinMode(OUT_PIN, OUTPUT);
        digitalWrite(OUT_PIN, LOW); 
        state = LOW;
        delay(40);
    } 
    else {
//        if(state == LOW) {
            pinMode(OUT_PIN, INPUT);
//        }
        state = HIGH;
        int val = digitalRead(OUT_PIN);
        Serial.println(val);
        delay(10);
    }
}

void defaultCommand(String command) {
    char *json = sCmd.next();

    Serial.println(json);
    
    client.publish(PUBLISH_TOPIC, json);
}




