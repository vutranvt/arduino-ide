#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__


/* name of game */
#define DRAGON
// #define PANDA

/* wifi */
#define SSID "Phong Ky Thuat"
#define PASSWORD "123456789"

/* dragon */
#if defined(DRAGON)
    //mqtt topic
    #define TOPIC_PUBLISH "bmt/game/dragon"
	#define TOPIC_SUBSCRIBE	"bmt/game/dragon/sub"  
    #define TOPIC_LWT "bmt/game/dragon/lwt"
    //function
    #define LOCATION "bmt"   // vị trí
    #define TYPE "game"      // loại
    #define NAME "dragon"  // tên
#endif

/* panda */
#if defined(PANDA)
    //mqtt topic
    #define TOPIC_PUBLISH "bmt/game/panda"
    #define TOPIC_SUBSCRIBE "bmt/game/panda/sub"  
    #define TOPIC_LWT "bmt/game/panda/lwt"
    //function
    #define LOCATION "bmt"   // vị trí
    #define TYPE "game"      // loại
    #define NAME "panda"  // tên
#endif

#endif
