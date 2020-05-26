/**************************************************
 * ME216M final project
 * 
 * Detects presence of phone with analog light sensor
 * Lights appropriate LED to indicate availability
 **************************************************/
 
#include <Adafruit_NeoPixel.h>

#define SENSOR_PIN A0 
#define LED_STICK_PIN 6 
#define NUMPIXELS 10
#define MY_LED_ID 8 // number from 0 - 9

// configure the NeoPixel library
Adafruit_NeoPixel pixels(NUMPIXELS, LED_STICK_PIN, NEO_GRB + NEO_KHZ800);

int index_LED; // variable to store current number of lit LEDs
int currentLightVal = 0;   // variable to store the value coming from sensor
int prevLightVal = 0; // variable to store previous value of sensor
int phoneThresh = 550;

void setup() {
  Serial.begin(9600); // initialize serial
  while (!Serial); // wait for serial to connect
  pixels.begin(); // initialize NeoPixel strip object 
  pixels.clear();
}

void loop() {
  currentLightVal = analogRead(SENSOR_PIN); // read the value from the sensor
  
  if (currentLightVal < phoneThresh && prevLightVal > phoneThresh) {
    Serial.println("Phone detected");
    pixelOn(); // light pixel
  } else if (currentLightVal > phoneThresh && prevLightVal < phoneThresh) {
    Serial.println("Phone off");
    pixels.clear();
    pixels.show(); 
  }
  
  prevLightVal = currentLightVal;

}

void pixelOn() {
    pixels.clear(); // set all pixel colors to 'off'

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(MY_LED_ID, pixels.Color(0,255,0)); // bright green color
    
    pixels.show();   // send the updated pixel colors to the hardware
}
