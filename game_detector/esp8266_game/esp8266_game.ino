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
#include "user_config.h"

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

int state = HIGH;   // state mức cao: chưa phát hiện
int previousState = 0;    

unsigned long mqttTime = 30 * 1000;
unsigned long wifiTime = 60 * 1000;
unsigned long startSendTime = 0;
unsigned long sendTime = 30 * 1000;

const byte RX = 4;
const byte TX = 5;

SoftwareSerial mySerial(RX, TX, false, 256);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// Update these with values suitable for your network.
const char* SSID = "Phong Ky Thuat";
const char* PASSWORD = "123456789";
const char* MQTT_SERVER = "113.161.21.15";

const char* PUBLISH_TOPIC = "bmt/maybanca";
const char* SUBSCRIBE_TOPIC = "bmt/maybanca";
const char* LWT_TOPIC = "bmt/maybanca/lwt";

WiFiClient client;
PubSubClient mqttClient(client);

void setup() {

    pinMode(OUT_PIN, OUTPUT);
    digitalWrite(OUT_PIN, HIGH);

    pinMode(IN_PIN, INPUT_PULLUP);
    

    Serial.begin(115200);
    mySerial.begin(38400); //Bật software serial để giao tiếp với Arduino, nhớ để baudrate trùng với software serial trên mạch arduino
    
    delay(10);

    // We start by connecting to a WiFi network
    // connectWifi();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);
    
    unsigned long startWifiTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if ((unsigned long)(millis() - startWifiTime) >= wifiTime) {
            ESP.restart();
        }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    mqttClient.setServer(MQTT_SERVER, 1884);
    // mqttClient.setCallback(callback);
    startSendTime = millis();

    // Một số hàm trong thư viện Serial Command
    sCmd.addDefaultHandler(defaultCommand);

}

void connectWifi() {
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);
    
    unsigned long startWifiTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        // Nếu trong 20s không có kết nối thì restart 
        if ((unsigned long)(millis() - startWifiTime)>=wifiTime) {
            ESP.restart();
        }
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

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
    unsigned long startMqttTime = millis();
    // Loop until we're reconnected
    while (!mqttClient.connected()) {
        // Kiểm tra kết nối Wifi
        while (WiFi.status() != WL_CONNECTED) {
            connectWifi();
        }
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect("ESP8266Game", "esp32", "mtt@23377", LWT_TOPIC, 0, false, "offline")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            mqttClient.publish(PUBLISH_TOPIC, "hello world");
            // ... and resubscribe
            mqttClient.subscribe(SUBSCRIBE_TOPIC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(2000);
        }
        // Nếu sau 'mqttTime' không kết nối được thì restart 
        if ((unsigned long)(millis() - startMqttTime)>=mqttTime) {
            ESP.restart();
        }
    }
}

void loop() {
    // Kiểm tra kết nối Mqtt Server
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();  //

    sCmd.readSerial();  //
}

void defaultCommand(String command) {
    char *json = sCmd.next();

    Serial.println(json);
    
    if (mqttClient.publish(PUBLISH_TOPIC, json)) {
        startSendTime = millis();
    } else if ((unsigned long)(millis() - startSendTime) >= sendTime) {
        ESP.restart();
    }
}




