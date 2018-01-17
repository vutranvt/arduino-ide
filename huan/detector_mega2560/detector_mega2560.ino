#define ARDUINOJSON_USE_DOUBLE 1
#include <String.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
#include <TinyGPS.h>
#include <math.h>

#define UP_PIN 4        // D1
#define DOWN_PIN 5      // D2
#define LEFT_PIN 6      // D5
#define RIGHT_PIN 7     // D6
#define DETECT_PIN 8    // D7
#define ALARM_PIN 9     // SD3


int state = LOW;   // state mức cao: chưa phát hiện
int previousState = HIGH;

/* variables */
unsigned long rxPulse = 10; //xung (ms) nhận các pin ngõ vào
unsigned long currentMillis = 0;
unsigned long sendMillis = 2 * 100;   // thời gian (ms) mỗi lần gửi dữ liệu

const byte RX = 50;          // Chân 3 được dùng làm chân RX
const byte TX = 51;          // Chân 2 được dùng làm chân TX

const byte gpsRX = 11;          
const byte gpsTX = 12;

TinyGPS gps;
SoftwareSerial ss(gpsRX, gpsTX);

SoftwareSerial mySerial = SoftwareSerial(RX, TX);

SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// int red = 4, blue = 5; // led đỏ đối vô digital 4, led xanh đối vô digital 5

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

void setup() {

    pinMode(UP_PIN, OUTPUT);
    pinMode(DOWN_PIN, OUTPUT);
    pinMode(RIGHT_PIN, OUTPUT);
    pinMode(LEFT_PIN, OUTPUT);
    pinMode(DETECT_PIN, INPUT);
    pinMode(ALARM_PIN, OUTPUT);

    digitalWrite(UP_PIN, HIGH);
    digitalWrite(DOWN_PIN, HIGH);
    digitalWrite(LEFT_PIN, HIGH);
    digitalWrite(RIGHT_PIN, HIGH);
    digitalWrite(ALARM_PIN, HIGH);

    // Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
    Serial.begin(115200);
    // Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
    mySerial.begin(9600);

    ss.begin(9600);

    // Một số hàm trong thư viện Serial Command
    // sCmd.addCommand(esp_arduino, rxResponseEsp);  //Khi có lệnh thì thực hiện hàm "rxResponseEsp"
    sCmd.addDefaultHandler(defaultCommand);

}
    
void loop() {
    sCmd.readSerial();

    state = pinIsPressed(); // pin đã được nhấn

    // PIN_DETECT -> HIGH
    if (state == B11) {
        digitalWrite(ALARM_PIN, LOW);     // Bật đèn cảnh báo
        previousState = state;

        delay(2000);

        
        if ((unsigned long)(millis() - currentMillis) > sendMillis) // send data in "sendMillis"
        {
            StaticJsonBuffer<200> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            root["detect"] = "ON";
            
            bool newData = false;
            float flat, flon;
            unsigned long age, date, time, chars = 0;
            
            for (unsigned long start = millis(); millis() - start < 1000;)
            {
              while (ss.available())
              {
                char c = ss.read();
                // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
                if (gps.encode(c)) // Did a new valid sentence come in?
                  newData = true;
              }
            }
            if (newData) {
              gps.f_get_position(&flat, &flon, &age);
              Serial.print(flat, 6);
              Serial.print(flon, 6);
              Serial.println();
              if (flat != TinyGPS::GPS_INVALID_F_ANGLE && flon != TinyGPS::GPS_INVALID_F_ANGLE) {
                JsonArray& gps = root.createNestedArray("gps");
//                float latitude = truncate(flat, 6);
//                float longitude = truncate(flon, 6);
                gps.add(flat);
                gps.add(flon);

              }
            }
            smartdelay(0);

            // Print to software serial (esp wifi)
            mySerial.print('arduino');
            mySerial.print('\r');               
            root.printTo(mySerial);
            mySerial.print('\r'); 

            root.printTo(Serial);   // in ra trên cổng Serial
            Serial.println();

            currentMillis = millis();
        }
    }
    // PIN_DETECT -> LOW
    else if (state == B00) {
        if (state != previousState) {
            digitalWrite(ALARM_PIN, HIGH);    // Tắt đèn cảnh báo 
            previousState = state;

            StaticJsonBuffer<200> jsonBuffer;
            JsonObject& root = jsonBuffer.createObject();
            root["detect"] = "OFF";

            bool newData = false;
            float flat, flon;
            unsigned long age, date, time, chars = 0;
            
            for (unsigned long start = millis(); millis() - start < 1000;)
            {
              while (ss.available())
              {
                char c = ss.read();
                // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
                if (gps.encode(c)) // Did a new valid sentence come in?
                  newData = true;
              }
            }
            if (newData) {
                gps.f_get_position(&flat, &flon, &age);
//                Serial.print(flat, 6);
//                Serial.print(flon, 6);
//                Serial.println();
                if (flat != TinyGPS::GPS_INVALID_F_ANGLE && flon != TinyGPS::GPS_INVALID_F_ANGLE)
                {
                    JsonArray &gps = root.createNestedArray("gps");
//                    float latitude = truncate(flat, 6);
//                    float longitude = truncate(flon, 6);
                    gps.add(flat);
                    gps.add(flon);
                }
            }
            smartdelay(0);           

            // Print to software serial (esp wifi)
            mySerial.print('arduino');
            mySerial.print('\r');
            root.printTo(mySerial);
            mySerial.print('\r'); 

            root.printTo(Serial); // Print to computer's serial
            Serial.println();
        }
    }
}


void rxCommand(String command) {
    char *json = sCmd.next();

    Serial.println(json);
    
}
void defaultCommand(String command) {
    char *json = sCmd.next();

    Serial.println(json);
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    unsigned int updownValue = root["updown"];
    unsigned int leftrightValue = root["leftright"];
    Serial.print("updown value: ");
    Serial.print(updownValue);
    Serial.print("----leftright value: ");
    Serial.println(leftrightValue);

    if (updownValue < 1000) {
        // set "down"
        digitalWrite(UP_PIN, HIGH);
        digitalWrite(DOWN_PIN, LOW);  //--> low
        Serial.print("DOWN");
    } else if (updownValue > 1000 && updownValue < 3000) {
        // set "đứng yên"
        digitalWrite(UP_PIN, HIGH);
        digitalWrite(DOWN_PIN, HIGH);
    } else if (updownValue > 3000) {
        // set "up"
        digitalWrite(UP_PIN, LOW);    //--> low
        digitalWrite(DOWN_PIN, HIGH);
        Serial.print("UP");
    }

    if (leftrightValue < 1000) {
        // set "RIGHT"
        digitalWrite(LEFT_PIN, HIGH);
        digitalWrite(RIGHT_PIN, LOW); //--> low
        Serial.println("-RIGHT");
    } else if (leftrightValue > 1000 && leftrightValue < 3000) {
        // set "đứng yên"
        digitalWrite(LEFT_PIN, HIGH);
        digitalWrite(RIGHT_PIN, HIGH);
        Serial.println("-");
    } else if (leftrightValue > 3000) {
        // set "LEFT"
        digitalWrite(LEFT_PIN, LOW);  //--> low
        digitalWrite(RIGHT_PIN, HIGH);
        Serial.println("-LEFT");
    }

}

int pinIsPressed()
{
    unsigned long current = millis();
    int result = 0;
    int readPin = digitalRead(DETECT_PIN);

    while (1)
    {
        if (digitalRead(DETECT_PIN) != readPin)
        { //Nhận fake state của pin
            return result = (readPin << 1) | (~readPin & 0x1);
        }
        else if ((unsigned long)(millis() - current) >= rxPulse)
        { //Nhận state của pin trong rxPulse (ms)
            return result = (readPin << 1) | readPin;
        }
    }
}

static void smartdelay(unsigned long ms)
{
    unsigned long start = millis();
    do
    {
        while (ss.available())
            gps.encode(ss.read());
    } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
    if (val == invalid)
    {
        while (len-- > 1)
            Serial.print('*');
        Serial.print(' ');
    }
    else
    {
        Serial.print(val, prec);
        int vi = abs((int)val);
        int flen = prec + (val < 0.0 ? 2 : 1); // . and -
        flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
        for (int i = flen; i < len; ++i)
            Serial.print(' ');
    }
    smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
    char sz[32];
    if (val == invalid)
        strcpy(sz, "*******");
    else
        sprintf(sz, "%ld", val);
    sz[len] = 0;
    for (int i = strlen(sz); i < len; ++i)
        sz[i] = ' ';
    if (len > 0)
        sz[len - 1] = ' ';
    Serial.print(sz);
    smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long age;
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
    if (age == TinyGPS::GPS_INVALID_AGE)
        Serial.print("********** ******** ");
    else
    {
        char sz[32];
        sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
                month, day, year, hour, minute, second);
        Serial.print(sz);
    }
    print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
    smartdelay(0);
}

static void print_str(const char *str, int len)
{
    int slen = strlen(str);
    for (int i = 0; i < len; ++i)
        Serial.print(i < slen ? str[i] : ' ');
    smartdelay(0);
}

void getGps()
{
    float flat, flon;
    unsigned long age, date, time, chars = 0;
    unsigned short sentences = 0, failed = 0;
    static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

    smartdelay(1000);
    
    //  print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
    //  print_int(gps.hdop(), TinyGPS::GPS_INVALID_HDOP, 5);
    gps.f_get_position(&flat, &flon, &age);

    
    // print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
    // print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
    //  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
    //  print_date(gps);
    //  print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);
    //  print_float(gps.f_course(), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
    //  print_float(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2);
    //  print_str(gps.f_course() == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(gps.f_course()), 6);
    //  print_int(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long)TinyGPS::distance_between(flat, flon, LONDON_LAT, LONDON_LON) / 1000, 0xFFFFFFFF, 9);
    //  print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? TinyGPS::GPS_INVALID_F_ANGLE : TinyGPS::course_to(flat, flon, LONDON_LAT, LONDON_LON), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
    //  print_str(flat == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(TinyGPS::course_to(flat, flon, LONDON_LAT, LONDON_LON)), 6);

    gps.stats(&chars, &sentences, &failed);
    //  print_int(chars, 0xFFFFFFFF, 6);
    //  print_int(sentences, 0xFFFFFFFF, 10);
    //  print_int(failed, 0xFFFFFFFF, 9);
    Serial.println();
}

float truncate(float number, int precision)
{
    float prec = pow(10, precision);
    long integerPart = (int)number;
    float fractionalPart = number - integerPart;
    fractionalPart *= prec;
//    Serial.println(fractionalPart);
    long fractPart = (long)fractionalPart;
//    Serial.println(fractPart);
    fractionalPart = (float)(integerPart) + (float)(fractPart) / prec;
//    Serial.println(fractionalPart);
    return fractionalPart;
}
