#include <Wire.h>
#include <RtcDateTime.h>
#include <RtcDS1307.h>
#include <RtcDS3231.h>
#include <RtcTemperature.h>
#include <RtcUtility.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

extern "C" {
#include "user_interface.h"
}

byte mac[6];
char MAC_ADDRESS[50];
char MQTT_CLIENT[64];
unsigned long currentPublishMillis = 0;

const byte RX = 4;  // D2
const byte TX = 5;  // D1

SoftwareSerial mySerial(RX, TX, false, 256);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// Update these with values suitable for your network.
//const char* SSID = "dlink_DWR-710_CA58";
//const char* PASSWORD = "RXbgc83862";
const char* SSID = "Phong Ky Thuat";
const char* PASSWORD = "123456789";
const char* MQTT_SERVER = "113.161.21.15";
const int MQTT_PORT = 1884;
const char *MQTT_USER = "esp32";
const char *MQTT_PASS = "mtt@23377";

const char* PUBLISH_TOPIC = "ducanh/alarm";
const char* SUBSCRIBE_TOPIC = "ducanh/alarm";

WiFiClient client;
PubSubClient mqttClient(client);

void setup() {

    Serial.begin(115200);
    mySerial.begin(9600); //Bật software serial để giao tiếp với Arduino, nhớ để baudrate trùng với software serial trên mạch arduino
    
    delay(10);

    // We start by connecting to a WiFi network
    // connectWifi();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);

    unsigned long currentWifiMillis = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
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
    // sCmd.addCommand(esp_arduino, rxRequest);        //Khi có lệnh thì thực hiện hàm "rxResponseEsp"
    sCmd.addDefaultHandler(defaultCommand);

}

void connectWifi()
{
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        // Nếu trong 60s không có kết nối thì restart
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

//       root.printTo(Serial);
      Serial.println(jsonStr);

}


void reconnect() {
    unsigned long currentMqttMillis = millis();
    // Loop until we're reconnected
    while (!mqttClient.connected()) {
        // Kiểm tra kết nối Wifi
        while (WiFi.status() != WL_CONNECTED)
        {
            connectWifi();
        }
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS, "detector/lwt", 1, false, "offline"))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            mqttClient.publish(PUBLISH_TOPIC, "hello world");
            // ... and resubscribe
            mqttClient.subscribe(SUBSCRIBE_TOPIC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 2 seconds");
            // Wait 5 seconds before retrying
            delay(2000);
        }
        // Nếu sau '60s' không kết nối được thì restart
        if ((unsigned long)(millis() - currentMqttMillis) >= 60000)
        {
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
    
    mqttClient.publish(PUBLISH_TOPIC, json);
}




