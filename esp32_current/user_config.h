#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

//#define RAW_ADC    // send 'raw_adc' replace for 'result' 

/* location */
 #define NTH

/* wifi */
#if defined(TEST)
    #define WIFI_SSID "BLUE SKY"
    #define WIFI_PASS "bluesky@nth"
#else 
  #if defined(NTH)
    #define WIFI_SSID "Phong Ky Thuat"
    #define WIFI_PASS "123456789"
  #else
    #define WIFI_SSID "STARLIGHT"
    #define WIFI_PASS "starlight123"
  #endif  
#endif

/* adc */
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
#define CALIB_RATIO 27
#define ADC_ZERO_1 2450
#define ADC_ZERO_2 2424
#define ADC_ZERO_3 2445
#define TI_RATIO 80


/* test */
#if defined(TEST)
    //mqtt topic
    #define TOPIC_PUBLISH "location/type/name"
    #define TOPIC_SUBSCRIBE "location/type/name/sub"
    #define TOPIC_LWT "location/type/name/lwt"    
    //function
    #define LOCATION "location"   // trung tam
    #define TYPE "type"      // loai
    #define NAME "name"  // vi tri
#endif


/* NTH */
#if defined(NTH)
    //mqtt topic
    #define TOPIC_PUBLISH "nth/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "nth/current/tudien_tong/sub"
    #define TOPIC_LWT "nth/current/tudien_tong/lwt"
    //function
    #define LOCATION "nth"   // trung tam
    #define TYPE "current"      // loai
    #define NAME "tudien_tong"  // vi tri
#endif

/* BMT */
#if defined(BMT)
    //mqtt topic
    #define TOPIC_PUBLISH "bmt/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "bmt/current/tudien_tong/sub"
    #define TOPIC_LWT "bmt/current/tudien_tong/lwt"
    //function 
    #define LOCATION "bmt"
    #define TYPE "current"
    #define NAME "tudien_tong"
#endif

/* DANANG */
#if defined(DANANG)
    //mqtt topic
    #define TOPIC_PUBLISH "dna/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "dna/current/tudien_tong/sub"
    #define TOPIC_LWT "dna/current/tudien_tong/lwt"
    //function
    #define LOCATION "dna"
    #define TYPE "current"
    #define NAME "tudien_tong"
#endif

/* HUE */
#if defined(HUE)
    #define TOPIC_PUBLISH "hue/current/tudien_tong"
    #define TOPIC_SUBSCRIBE "hue/current/tudien_tong/sub"
    #define TOPIC_LWT "hue/current/tudien_tong/lwt"
    //function
    #define LOCATION "hue"
    #define TYPE "current"
    #define NAME "tudien_tong"
#endif

#endif
