#include "AdafruitIO_WiFi.h"


void ZoomBoxWiFi_setup() {
    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      while (true); // do not continue
    }
    // Check for firmware version
    String fv = WiFi.firmwareVersion();
    if (fv < "1.0.0") {
      Serial.println("Please upgrade the firmware");
    }
    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ZoomBoxWiFi_SSID);
      
      WiFi.begin(ZoomBoxWiFi_SSID, ZoomBoxWiFi_Password);
      
      // wait 10 seconds for connection:
      int seconds = 0;
      while (seconds++ < 10 && WiFi.status() != WL_CONNECTED) {
        Serial.println("Connecting to WiFi...");
        delay(1000);
      }
    }
    Serial.println("WiFi successfully connected");
}
