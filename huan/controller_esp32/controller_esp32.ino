/*
    This sketch sends data via HTTP GET requests to data.sparkfun.com service.

    You need to get streamId and privateKey at data.sparkfun.com and paste them
    below. Or just customize this script to talk to other HTTP servers.

*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>

const int rs = 5, en = 18, d4 = 19, d5 = 21, d6 = 22, d7 = 23;
LiquidCrystal lcd2(rs, en, d4, d5, d6, d7);

#define PIN_ADC3 34
#define PIN_ADC2 35
#define PIN_DETECT 13

//const char* SSID = "dlink_DWR-710_CA58";
//const char* PASSWORD = "RXbgc83862";
const char* SSID = "Phong Ky Thuat";
const char* PASSWORD = "123456789";

const char* MQTT_SERVER = "113.161.21.15";
const int MQTT_PORT = 1884;
const char *MQTT_USER = "esp32";
const char *MQTT_PASS = "mtt@23377";
const char* PUBLISH_TOPIC = "controller";   // Topic điều khiển xe detector
const char* SUBSCRIBE_TOPIC = "detector";   // Topic nhận cảnh báo phát hiện từ detector
const char* LWT_TOPIC = "controller/lwt";   // lwt

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
WiFiClient client;
PubSubClient mqttClient(client);

byte mac[6];
char MAC_ADDRESS[50];
char MQTT_CLIENT[64];

// controller variable 
long updown = 2048;     // <1000: down, >3000: up
long leftright = 2048;  // <1000: right, >3000: left
int updownPreviousState = 1;    // 0: down, 1: Đứng yên, 2: up
int updownCurrentState = 1;

int leftrightPreviousState = 1; // 0: right, 1: Đứng yên, 2: left
int leftrightCurrentState = 1;

void setup()
{
    // Serial and port mode
    Serial.begin(115200);
    delay(10);
    pinMode(PIN_DETECT, OUTPUT);
    digitalWrite(PIN_DETECT, HIGH);

    // Wifi
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
    sprintf(MQTT_CLIENT, "ESP32-%02X%02X%02X%02X%02X%02X", mac[3], mac[4], mac[5], mac[0], mac[1], mac[2]);
    Serial.println(MQTT_CLIENT);

    // mqtt
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);

    // lcd
//    lcd.begin(12, 14);                      // initialize the lcd
//    lcd.backlight();
//    lcd.setCursor(0, 0);
//    lcd.print("DETECT MINE");
//    lcd.setCursor(0, 1);
//    lcd.print("OFF");

    //lcd2
    lcd2.begin(16, 2);
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd2.print("Start....");
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

    for (int i = 0; i < length; i++) {
        //Serial.print((char)payload[i]);
        jsonStr += (char)payload[i];
    }

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonStr);

    String detect = root["detect"];
    float latitude = root["gps"][0];
    float longitude = root["gps"][1];
    Serial.println(latitude, 6);
    Serial.println(longitude, 6);
    if (detect == "ON")
    {
        Serial.println("detect: ON");
        digitalWrite(PIN_DETECT, LOW);
        displayLCD2(detect, latitude, longitude);
        //        delay(100);
    }
    if (detect == "OFF") {
        Serial.println("detect: OFF");
        digitalWrite(PIN_DETECT, HIGH);
        displayLCD2(detect, latitude, longitude);
        //        delay(100);
    }
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
        if (mqttClient.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASS, LWT_TOPIC, 1, false, "offline")) {
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
        // Nếu sau '60s' không kết nối được thì restart
        if ((unsigned long)(millis() - currentMqttMillis) >= 60000)
        {
            ESP.restart();
        }
    }
}

void loop()
{

    // Kiểm tra kết nối với Mqtt Server
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();  //

    analogSetWidth(12);
    analogSetPinAttenuation(PIN_ADC3, ADC_6db);
    updown = analogRead(PIN_ADC3);
    updown = 2048;
    updownCurrentState = getState(updown);

    //    Serial.println(updownCurrentState);

    analogSetWidth(12);
    analogSetPinAttenuation(PIN_ADC2, ADC_6db);
    leftright = analogRead(PIN_ADC2);
    leftright = 2048;
    leftrightCurrentState = getState(leftright);

    
    
    if (updownCurrentState != updownPreviousState || leftrightCurrentState != leftrightPreviousState) {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["updown"] = updown;
        root["leftright"] = leftright;

        char JSONmessageBuffer[100];
        root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        mqttClient.publish(PUBLISH_TOPIC, JSONmessageBuffer);

        updownPreviousState = updownCurrentState;
        leftrightPreviousState = leftrightCurrentState;
        //        Serial.println(updownPreviousState);
    }

    delay(10);
}

int getState(long data) {
    if (data < 1000) {
        return 0;   // Down, right
    } else if (data > 1000 && data < 3000) {
        return 1;   // Đứng yên
    } else if (data > 3000) {
        return 2;   // Up, left
    }
}


void displayLCD2(String state, float lat, float lon)
{
 
    lcd2.clear();
    if (state == "ON") // On
    {
        lcd2.setCursor(0, 0);
        lcd2.print(lat, 6);
        lcd2.setCursor(13, 0);
        lcd2.print("on");
        
        lcd2.setCursor(0, 1);
        lcd2.print(lon, 6);
        lcd2.setCursor(13, 1);
        lcd2.print("on");
    }
    else if (state == "OFF") // Off
    {
        lcd2.setCursor(0, 0);
        lcd2.print(lat, 6);
        lcd2.setCursor(13, 0);
        lcd2.print("off");
        lcd2.setCursor(0, 1);
        lcd2.print(lon, 6);
        lcd2.setCursor(13, 1);
        lcd2.print("off");
    }
}
