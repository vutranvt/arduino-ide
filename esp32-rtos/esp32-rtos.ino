
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Update.h>
#include "user_config.h"

WiFiClient client;
PubSubClient mqttClient(client);

#define TEST_PIN 14 // D5

volatile unsigned int counter = 0;
volatile unsigned int counter1 = 0;
volatile unsigned int counter2 = 0;
volatile unsigned int counter3 = 0;
volatile double ampAdc1 = 0;
volatile double ampAdc2 = 0;
volatile double ampAdc3 = 0;

uint64_t chipid;  
byte mac[6];
char MAC_ADDRESS[50];

// Variables to validate
// response from S3
int contentLength = 0;
bool isValidContentType = false;

// update firmware config
String host = "113.161.21.15"; // Host => bucket-name.s3.region.amazonaws.com
int port = 8267; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin = "/current-v1.1.ino.esp32.bin"; // bin file name with a slash in front.
String FIRMWARE_VERSION = "1.0";    //

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
    return header.substring(strlen(headerName.c_str()));
}

// OTA Logic
void execOTA() {
    Serial.println("Connecting to: " + String(host));
    // Connect to S3
    if (client.connect(host.c_str(), port)) {
        // Connection Succeed.
        // Fecthing the bin
        Serial.println("Fetching Bin: " + String(bin));

        // Get the contents of the bin file
        client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Cache-Control: no-cache\r\n" +
                     "Connection: close\r\n\r\n");

        // Check what is being sent
        //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
        //                 "Host: " + host + "\r\n" +
        //                 "Cache-Control: no-cache\r\n" +
        //                 "Connection: close\r\n\r\n");

        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) {
                Serial.println("Client Timeout !");
                client.stop();
                return;
            }
        }
        // Once the response is available,
        // check stuff

        /*
            Response Structure
            HTTP/1.1 200 OK
            x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
            x-amz-request-id: 2D56B47560B764EC
            Date: Wed, 14 Jun 2017 03:33:59 GMT
            Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
            ETag: "d2afebbaaebc38cd669ce36727152af9"
            Accept-Ranges: bytes
            Content-Type: application/octet-stream
            Content-Length: 357280
            Server: AmazonS3

            {{BIN FILE CONTENTS}}

        */
        while (client.available()) {
            // read line till /n
            String line = client.readStringUntil('\n');
            // remove space, to check if the line is end of headers
            line.trim();

            // if the the line is empty,
            // this is end of headers
            // break the while and feed the
            // remaining `client` to the
            // Update.writeStream();
            if (!line.length()) {
                //headers ended
                break; // and get the OTA started
            }

            // Check if the HTTP Response is 200
            // else break and Exit Update
            if (line.startsWith("HTTP/1.1")) {
                if (line.indexOf("200") < 0) {
                    Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
                    break;
                }
            }

            // extract headers here
            // Start with content length
            if (line.startsWith("Content-Length: ")) {
                contentLength = atoi((getHeaderValue(line, "Content-Length: ")).c_str());
                Serial.println("Got " + String(contentLength) + " bytes from server");
            }

            // Next, the content type
            if (line.startsWith("Content-Type: ")) {
                String contentType = getHeaderValue(line, "Content-Type: ");
                Serial.println("Got " + contentType + " payload.");
                if (contentType == "application/octet-stream") {
                    isValidContentType = true;
                }
            }
        }
    } else {
        // Connect to S3 failed
        // May be try?
        // Probably a choppy network?
        Serial.println("Connection to " + String(host) + " failed. Please check your setup");
        // retry??
        // execOTA();
    }

    // Check what is the contentLength and if content type is `application/octet-stream`
    Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

    // check contentLength and content type
    if (contentLength && isValidContentType) {
        // Check if there is enough to OTA Update
        bool canBegin = Update.begin(contentLength);

        // If yes, begin
        if (canBegin) {
            Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
            // No activity would appear on the Serial monitor
            // So be patient. This may take 2 - 5mins to complete
            size_t written = Update.writeStream(client);

            if (written == contentLength) {
                Serial.println("Written : " + String(written) + " successfully");
            } else {
                Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
                // retry??
                // execOTA();
            }

            if (Update.end()) {
                Serial.println("OTA done!");
                if (Update.isFinished()) {
                    Serial.println("Update successfully completed. Rebooting.");
                    ESP.restart();
                } else {
                    Serial.println("Update not finished? Something went wrong!");
                }
            } else {
                Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            }
        } else {
            // not enough space to begin OTA
            // Understand the partitions and
            // space availability
            Serial.println("Not enough space to begin OTA");
            client.flush();
        }
    } else {
        Serial.println("There was no content in the response");
        client.flush();
    }
}

void setup() {

    Serial.begin(112500);
    delay(1000);

    pinMode(TEST_PIN, INPUT_PULLUP);

    Serial.print("Firmware Version: ");
    Serial.println(FIRMWARE_VERSION);

    // We start by connecting to a WiFi network
    // connectWifi();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
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
    
    // config Mqtt
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);

    // config task list
    xTaskCreate(
        taskOne,          /* Task function. */
        "TaskOne",        /* String with name of task. */
        10000,            /* Stack size in words. */
        NULL,             /* Parameter passed as input of the task */
        1,                /* Priority of the task. */
        NULL);            /* Task handle. */

    xTaskCreate(
        taskTwo,          /* Task function. */
        "TaskTwo",        /* String with name of task. */
        10000,            /* Stack size in words. */
        NULL,             /* Parameter passed as input of the task */
        1,                /* Priority of the task. */
        NULL);            /* Task handle. */

}

void connectWifi() {
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
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

    String command = root["cmd"];
    if (command == "update") {
        Serial.println("updating...");
        execOTA();
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(MQTT_CLIENTID, MQTT_USER, MQTT_PASS, TOPIC_LWT, 0, false, MESSAGE_LWT)) {
            Serial.println("connected");

            const size_t bufferSize = JSON_OBJECT_SIZE(5)+JSON_OBJECT_SIZE(3)+JSON_OBJECT_SIZE(7); //+JSON_OBJECT_SIZE(3);
            DynamicJsonBuffer jsonBuffer(bufferSize);

//            StaticJsonBuffer<400> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            root["center"] = CENTER;
            root["type"] = TYPE;
            root["location"] = LOCATION;
            root["firmwareVersion"] = FIRMWARE_VERSION;
            root["macAddress"] = MAC_ADDRESS;
//            root["macAddress"] = "FW";

            JsonObject& updateInfo = root.createNestedObject("updateInfo");
            updateInfo["server"] = host;
            updateInfo["port"] = port;
            updateInfo["fileName"] = bin;

            JsonObject& deviceConfig = root.createNestedObject("deviceConfig");
            deviceConfig["calibrationRatio"] = CALIB_RATIO;
            deviceConfig["TIRatio"] = TI_RATIO;
            deviceConfig["adcZero1"] = ADC_ZERO_1;
            deviceConfig["adcZero2"] = ADC_ZERO_2;
            deviceConfig["adcZero3"] = ADC_ZERO_3;

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
            delay(5000);
        }
    }
}

void loop() {
    // Kiểm tra kết nối Wifi
    while (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    // Kiểm tra kết nối với Mqtt Server
    if (!client.connected()) {
        reconnect();
    }

    mqttClient.loop();

    unsigned long startTime = millis();

    while ((unsigned long)(millis() - startTime) < 5000) //sample for 1 Sec
    {
        double data1, data2, data3;

        data1 = ceilf(ampAdc1);
        ampAdc1 = 0;
        counter1 = 0;
        data2 = ceilf(ampAdc2);
        ampAdc2 = 0;
        counter2 = 0;
        data3 = ceilf(ampAdc3);
        ampAdc3 = 0;
        counter3 = 0;

        const size_t bufferSize = JSON_OBJECT_SIZE(3);
        DynamicJsonBuffer jsonBuffer(bufferSize);

    //    StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["value1"] = data1;
        root["value2"] = data2;
        root["value3"] = data3;

        char JSONmessageBuffer[100];
        root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        mqttClient.publish(TOPIC_PUBLISH, JSONmessageBuffer);
    }
    
}

void taskOne( void * parameter )
{

    unsigned long adcValue1 = 0;
    unsigned long adcValue2 = 0;
    unsigned long adcValue3 = 0;
    unsigned long sampleTime = 0;
    unsigned long maxValue = 0;
    double avr1 = 0;
    double avr2 = 0;
    double avr3 = 0;
    double result1 = 0;
    double result2 = 0;
    double result3 = 0;
    double temp1;
    double temp2;
    double temp3;
    //unsigned long startTime = millis();

    // initialize ADC
    analogSetWidth(12);
    analogSetPinAttenuation(PIN_ADC1, ADC_6db);
    analogSetPinAttenuation(PIN_ADC2, ADC_6db);
    analogSetPinAttenuation(PIN_ADC3, ADC_6db);

    while (1)
    {
        sampleTime = 0;
        avr1 = 0;
        avr2 = 0;
        avr3 = 0;
        maxValue = 0;
        while (sampleTime < 2000)
        {
#if defined(RAW_ADC)
            adcValue1 = analogRead(PIN_ADC1);
            adcValue2 = analogRead(PIN_ADC2);
            adcValue3 = analogRead(PIN_ADC3);
#else
            adcValue1 = analogRead(PIN_ADC1) - ADC_ZERO_1;
            avr1 = avr1 + (double)(adcValue1 * adcValue1);

            adcValue2 = analogRead(PIN_ADC2) - ADC_ZERO_2;
            avr2 = avr2 + (double)(adcValue2 * adcValue2);
            
            adcValue3 = analogRead(PIN_ADC3) - ADC_ZERO_3;
            avr3 = avr3 + (double)(adcValue3 * adcValue3);
            /*  if(adcValue1 > maxValue){
                maxValue = adcValue1;
                }*/
#endif
            sampleTime++;
        }

        temp1 = sqrt(avr1 / (double)sampleTime);
        temp2 = sqrt(avr2 / (double)sampleTime);
        temp3 = sqrt(avr3 / (double)sampleTime);
        // printf("adc1 = %.2f\n",temp);

        result1 = sqrt(avr1 / (double)sampleTime) * (CALIB_RATIO / 4096.0); // Lay gia tri trung binh
        result2 = sqrt(avr2 / (double)sampleTime) * (CALIB_RATIO / 4096.0); // Lay gia tri trung binh
        result3 = sqrt(avr3 / (double)sampleTime) * (CALIB_RATIO / 4096.0); // Lay gia tri trung binh

        result1 = result1 - 0.10;
        result2 = result2 - 0.10;
        result3 = result3 - 0.10;

        if (result1 < 0.20)     result1 = 0;
        else    result1 = result1 + 0.10;

        if (result2 < 0.20)     result2 = 0;
        else    result2 = result2 + 0.10;

        if (result3 < 0.20)     result3 = 0;
        else    result3 = result3 + 0.10;

        result1 = result1 * TI_RATIO;
        result2 = result2 * TI_RATIO;
        result3 = result3 * TI_RATIO;

#if defined(RAW_ADC)
        ampAdc1 = (ampAdc1 + adcValue1) / (counter + 1);
        ampAdc2 = (ampAdc2 + adcValue2) / (counter + 1);
        ampAdc3 = (ampAdc3 + adcValue3) / (counter + 1);
#else
        ampAdc1 = (ampAdc1 + result1) / (counter + 1);
        ampAdc2 = (ampAdc2 + result2) / (counter + 1);
        ampAdc3 = (ampAdc3 + result3) / (counter + 1);
#endif
        counter = 1;

        delay(100);
    }

    vTaskDelete( NULL );
}

void taskTwo( void * parameter)
{


    vTaskDelete( NULL );
}

