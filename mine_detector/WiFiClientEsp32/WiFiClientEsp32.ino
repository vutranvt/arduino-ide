/*
    This sketch sends data via HTTP GET requests to data.sparkfun.com service.

    You need to get streamId and privateKey at data.sparkfun.com and paste them
    below. Or just customize this script to talk to other HTTP servers.

*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

#define PIN_ADC3 34
#define PIN_ADC2 35
#define PIN_DETECT 13

int detectState = HIGH;

const char* SSID = "dlink_DWR-710_CA58";
const char* PASSWORD = "RXbgc83862";
const char* MQTT_SERVER = "113.161.21.15";

const char* PUBLISH_TOPIC = "controller";
const char* SUBSCRIBE_TOPIC = "miner";

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];
//long updownPrevious = 2048;
long updown = 2048;
long leftright = 2048;
int updownIntervalPrevious = 1;
int updownIntervalNow = 1;

int leftrightIntervalPrevious = 1;
int leftrightIntervalNow = 1;

void setup()
{
    Serial.begin(115200);
    delay(10);
    pinMode(PIN_DETECT, OUTPUT);
    digitalWrite(PIN_DETECT, HIGH);

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

    lcd.begin(12, 14);                      // initialize the lcd
    // Print a message to the LCD.
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("DETECT MINE");
    lcd.setCursor(0, 1);
    lcd.print("OFF");
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

    for (int i = 0; i < length; i++) {
        //Serial.print((char)payload[i]);
        jsonStr += (char)payload[i];
    }

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonStr);

    String detect = root["detect"];
    if (detect == "ON") {
        Serial.println("detect: ON");
        detectState = LOW;
        digitalWrite(PIN_DETECT, LOW);
        displayLCD(detectState);
        //        delay(100);
    }
    if (detect == "OFF") {
        Serial.println("detect: OFF");
        detectState = HIGH;
        digitalWrite(PIN_DETECT, HIGH);
        displayLCD(detectState);
        //        delay(100);
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32Client", "esp32", "mtt@23377", "miner/lwt", 0, false, "offline")) {
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

void loop()
{
//    displayLCD(detectState);
    // Kiểm tra kết nối Wifi
    while (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    // Kiểm tra kết nối với Mqtt Server
    if (!client.connected()) {
        reconnect();
    }
    client.loop();  //

    long now = millis();

    analogSetWidth(12);
    analogSetPinAttenuation(PIN_ADC3, ADC_6db);
    updown = analogRead(PIN_ADC3);
    updownIntervalNow = getInterval(updown);

    //    Serial.println(updownIntervalNow);

    analogSetWidth(12);
    analogSetPinAttenuation(PIN_ADC2, ADC_6db);
    leftright = analogRead(PIN_ADC2);
    leftrightIntervalNow = getInterval(leftright);



    if (updownIntervalNow != updownIntervalPrevious || leftrightIntervalNow != leftrightIntervalPrevious) {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["updown"] = updown;
        root["leftright"] = leftright;

        char JSONmessageBuffer[100];
        root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        client.publish(PUBLISH_TOPIC, JSONmessageBuffer);

        updownIntervalPrevious = updownIntervalNow;
        leftrightIntervalPrevious = leftrightIntervalNow;
        //        Serial.println(updownIntervalPrevious);
    }

    delay(10);
}

int getInterval(long data) {
    if (data < 1000) {
        return 0;
    } else if (data > 1000 && data < 3000) {
        return 1;
    } else if (data > 3000) {
        return 2;
    }
}

void displayLCD (int state) {
//  lcd.begin(12, 14);
//  Serial.println("Print LCD ");
  lcd.clear();
  if (state == LOW) {
    lcd.setCursor(0, 0);
    lcd.print("DETECT MINE");
    lcd.setCursor(0, 1);
    lcd.print("ON");
  } else if (state == HIGH) {
    lcd.setCursor(0, 0);
    lcd.print("DETECT MINE");
    lcd.setCursor(0, 1);
    lcd.print("OFF");
  }
}

