#include <LiquidCrystal.h>
#include "Wire.h"
#include <LiquidCrystal.h>
LiquidCrystal lcd(5, 4, 3, 2, 1, 0);

#define DIM_CH2   0   //DIM_CH2 = 0: Not Dimming CH2    //DIM_CH2 = 1: Dimming CH2
#define NO_NOISE   1;
#define PCF8591 (0x90 >> 1) 

//SET chan ngo ra PWM
#define pinOutCH1  11    //ngõ ra CH1
#define pinOutCH2  12    //ngõ ra CH2

//SET chan ngo vao Nut nhan
#define pinOnCH1    19  //switch On CH1 
#define pinHalfCH1  18  //switch Half CH1
#define pinOffCH1   17  //switch Off CH1
#define pinOnCH2    16  //switch On CH2
#define pinOffCH2   15  //switch Off CH2
                                              
//char* stateChannel[] = {

//SET Dien ap khoi dong
int dienapCho  =  95;        //điện áp chờ: 94
unsigned int dienapStart =  100;      //điện áp khoi dong: 250

//Dien ap kenh 1
const unsigned int dienapOnCH1   =  1500;
const unsigned int dienapHalfCH1 =  500;//500
const unsigned int dienapOffCH1  =  200;//250
unsigned int buocNhay   =  1;

//Dien ap kenh 2
const unsigned int dienapOnCH2   =  1500;
const unsigned int dienapOffCH2  =  0;

const unsigned int off = 0;

//SET Thoi gian cho cac kenh
unsigned int msOffCH1 = 20;
unsigned int msHalfCH1 = 40;//40
unsigned int msOnCH1 = 20;//20
unsigned int msOffCH2 = 30;//30
unsigned int msOnCH2 = 20;

volatile unsigned int stateCH1 = 0;   //Trạng thái kênh 1  
volatile unsigned int stateCH2 = 0;   //Trạng thái kênh 2  
//unsigned int i = 0;

bool realState;            //Trang thai Thuc cua Tin hieu Kich
int msState = 5;           //SET Thoi gian Chap nhan Tin hieu Kich

void lcd_display(unsigned int state1, unsigned int state2){
    lcd.begin(16, 2);
    lcd.clear();
    if(state1==dienapOnCH1){
        lcd.setCursor(0, 0);
        lcd.print("CH1: ON");
        lcd.setCursor(11, 0);
        lcd.print(stateCH1);
    }else if(state1==dienapHalfCH1){
        lcd.setCursor(0, 0);
        lcd.print("CH1: HALF");
        lcd.setCursor(11, 0);
        lcd.print(stateCH1);
    }else if(state1==off){
        lcd.setCursor(0, 0);
        lcd.print("CH1: OFF");
        lcd.setCursor(11, 0);
        lcd.print(stateCH1);
    }
    if(state2==dienapOnCH2){
        lcd.setCursor(0, 1);
        lcd.print("CH2: ON");
        lcd.setCursor(11, 1);
        lcd.print(stateCH2);  
    }else if(state2==off){
        lcd.setCursor(0, 1);
        lcd.print("CH2: OFF");
        lcd.setCursor(11, 1);
        lcd.print(stateCH2);
    }
}
/* *******************************************************
 *      Do xung kich tu may chieu
 *********************************************************/
/*
unsigned int pulse_time(int pin){
    unsigned int pulseTime;
    unsigned int startTime = microSeconds();
    while((microSeconds()-startTime)<1000){
        if(digitalRead(pin)==LOW){
            continue;
        }else{
            pulseTime = microSeconds() - startTime;
            Serial.println(pulseTime);
            return pulseTime;
            break;
        }
    }
}*/
/* *******************************************************
 *      Kiem tra Trang thai cua Tin hieu Kich
 *********************************************************/
bool checking_pin(int state){
  switch(state){
      case pinOffCH1:
        delay(msState);
        return digitalRead(pinOffCH1); 
        break;
      case pinOnCH1:
        delay(msState);
        return digitalRead(pinOnCH1); 
        break;
      case pinHalfCH1:
        delay(msState);
        return digitalRead(pinHalfCH1); 
        break;
      case pinOffCH2:
        delay(msState);
        return digitalRead(pinOffCH2);      
        break;
      case pinOnCH2:
        delay(msState);
        return digitalRead(pinOnCH2); 
        break;
  }
}
/* *******************************************************
 *                       Write PWM
 *********************************************************/
void pwmWrite(int pin, unsigned int pulseVolt){
    if(pin==pinOutCH1){
        OCR1A = pulseVolt;
    }else if(pin==pinOutCH2){
        OCR1B = pulseVolt;
    }
}
/* *******************************************************
 *                       Output Analog Voltage
 *********************************************************/
void pcf8591_Output(unsigned int pulseVolt){
  for(int j=0; j<buocNhay; j++){
      Wire.beginTransmission(PCF8591); 
      Wire.write(0x40); // sets the PCF8591 into a DA mode
      Wire.write(pulseVolt + j); // sets the outputn
      Wire.endTransmission();
  }
}
/* *******************************************************
 *                       OFF CHANNEL 1
 *********************************************************/
void off_CH1(){
    lcd_display(off, stateCH2);
    unsigned int val = stateCH1;
    unsigned int val1 = stateCH1;
    //unsigned int val2 = stateCH2;
    if(stateCH1!=0){
        for(val1=stateCH1; val1>=dienapOffCH1; val1=val1-buocNhay){
            pwmWrite(pinOutCH1, val1);
            if(val1==dienapStart){
                delay(2000);
            }
            delay(msOffCH1);
            lcd.setCursor(11, 0);
            lcd.print(val1);
            lcd.print(" ");
        } 
    }       
    stateCH1=0;
    pwmWrite(pinOutCH1, stateCH1);
    
    lcd.setCursor(11, 0);
    lcd.print(stateCH1);
    lcd.print("  ");
    lcd_display(stateCH1,stateCH2);
}
/* *******************************************************
 *                        KHỞI ĐỘNG
 *********************************************************/
void start(){
  unsigned int stateCH2 = 1000;
  unsigned int thoigian;
  unsigned int val;
  for(val=70; val<dienapCho; val=val+buocNhay){
      pcf8591_Output(val);
      delay(20); 
  }      
  delay(3000);
  lcd.setCursor(11, 0);
  lcd.print(val);
  lcd.print(" ");
  
  for(val=dienapCho;val<=dienapStart;val=val+buocNhay){
      pcf8591_Output(val);
      delay(10);
  }
  lcd.setCursor(11, 0);
  lcd.print(val);
  lcd.print(" ");
  //delay(5000);  
}


/* *******************************************************
 *                        ON CHANNEL 1
 *********************************************************/
void on_CH1(){
    lcd_display(dienapOnCH1, stateCH2);
    //lcd.setCursor(0, 0);
    //lcd.print("CH1: ON  ");
    unsigned int val = stateCH1;
    unsigned int val1=stateCH1;
    //unsigned int val2=stateCH2;

    //TU OFF --> ON
    if(stateCH1==0){ //Kiểm tra trạng thái đang là OFF
        //start();     
        for(val1=dienapStart; val1<=dienapOnCH1; val1=val1+buocNhay){
            pwmWrite(pinOutCH1, val1);
            delay(msOnCH1);
            lcd.setCursor(11, 0);
            lcd.print(val1);
            lcd.print(" ");
        }   
    }
    
    //TU HAFT --> ON
    if(stateCH1==dienapHalfCH1) { //Trạng thái hoạt động đang là HAFT
        for(val1=dienapHalfCH1; val1<=dienapOnCH1; val1=val1+buocNhay){
            pwmWrite(pinOutCH1, val1);
            delay(msOnCH1);
            lcd.setCursor(11, 0);
            lcd.print(val1);
            lcd.print(" ");
        }
    }
    stateCH1=dienapOnCH1;
    lcd.setCursor(11, 0);
    lcd.print(stateCH1);
    lcd.print(" ");
    lcd_display(stateCH1,stateCH2);
}

/* *******************************************************
 *                        HALF CHANNEL 1
 *********************************************************/
void half_CH1(){
    lcd_display(dienapHalfCH1, stateCH2);
    //lcd.setCursor(0, 0);
    //lcd.print("CH1: HALF");
    unsigned int val = stateCH1;
    unsigned int val1 = stateCH1;
    //unsigned int val2 = stateCH2;
    
    //TU OFF --> HALF
    if(stateCH1==0){ //Kiểm tra trạng thái hoạt động đang là OFF
      //start();
      for(val1=dienapStart; val1<=dienapHalfCH1; val1=val1+buocNhay){
          pwmWrite(pinOutCH1, val1);
          delay(msHalfCH1);
          lcd.setCursor(11, 0);
          lcd.print(val1);
          lcd.print(" ");
      }
      stateCH1=dienapHalfCH1;
      //on_CH2();                 //ON CH2 sau khi half xong CH1
    }   
   
    //TU ON --> HALF
    if(stateCH1==dienapOnCH1) {  //Kiểm tra trạng thái đang là ON
      for(val1=dienapOnCH1; val1>=dienapHalfCH1; val1=val1-buocNhay){
        pwmWrite(pinOutCH1, val1);
        delay(30);   //30
        lcd.setCursor(11, 0);
        lcd.print(val1);
        lcd.print(" ");
      }
    }
    stateCH1=dienapHalfCH1;  
    lcd.setCursor(11, 0);
    lcd.print(stateCH1);
    lcd.print(" ");
    lcd_display(stateCH1,stateCH2);
}
/* *******************************************************
 *                        OFF CHANNEL 2
 *********************************************************/
void off_CH2(){
    lcd_display(stateCH1, off);
    unsigned int val2 = stateCH2;
    
    #if DIM_CH2         //0: NOT DIM   // 1: DIM
      if(stateCH2==dienapOnCH2){   //kiem tra trang thai hien tai la: ON
          for(val2=stateCH2; val2>=dienapOffCH2; val2=val2-buocNhay){      //DIM process
              pwmWrite(pinOutCH2, val2);
              delay(msOffCH2);
              lcd.setCursor(11, 1);
              lcd.print(val2);
              lcd.print(" ");
          }      
      }
    #else
        stateCH2=0;
        pwmWrite(pinOutCH2, stateCH2);
    #endif  
            
    stateCH2=0;
    pwmWrite(pinOutCH2,stateCH2);
    lcd_display(stateCH1,stateCH2);
}
/* *******************************************************
*                        ON CHANNEL 2
*********************************************************/
void on_CH2(){
    lcd_display(stateCH1, dienapOnCH2);
    unsigned int val2 = stateCH2;

    #if DIM_CH2     //0: NOT DIM    //1: DIM
        //TU OFF --> ON
        if(stateCH2==0){ //Kiểm tra trạng thái đang là OFF    
          for(val2=dienapStart; val2<=dienapOnCH2; val2=val2+buocNhay){    //DIM process
              pwmWrite(pinOutCH2,val2);
              delay(msOnCH2);
              lcd.setCursor(11, 1);
              lcd.print(val2);
              lcd.print(" ");
          }    
        }
    #else
        stateCH2=dienapOnCH2;
        pwmWrite(pinOutCH2, stateCH2);
    #endif
    
    stateCH2=dienapOnCH2;
    lcd_display(stateCH1,stateCH2);
}

void setup(){
  pinMode(pinOutCH1, OUTPUT);    // OUTPUT KENH 1
  pinMode(pinOutCH2, OUTPUT);    // OUTPUT KENH 2
  pinMode(pinOnCH1, INPUT_PULLUP);    
  pinMode(pinHalfCH1, INPUT_PULLUP); 
  pinMode(pinOffCH1, INPUT_PULLUP); 
  pinMode(pinOnCH2, INPUT_PULLUP); 
  pinMode(pinOffCH2, INPUT_PULLUP);  
  pinMode(A8, INPUT_PULLUP); 

  //Serial.begin(9600);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CH1: OFF ");
  lcd.setCursor(11, 0);
  lcd.print("0  ");
  lcd.setCursor(0, 1);
  lcd.print("CH2: OFF ");
  lcd.setCursor(11, 1);
  lcd.print("0   ");

  
      // Set PB1/2 as outputs.
      DDRB |= (1 << DDB1) | (1 << DDB2);
    
      TCCR1A =
          (1 << COM1A1) | (1 << COM1B1) |
          // Fast PWM mode.
          (1 << WGM11);
      TCCR1B =
          // Fast PWM mode.
          (1 << WGM12) | (1 << WGM13) |
          // No clock prescaling (fastest possible
          // freq).
          (1 << CS10);
      OCR1A = 0;
      OCR1B = 0;
      // Set the counter value that corresponds to
      // full duty cycle. For 15-bit PWM use
      // 0x7fff, etc. A lower value for ICR1 will
      // allow a faster PWM frequency.
      ICR1 = 0xffff;
}

void loop() {  

  //----------OFF CH1----------
  while(digitalRead(pinOffCH1)==LOW){  // CHECK CHÂN OFF CH1
      realState = checking_pin(pinOffCH1);
      if(realState==LOW){
          off_CH1();
      }else{
          break;   
      }
  }
  //----------HALF CH1---------
  while(digitalRead(pinHalfCH1)==LOW){ // CHECK CHÂN HALF CH1
      realState = checking_pin(pinHalfCH1);
      if(realState==LOW){
          half_CH1();
      }else{
          break;   
      }
  }
  //----------ON CH1-----------
  while(digitalRead(pinOnCH1)==LOW){   // CHECK CHÂN ON CH1 
      realState = checking_pin(pinOnCH1);
      if(realState==LOW){
          on_CH1();
      }else{
          break;   
      }
  }
  
  //----------ON CH2-----------
  while(digitalRead(pinOnCH2)==LOW){  // CHECK CHÂN ON CH2
      realState = checking_pin(pinOnCH2);
      if(realState==LOW){
          on_CH2();
      }else{
          break;          
      }
  }
  //----------OFF CH2----------
  while(digitalRead(pinOffCH2)==LOW){  // CHECK CHÂN OF CH2
      realState = checking_pin(pinOffCH2);
      if(realState==LOW){
          off_CH2();
      }else{
          break;   
      }
  }
}
