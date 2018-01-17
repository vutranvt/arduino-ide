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

const char* PUBLISH_TOPIC = "detector";
const char* SUBSCRIBE_TOPIC = "controller";

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
        if ((unsigned long)(millis() - currentWifiMillis) >= 60000)
        {
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

    unsigned long currentWifiMillis = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        // Nếu trong 60s không có kết nối thì restart
        if ((unsigned long)(millis() - currentWifiMillis) >= 60000)
        {
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




