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

// config device command
const char* server_arduino = "server_arduino";
const char* esp_arduino = "esp_arduino";

// config request command
#define chip_id_request "chip_id_request"
#define mac_address_request "mac_address_request"
#define data "data"

#define UPDOWN_PIN 5
#define LEFTRIGHT_PIN 4

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

const char* PUBLISH_TOPIC = "outTopic";
const char* SUBSCRIBE_TOPIC = "miner";

const char* get_chipID = "get_chipID";
uint32_t chipID;

WiFiClient espClient;
PubSubClient client(espClient);

//StaticJsonBuffer<200> jsonBuffer;

long lastMsg = 0;
char msg[50];
int value = 0;



void setup() {

    pinMode(UPDOWN_PIN, OUTPUT);     
    pinMode(LEFTRIGHT_PIN, OUTPUT);    
     
    Serial.begin(115200);
    mySerial.begin(115200); //Bật software serial để giao tiếp với Arduino, nhớ để baudrate trùng với software serial trên mạch arduino
    delay(10);

    setup_wifi();
    client.setServer(MQTT_SERVER, 1884);
    client.setCallback(callback);

    chipID = ESP.getChipId();
    // sCmd.addDefaultHandler(defaultCommand);

    // Một số hàm trong thư viện Serial Command
    sCmd.addCommand(server_arduino, requestResponse);   //Khi có lệnh thì thực hiện hàm "rxRequestServer"
//    sCmd.addCommand(esp_arduino, rxRequest);        //Khi có lệnh thì thực hiện hàm "rxResponseEsp"
//sCmd.addDefaultHandler(defaultCommand);


}

void setup_wifi() {

    delay(10);
    // We start by connecting to a WiFi network
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
//    Serial.println(jsonStr);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonStr); 

    unsigned int updownValue = root["updown"];
    unsigned int leftrightValue = root["leftright"];
    Serial.print("updown value: ");
    Serial.println(updownValue);
    Serial.print("leftright value: ");
    Serial.println(leftrightValue);

    if(updownValue > 3000) {
        digitalWrite(UPDOWN_PIN, 1);  // up
    }else if (updownValue < 1000) {
        digitalWrite(UPDOWN_PIN, 0);  // down
    }

    if(leftrightValue > 3000) {
        digitalWrite(LEFTRIGHT_PIN, 1);  // left     
    }else if (leftrightValue < 1000) {
        digitalWrite(LEFTRIGHT_PIN, 0);  // right     
    }
    
    
//    mySerial.print("ARDUINO");
//    mySerial.print('\r');
//    mySerial.print(jsonStr);
//    mySerial.print('\r');

}



void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266Client", "esp32", "mtt@23377")) {
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

    if (!client.connected()) {
        reconnect();
    }
    client.loop();  //

//    sCmd.readSerial();  //

}

/*
    json_string_from_server = {
    "jsonCmd" : "read_max_value/read_average_value/get_chipID"
    }
*/

void defaultCommand(char *command) {
    char *json = sCmd.next();
    client.publish("PUBLISH_TOPIC", json);
}

void requestResponse() {
//    Serial.println(deviceCmd);
    Serial.println(esp_arduino);
    char *json = sCmd.next(); //Chỉ cần một dòng này để đọc tham số nhận đươc
    Serial.println(json);
    StaticJsonBuffer<200> jsonBuffer; //tạo Buffer json có khả năng chứa tối đa 200 ký tự
    JsonObject& root = jsonBuffer.parseObject(json);//đặt một biến root mang kiểu json

    String requestCmd = root["request"];
    String response = root["response"];

    if(requestCmd==mac_address_request){
        //txResponse(deviceCmd, requestCmd, mac_address);
    }
    if(response==data){
        client.publish("PUBLISH_TOPIC", json);
    }
}



