#include <Arduino_FreeRTOS.h>

#define PIN_ADC1 A0
#define PIN_ADC2 A1
#define PIN_ADC3 A2

const unsigned long sampleTime = 30000;  //Thoi gian lay mau: 1000 mili giay
const uint32_t  averageTimeNum = 26;  //Thời gian lấy dữ liệu dòng điện
const int adc_zero = 512;

double ampAdc1 = 0;
double ampAdc2 = 0;
double ampAdc3 = 0;
int counter1 = 0;
int counter2 = 0;
int counter3 = 0;

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void TaskAnalogRead1( void *pvParameters );
void TaskAnalogRead2( void *pvParameters );
void TaskAnalogRead3( void *pvParameters );

// the setup function runs once when you press reset or power the board
void setup() {

    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);

    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
    }

    xTaskCreate(
        TaskAnalogRead1
        ,  (const portCHAR *) "AnalogRead1"
        ,  128  // Stack size
        ,  NULL
        ,  1  // Priority
        ,  NULL );

    xTaskCreate(
        TaskAnalogRead2
        ,  (const portCHAR *) "AnalogRead2"
        ,  128  // Stack size
        ,  NULL
        ,  1  // Priority
        ,  NULL );

    xTaskCreate(
        TaskAnalogRead3
        ,  (const portCHAR *) "AnalogRead3"
        ,  128  // Stack size
        ,  NULL
        ,  1  // Priority
        ,  NULL );
    // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
    // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/


void TaskAnalogRead1(void *pvParameters)  // This is a task.
{
    (void) pvParameters;

    double result = 0;
    double temp = 0;
    double average = 0;
    unsigned long counter = 0;
    long analogValue;              //value read from the sensor

    unsigned long startTime = millis();
    while (1) {
        counter = 0;
        average = 0;
        while ((unsigned long)(millis() - startTime) < 1000) //sample for 1 Sec
        {
            analogValue = analogRead(PIN_ADC1) - adc_zero;
            Serial.println(analogValue);
            average = average + (double)(analogValue * analogValue);
            counter++;
        }
        // temp = sqrt(average/(double)counter);
        // Serial.println(temp);

        result = sqrt(average / (double)counter) * (75 / 1024.0); // Lay gia tri trung binh
        result = result - 0.10; //

        if (result < 0.20)
            result = 0;
        else
            result = result + 0.10;

        result = result * 10; // TI radio = 10
        ampAdc1 = (ampAdc1 + result) / (counter1 + 1);  //
        counter1 = 1;

        delay(10);
    }
}
void TaskAnalogRead2(void *pvParameters)  // This is a task.
{
    (void) pvParameters;

    double result = 0;
    double temp = 0;
    double average = 0;
    unsigned long counter = 0;
    long analogValue;              //value read from the sensor

    unsigned long startTime = millis();
    while (1) {
        counter = 0;
        average = 0;
        while ((unsigned long)(millis() - startTime) < 1000) //sample for 1 Sec
        {
            analogValue = analogRead(PIN_ADC2) - adc_zero;
            Serial.println(analogValue);
            average = average + (double)(analogValue * analogValue);
            counter++;
        }
        // temp = sqrt(average/(double)counter);
        // Serial.println(temp);

        result = sqrt(average / (double)counter) * (75 / 1024.0); // Lay gia tri trung binh
        result = result - 0.10; //

        if (result < 0.20)
            result = 0;
        else
            result = result + 0.10;

        result = result * 10; // TI radio = 10
        ampAdc2 = (ampAdc2 + result) / (counter2 + 1);  //
        counter2 = 1;

        delay(10);
    }
}

void TaskAnalogRead3(void *pvParameters)  // This is a task.
{
    (void) pvParameters;

    double result = 0;
    double temp = 0;
    double average = 0;
    unsigned long counter = 0;
    long analogValue;              //value read from the sensor

    unsigned long startTime = millis();
    while (1) {
        counter = 0;
        average = 0;
        while ((unsigned long)(millis() - startTime) < 1000) //sample for 1 Sec
        {
            analogValue = analogRead(PIN_ADC3) - adc_zero;
            Serial.println(analogValue);
            average = average + (double)(analogValue * analogValue);
            counter++;
        }
        // temp = sqrt(average/(double)counter);
        // Serial.println(temp);

        result = sqrt(average / (double)counter) * (75 / 1024.0); // Lay gia tri trung binh
        result = result - 0.10; //

        if (result < 0.20)
            result = 0;
        else
            result = result + 0.10;

        result = result * 10; // TI radio = 10
        ampAdc3 = (ampAdc3 + result) / (counter3 + 1);  //
        counter3 = 1;

        delay(10);
    }
}
