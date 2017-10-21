/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define PIN_ADC3 34

const char* ssid = "dlink_DWR-710_CA58";
const char* PASSWORD = "RXbgc83862";
const char* MQTT_SERVER = "113.161.21.15";

const char* PUBLISH_TOPIC = "outTopic";
const char* SUBSCRIBE_TOPIC = "miner";

WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network

    setup_wifi();

    client.setServer(MQTT_SERVER, 1884);
//    client.setCallback(callback);
}

void setup_wifi() {

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
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

    unsigned int updownValue = root["updown"];
    Serial.print("updown value: ");
    Serial.println(updownValue);

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
    if (!client.connected()) {
        reconnect();
    }
    client.loop();  //
    
    long now = millis();
//    if (now - lastMsg > 2000) {
//        lastMsg = now;
//        ++value;
        analogSetWidth(12);
        analogSetPinAttenuation(PIN_ADC3, ADC_6db);
        long updown = analogRead(PIN_ADC3);
        
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["updown"] = updown;
        
        char JSONmessageBuffer[100];
        root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        
        client.publish("miner", JSONmessageBuffer);
          delay(200);
//    }
}

