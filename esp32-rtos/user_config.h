#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

//#define RAW_ADC    // send raw_adc  to broker


/* config device's LOCATION*/
//#define TEST
// #define NTH
 #define BUONMATHUOT
// #define DANANG
// #define HUE
// #define QUYNHON
// #define BAOLOC


/* config mqtt*/
#define MQTT_SERVER "113.161.21.15"
#define MQTT_PORT 1884
#define MQTT_USER "esp32"
#define MQTT_PASS "mtt@23377"


/* config wifi*/
#if defined(TEST)
    #define WIFI_SSID "Phong Ky Thuat"
    #define WIFI_PASS "123456789"
#else 
  #if defined(NTH)
    #define WIFI_SSID "Phong Ky Thuat"
    #define WIFI_PASS "123456789"
  #else
    #define WIFI_SSID "STARLIGHT"
    #define WIFI_PASS "starlight123"
  #endif
#endif


/* config mqtt client id */
#if defined(TEST)
    #define MQTT_CLIENTID "esp_24:0A:C4:10:F9:C8"
#endif
#if defined(NTH)
   #define MQTT_CLIENTID "esp_24:0A:C4:0F:17:2C"
#endif
#if defined(BUONMATHUOT)
    #define MQTT_CLIENTID "esp_24:0A:C4:12:1F:04" // bmt/current/tudien_tong
#endif



/* config adc */
// (ADC1_CHANNEL_0)   // GPIO 36 - VP
// (ADC1_CHANNEL_1)   // GPIO 37
// (ADC1_CHANNEL_2)   // GPIO 38
// (ADC1_CHANNEL_3)   // GPIO 39 - VN
// (ADC1_CHANNEL_4)   // GPIO 32
// (ADC1_CHANNEL_5)   // GPIO 33
// (ADC1_CHANNEL_6)   // GPIO 34
// (ADC1_CHANNEL_7)   // GPIO 35
//#define PIN_ADC1 (ADC1_CHANNEL_0)   // GPIO 36 - VP
//#define PIN_ADC2 (ADC1_CHANNEL_7)   // GPIO 35
//#define PIN_ADC3 (ADC1_CHANNEL_6)   // GPIO 34
#define PIN_ADC1 36   // GPIO 36 - VP
#define PIN_ADC2 35   // GPIO 35
#define PIN_ADC3 34   // GPIO 34

// config ADC
#define CALIB_RATIO 27
#define ADC_ZERO_1 2450
#define ADC_ZERO_2 2424
#define ADC_ZERO_3 2445
#define TI_RATIO 80


/* config for testing */
#if defined(TEST)
    //config mqtt topic
    #define TOPIC_PUBLISH "center/type/location"
    #define TOPIC_SUBSCRIBE "center/type/location"
    #define TOPIC_MAC_ADDRESS "center/type/location/mac_address"
    #define TOPIC_FIRMWARE_VERSION "center/type/location/firmware_version"
    #define TOPIC_LWT "center/type/location/lwt"
    #define MESSAGE_LWT "offline"
    
    //config function of device
    #define CENTER "center"   // trung tam
    #define TYPE "type"      // loai
    #define LOCATION "location"  // vi tri
#endif


/* config device in NTH */
#if defined(NTH)
    //config mqtt topic
    #define TOPIC_PUBLISH "nth/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "nth/current/tudien_tong"
    #define TOPIC_MAC_ADDRESS "nth/current/tudien_tong/mac_address"
    #define TOPIC_FIRMWARE_VERSION "nth/current/tudien_tong/firmware_version"
    #define TOPIC_LWT "nth/current/tudien_tong/lwt"
    #define MESSAGE_LWT "offline"
    
    //config function of device
    #define CENTER "nth"   // trung tam
    #define TYPE "current"      // loai
    #define LOCATION "tudien_tong"  // vi tri
#endif

/* config device in BMT */
#if defined(BUONMATHUOT)
    #define TOPIC_PUBLISH "bmt/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "bmt/current/tudien_tong"
    #define TOPIC_MAC_ADDRESS "bmt/current/tudien_tong/mac_address"
    #define TOPIC_FIRMWARE_VERSION "bmt/current/tudien_tong/firmware_version"
    #define TOPIC_LWT "bmt/current/tudien_tong/lwt"
    #define MESSAGE_LWT "offline"
    
    //config function of device
    #define CENTER "bmt"
    #define TYPE "current"
    #define LOCATION "tudien_tong"
#endif

/* config device in DA NANG */
#if defined(DANANG)
    #define TOPIC_PUBLISH "dna/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "dna/current/tudien_tong"
    #define TOPIC_MAC_ADDRESS "dna/current/tudien_tong/mac_address"
    #define TOPIC_FIRMWARE_VERSION "dna/current/tudien_tong/firmware_version"
    #define TOPIC_LWT "dna/current/tudien_tong/lwt"
    #define MESSAGE_LWT "offline"
    
    //config function of device
    #define CENTER "dna"
    #define TYPE "current"
    #define LOCATION "tudien_tong"
#endif

/* config device in HUE */
#if defined(HUE)
    #define TOPIC_PUBLISH "hue/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "hue/current/tudien_tong"
    #define TOPIC_MAC_ADDRESS "hue/current/tudien_tong/mac_address"
    #define TOPIC_FIRMWARE_VERSION "hue/current/tudien_tong/firmware_version"
    #define TOPIC_LWT "hue/current/tudien_tong/lwt"
    #define MESSAGE_LWT "offline"
    
    //config function of device
    #define CENTER "hue"
    #define TYPE "current"
    #define LOCATION "tudien_tong"
#endif


/* config device in BAOLOC */
#if defined(BAOLOC)
    #define TOPIC_PUBLISH "blo/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "blo/current/tudien_tong"
    #define TOPIC_MAC_ADDRESS "blo/current/tudien_tong/mac_address"
    #define TOPIC_FIRMWARE_VERSION "blo/current/tudien_tong/firmware_version"
    #define TOPIC_LWT "blo/current/tudien_tong/lwt"
    #define MESSAGE_LWT "offline"
    
    //config function of device
    #define CENTER "blo"
    #define TYPE "current"
    #define LOCATION "tudien_tong"
#endif

/* config device in QUYNHON */
#if defined(QUYNHON)
    #define TOPIC_PUBLISH "qn/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "qn/current/tudien_tong"
    #define TOPIC_MAC_ADDRESS "qn/current/tudien_tong/mac_address"
    #define TOPIC_FIRMWARE_VERSION "qn/current/tudien_tong/firmware_version"
    #define TOPIC_LWT "qn/current/tudien_tong/lwt"
    #define MESSAGE_LWT "offline"
    
    //config function of device
    #define CENTER "qn"
    #define TYPE "current"
    #define LOCATION "tudien_tong"
#endif


#endif
