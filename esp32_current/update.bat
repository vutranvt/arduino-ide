copy /Y C:\esp32\arduino-ide\esp32-rtos\esp32-rtos.ino.esp32.bin C:\xampp\htdocs\current-v1.1.ino.esp32.bin
mosquitto_pub -t hue/device1/update -m "update"