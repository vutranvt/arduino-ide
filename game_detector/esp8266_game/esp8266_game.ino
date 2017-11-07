#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "bmt_config.h"

extern "C" {
#include "user_interface.h"
}

#define TEST_IN_PIN 14  // D5
#define TEST_OUT_PIN 12 // D6

byte mac[6];
char MAC_ADDRESS[50];
char MQTT_CLIENT[64];   
unsigned long currentPublishMillis = 0;

const byte RX = 4;
const byte TX = 5;

SoftwareSerial mySerial(RX, TX, false, 256);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// Update these with values suitable for your network.
const char* MQTT_SERVER = "113.161.21.15";
const int MQTT_PORT = 1884;
const char* MQTT_USER = "esp32";
const char* MQTT_PASS = "mtt@23377";

WiFiClient client;
PubSubClient mqttClient(client);

// update firmware config
String host = "113.161.21.15"; // Host => bucket-name.s3.region.amazonaws.com
int port = 4000; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin_v10 = "/esp8266_game_v1.0.ino.nodemcu.bin";
String bin_v11 = "/esp8266_game_v1.1.ino.nodemcu.bin";
String bin_v12 = "/esp8266_game_v1.2.ino.nodemcu.bin";
String FIRMWARE_VERSION = "1.1";    //// config 

void updateFirmware(String binVersion) {

    // Kiểm tra kết nối Wifi
    while (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    Serial.println();
    Serial.print("Updating Firmware...");

    t_httpUpdate_return ret = ESPhttpUpdate.update(String("http://" + host + ":" + (String)port + binVersion));
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
//            ESP.restart();
            break;
    }
}

void setup() {
    pinMode(TEST_OUT_PIN, OUTPUT);
    digitalWrite(TEST_OUT_PIN, HIGH);

    pinMode(TEST_IN_PIN, INPUT_PULLUP);
    
    Serial.begin(115200);
    mySerial.begin(57600); //Để baudrate trùng với software serial trên mạch arduino
    
    delay(10);

    Serial.println();
    Serial.print("Firmware Version: ");
    Serial.println(FIRMWARE_VERSION);
    // We start by connecting to a WiFi network
    // connectWifi();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);
    
    unsigned long currentWifiMillis = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if ((unsigned long)(millis() - currentWifiMillis) >= 60000) {
            ESP.restart();
        }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // get chip id (về bản chất là mac address)
    WiFi.macAddress(mac);
    sprintf(MAC_ADDRESS, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.println(MAC_ADDRESS);
    sprintf(MQTT_CLIENT, "ESP8266-%02X%02X%02X%02X%02X%02X", mac[3], mac[4], mac[5], mac[0], mac[1], mac[2]);
    Serial.println(MQTT_CLIENT);

    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
    currentPublishMillis = millis();

    // Một số hàm trong thư viện Serial Command
    sCmd.addDefaultHandler(defaultCommand);

}

void connectWifi() {
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);
    
    unsigned long currentWifiMillis = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        // Nếu trong 60s không có kết nối thì restart 
        if ((unsigned long)(millis() - currentWifiMillis) >= 60000) {
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

    String rxData = root["cmd"];

    mySerial.print('esp8266');
    mySerial.print('\r');
    mySerial.print(jsonStr);
    mySerial.print('\r');

    if (rxData=="update_v1.0") {
        updateFirmware(bin_v10);
    } else if (rxData=="update_v1.1") {
        updateFirmware(bin_v11);
    } else if (rxData=="update_v1.2") {
        updateFirmware(bin_v12);
    }
    Serial.print("subscribe: ");
    Serial.println(jsonStr);
}



void reconnect() {
    unsigned long currentMqttMillis = millis();
    // Loop until we're reconnected
    while (!mqttClient.connected()) {
        // Kiểm tra kết nối Wifi
        while (WiFi.status() != WL_CONNECTED) {
            connectWifi();
        }
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS, TOPIC_LWT, 0, false, "offline")) {
            Serial.println("connected");
            const size_t bufferSize = JSON_OBJECT_SIZE(5)+JSON_OBJECT_SIZE(5)+JSON_OBJECT_SIZE(7); //+JSON_OBJECT_SIZE(3);
            DynamicJsonBuffer jsonBuffer(bufferSize);

//            StaticJsonBuffer<400> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            root["location"] = LOCATION;
            root["type"] = TYPE;
            root["name"] = NAME;
            root["firmwareVersion"] = FIRMWARE_VERSION;
            root["macAddress"] = MAC_ADDRESS;
//            root["macAddress"] = "FW";

            JsonObject& updateInfo = root.createNestedObject("updateInfo");
            updateInfo["server"] = host;
            updateInfo["port"] = port;
            updateInfo["v10"] = bin_v10;
            updateInfo["v11"] = bin_v11;
            updateInfo["v12"] = bin_v12;

            // JsonObject& deviceConfig = root.createNestedObject("deviceConfig");

            char JSONmessageBuffer[500];
            root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

            // Once connected, publish an announcement...
            mqttClient.publish(TOPIC_PUBLISH, JSONmessageBuffer);

            Serial.println(JSONmessageBuffer);
            // ... and resubscribe
            mqttClient.subscribe(TOPIC_SUBSCRIBE);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(2000);
        }
        // Nếu sau '30s' không kết nối được thì restart 
        if ((unsigned long)(millis() - currentMqttMillis) >= 30000) {
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

//    Serial.println(json);
    
    if (mqttClient.publish(TOPIC_PUBLISH, json)) {
        currentPublishMillis = millis();
    } 
    //Nếu sau '30s' không gửi được thì restart device
    else if ((unsigned long)(millis() - currentPublishMillis) >= 30000) {
        ESP.restart();
    }
}




