#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//SELECT SENSORTYPE: 5A, 20A, 30A
#define SENSORTYPE 5  //
#define UPDATE_TYPE 0 //1: OTA Update or 0: HTTP Update

//Khai báo các biến đọc giá trị ACS712
//-------------------------------------------------------------------
#define currentPin    0  //Chân ADC đọc cảm biến dòng ACS712
const unsigned long sampleTime = 30000;  //Thoi gian lay mau: 1000 mili giay
const uint32_t  averageTimeNum = 26;  //Thời gian lấy dữ liệu dòng điện
const int adc_zero = 512;
const double calibDivider = 0.00289855;

//KHAI BÁO WIFI
//-------------------------------------------------------------------
const char* ssid = "Phong Ky Thuat";
const char* password = "123456789";

const char* ssid1 = "GPON_NTH_TEST";
const char* password1 = "00000000";

const char* ssid2 = "BLUESKY";
const char* password2 = "bluesky@nth";

const char* ssid3 = "CAM_NTH";
const char* password3 = "0944613063";

bool connectStatus = LOW;   //Trạng thái kết nối Wifi
const unsigned long timeToWaiting = 30000;  //

//KHAI BÁO ĐỊA CHỈ CỦA SERVER 
//-------------------------------------------------------------------
const int httpPort = 8089;
const char* host = "113.161.18.185";
const char* url = "http://113.161.18.185:8089/DemoSV.asmx/INSERTDATA";

const char* passwordOTA = "123";

//KHAI BÁO CÁC BIẾN DỮ LIỆU GỬI LÊN SERVER
//-------------------------------------------------------------------
unsigned int accID = 0;
String dbAccessoriesID = "ACC|000001";  //MÃ THIẾT BỊ GIÁM SÁT
String dbAccessoriesID2 = "ACC|000002"; //MÃ THIẾT BỊ GIÁM SÁT
String dbCenterID = "CEN|000007";   //MÃ TRUNG TÂM
String dbPropertyID = "PRO|000003"; //NHÓM
String dbName = "GalaxyGame"; //ĐẶT TÊN CHO THIẾT BỊ
double dbCurrentValue = 0;    //GIÁ TRỊ DÒNG ĐIỆN ĐỌC TỪ CẢM BIẾN
String dbStatusDevice = "False";  //TRẠNG THÁI HIỆN TẠI CỦA DÒNG ĐIỆN
double dbAverageCurrentValue = 0;

String deviceData = ""; //Dữ liệu cứng của thiết bị

String data = "";   //Dữ liệu gửi lên Server 
int connectCounter = 0;
int txCounter = 0;
String MAC_ADDR = ""; //ĐỊA CHỈ MAC CỦA THIẾT BỊ
String rxINSERTDATA = ""; //data nhận được khi gọi webmethod "INSERTDATA"
String rxREGISTER = ""; //data nhận được khi gọi webmethod "REGISTER"
uint32_t chipID;
//char macAddr[25];

//KHAI BÁO CÁC WEBMETHOD TRONG WEBSERVICES
//-------------------------------------------------------------------
String INSERTDATA = "INSERTDATA";
String LOGIN = "LOGIN";
String SETTIME = "SETTIME";


/*********************************************************************
 *            KIỂM TRA KẾT NỐI CỦA MỘT SSID
 ********************************************************************/
bool checkingSSID(String _ssid){
  
  unsigned long startTime = millis();
  
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
      delay(500);
      Serial.println("Waiting for connection");
      
      //Hủy bỏ kết nối sau thời gian "timeToWaiting"
      if((unsigned long)(millis()-startTime) > timeToWaiting){ 
        Serial.println("Connect Wifi is FAILED");
        return LOW;
      }
  }
      
  Serial.print("Connecting to ");
  Serial.println(_ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  return HIGH;
  
}

/*********************************************************************
 *            KẾT NỐI WIFI THEO THỨ TỰ CÁC SSID
 ********************************************************************/
void connectingWifi(){

  while(WiFi.status() != WL_CONNECTED){
    
    WiFi.begin(ssid, password);
    connectStatus = checkingSSID(ssid);
    
    if(!connectStatus){
      WiFi.begin(ssid1, password1);
      connectStatus = checkingSSID(ssid1);
    }
    
    if(!connectStatus){
      WiFi.begin(ssid2, password2);
      connectStatus = checkingSSID(ssid2);
    }
    if(!connectStatus){
      WiFi.begin(ssid3, password3);
      connectStatus = checkingSSID(ssid3);
    }
  }
}
/*********************************************************************
 *            REGISTRY THIẾT BỊ
 ********************************************************************/
void tbRegister(String txdata){
  /*
  deviceData = "sAccessoriesID=" + dbAccessoriesID;
  deviceData += "&sCenterID=" + dbCenterID;
  deviceData += "&sPropertyID=" + dbPropertyID;
  deviceData += "&sName=" + dbName;
  MAC_ADDR = WiFi.macAddress();
  deviceData += "&macAddress=" + MAC_ADDR;
  Serial.println(MAC_ADDR);
  */
  
  String rx = comWithServer(txdata, "REGISTER");  //GỌI WEBMETHOD "REGISTER"
}

/*********************************************************************
 *            KẾT NỐI VỚI SERVER: CHỌN CÁC WEBMETHOD
 ********************************************************************/
String comWithServer(String txData, String webMethod){

  int j = 0;
  String rxData = "";
    
  WiFiClient client;
  //const int httpPort = 8089;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    /*
    connectCounter++;
    if(connectCounter==5){
      connectingWifi();
      connectCounter = 0;
    }*/
    return rxData;
  }
  
  //client.println("POST /DemoSV.asmx/INSERTDATA HTTP/1.1");
  client.print("POST /DemoSV.asmx/");
  client.print(webMethod);
  client.println(" HTTP/1.1");
  //client.println("Host: 113.161.18.185");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.print(txData.length());
  client.print("\n\n");
  client.print(txData);
  delay(500);

  while(client.available()){
    String line = client.readStringUntil('\r');
    //char* ch = &line[0];    
    //Serial.print(ch);
    
    if(j==11){ 
      rxData = processRxData(webMethod, line); 
    }
    j++;
  }
  
  client.stop(); //Stopping client  
  // Read all the lines of the reply from server and print them to Serial
  return rxData;
  
}
/*********************************************************************
 *            XỬ LÝ DỮ LIỆU NHẬN ĐƯỢC TỪ CÁC WEBMETHOD
 ********************************************************************/
String processRxData(String webMethodType, String sRxData){

  String result = "";
  
  if(webMethodType == LOGIN){
    return result;
  }else if(webMethodType == INSERTDATA){
    for(int j=37; j<60; j++){
      if((char)sRxData[j]=='<'){
        return  result;    
      }
      result += (char)sRxData[j];
    }
    
    /*
    char* p = strtok(rxData,"><");
    Serial.println (p);
    while (p!= NULL)
    {
      Serial.println(p);
      p = strtok (NULL, "><");
      if(j==1){
        //String tr =(char)p;
        Serial.println(p);
      }
      j++;
    }*/
  }else{
    return result;
  }
}

/****************************************************************************
*                           READ ACS712 5A VÀ TI SENSOR 
****************************************************************************/
double readACS712(){

  double result = 0;
  double temp = 0;
  double average = 0;
  
  unsigned long counter = 0;

  long analogValue;              //value read from the sensor
  
  unsigned long startTime = millis();   
  
  while((unsigned long)(millis()-startTime) < 1000) //sample for 1 Sec
  {
     analogValue = analogRead(A0) - adc_zero;
     average = average + (double)(analogValue * analogValue);
     counter++;
  }
  
  temp = sqrt(average/(double)counter);
  Serial.println(temp);
  
    result = sqrt(average/(double)counter) * (50/1024.0); // Lay gia tri trung binh


   result = result - 0.10;
    if (result<0.20)
    {
      result = 0;
    }else {
      result = result + 0.10;
    }
    result = result * 10;
    
  
   Serial.println(result);
   return result;
 }
/****************************************************************************
*                           UPDATE FIRMWARE 
****************************************************************************/
void updateFirmware(){
    
    String rxData = "";
    //KIỂM TRA KẾT NỐI WIFI
    connectingWifi(); 
    Serial.print("Updating Firmware...");
    
    
    #if UPDATE_TYPE   //OTA UPDATE
        ArduinoOTA.handle();
    #else   //HTTP UPDATE
        t_httpUpdate_return ret = ESPhttpUpdate.update("http://113.161.21.15:8266/WebUpdate.ino.nodemcu.bin");
        switch(ret) {
              case HTTP_UPDATE_FAILED:
                  Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                  break;
      
              case HTTP_UPDATE_NO_UPDATES:
                  Serial.println("HTTP_UPDATE_NO_UPDATES");
                  break;
      
              case HTTP_UPDATE_OK:
                  Serial.println("HTTP_UPDATE_OK");
                  //rxData = comWithServer("sUpdateStatus=OK","UPDATEFIRMWARE");  //CALL WEBMETHOD "UPDATEFIRMWARE"
                  ESP.restart();
                  break;
          }       
    #endif

}
void setup() {
  
  Serial.begin(115200);
  //pinMode(currentPin, INPUT_PULLUP);  //ĐẶT PIN ADC PULLUP

  // We start by connecting to a WiFi network
  //connectingWifi();

  //SETUP CÁC DỮ LIỆU CỨNG BAN ĐẦU
  chipID = ESP.getChipId();
  Serial.println(chipID);
  deviceData = "sAccessoriesID=" + dbAccessoriesID;
  deviceData += "&sCenterID=" + dbCenterID;
  deviceData += "&sPropertyID=" + dbPropertyID;
  deviceData += "&sName=" + dbName;
  MAC_ADDR = WiFi.macAddress();
  Serial.println(MAC_ADDR);
  //deviceData += "&macAddress=" + MAC_ADDR;
  //Serial.println(MAC_ADDR);
  
  /*//ĐĂNG KÝ THIẾT BỊ TRÊN SERVER 
  while(rxREGISTER != "Registration Successful"){
    delay(500);
    Serial.println("wait for registration");
    rxREGISTER = comWithServer(deviceData, "REGISTER");
  }
  Serial.println(rxREGISTER);
  */
  
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    //ESP.restart();
  }

  //Select Update Type: OTA or Http Update 
  #if UPDATE_TYPE   //UPDATE TYPE là OTA Update
      // Port defaults to 8266
      //ArduinoOTA.setPort(8266);
    
      // Hostname defaults to esp8266-[ChipID]
      ArduinoOTA.setHostname("NAME");
    
      // No authentication by default
      
      ArduinoOTA.setPassword(passwordOTA);
      
      
      ArduinoOTA.onStart([]() {
      Serial.println("Start");
      });
      ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      });
      ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });
      ArduinoOTA.begin();
  #endif

      
  connectingWifi();
  
}
/*********************************************************************
 *            MAIN PROGRAM
 ********************************************************************/
void loop() {

  if(digitalRead(4)==LOW){
      updateFirmware();
  }else{
      bool sta = LOW;
      //LẤY DỮ LIỆU DÒNG ĐIỆN
      dbAverageCurrentValue = 0;
      txCounter = 0;
      
      //WiFiClient client;
      /*
      unsigned long startTime = millis();
      
      while((unsigned long)(millis()-startTime)<24000){
        delay(1);
        dbCurrentValue = readACS712(); //Lấy giá trị tức thời của cảm biến dòng trong 1s
        dbAverageCurrentValue += dbCurrentValue; //Lấy giá trị dòng điện trung bình trong "averageTime"
      }
      Serial.println(dbAverageCurrentValue);
      */

      dbCurrentValue = readACS712();
      
      //KIỂM TRA KẾT NỐI WIFI
      connectingWifi(); 
      
      if(dbCurrentValue == 0){
        dbStatusDevice = "False";
      }else {
        dbStatusDevice = "True";
      }
    
      //DỮ LIỆU GỬI LÊN SERVER 
      data = deviceData;
      //data += "&MacAddress=" + macAddress;
      //data += "&AverageCurrentValue" + (String)dbAverageCurrentValue;
      data += "&sValue=" + (String)dbCurrentValue;
      data += "&sStatusDevice=" + dbStatusDevice;
    
      rxINSERTDATA = comWithServer(data, INSERTDATA);  //INSERT DỮ LIỆU VÀO SERVER 
      Serial.println(rxINSERTDATA);

      /*//UPDATE FIRMWARE
      if(rxINSERTDATA == "UPDATE FIRMWARE"){
        updateFirmware();
      }
      */
  }

 
  
} 
