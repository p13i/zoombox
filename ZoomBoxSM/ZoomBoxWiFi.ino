#include "AdafruitIO_WiFi.h"

// Define your WiFi settings here
const char* ZoomBoxWiFi_SSID = "VPPK_2020";
const char* ZoomBoxWiFi_Password = "bangalore";

void ZoomBoxWiFi_setup() {
    if (WiFi.status() == WL_NO_MODULE) {
      DEBUG_PRINTLN("Communication with WiFi module failed!");
      while (true); // do not continue
    }
    // Check for firmware version
    String fv = WiFi.firmwareVersion();
    if (fv < "1.0.0") {
      DEBUG_PRINTLN("Please upgrade the firmware");
    }
    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
      DEBUG_PRINT("Attempting to connect to SSID: ");
      DEBUG_PRINTLN(ZoomBoxWiFi_SSID);
      
      WiFi.begin(ZoomBoxWiFi_SSID, ZoomBoxWiFi_Password);
      
      // wait 10 seconds for connection:
      int seconds = 0;
      while (seconds++ < 10 && WiFi.status() != WL_CONNECTED) {
        DEBUG_PRINTLN("Connecting to WiFi...");
        delay(1000);
      }
    }
    DEBUG_PRINTLN("WiFi successfully connected");
}
