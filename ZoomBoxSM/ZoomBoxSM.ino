/**************************************************
 * ME216M final project
 * Spring 2020
 * 
 * ZoomBox V1
 * Lets your friends know that you are available to 
 * hang out virtually after you dock your phone in 
 * the ZoomBox device. Automatically launches a Zoom 
 * call by detecting a wave gesture. 
 **************************************************/

/* Dependencies:
 *  https://github.com/adafruit/Adafruit_MQTT_Library
 *  https://github.com/knolleary/pubsubclient
 */
 
#include <Adafruit_NeoPixel.h>
#include <Arduino_LSM6DS3.h>
#include "EventManager.h"
#include "Ultrasonic.h"
#include <TimeLib.h>
#include <WiFiNINA.h>

//---------------------------------
//      Defintions & Variables     
//---------------------------------

#define DEBUG true
#define DEBUG_PRINT(msg) if (DEBUG) Serial.print(msg);
#define DEBUG_PRINTLN(msg) if (DEBUG) Serial.println(msg);

#define ARRAY_LENGTH(array) ((sizeof(array))/(sizeof(array[0])))

const int LOOP_DELAY_PERIOD = 5;

// Pin definitions
#define LIGHT_SENSOR_PIN A2 
#define LED_STICK_PIN 6 
#define NUMPIXELS 10
#define MY_LED_ID 8 // number from 0 - 9
#define ULTRA_SENSOR_PIN 4 // NOTE: digital
#define LED_PIN LED_BUILTIN
#define LED_ON_TIME_MS 2000 // duration of time for LED to light on gesture detection
#define TIMEOUT 5000 // duration of time for LED to light on gesture detection

// configure the NeoPixel library
Adafruit_NeoPixel pixels(NUMPIXELS, LED_STICK_PIN, NEO_GRB + NEO_KHZ800);

// configure ultrasonic sensor library 
Ultrasonic sensor(ULTRA_SENSOR_PIN);

WiFiSSLClient ZoomBoxWifi_wifi;

// gesture recognition variables
int analogUltraVal;
bool feature1 = true;
bool feature2 = false;
int startTime = 0;
int featureCount = 0;

// phone detection variables
int currentLightVal = 0;   // variable to store the value coming from sensor
bool docked = 0;
int phoneThresh = 200; 
int waiting = 10; //brightness level of LED
int active = 50;

// LED on arduino
unsigned long ledStartTime;
int ledState = 0; // 1 iff LED currently lit

// MQTT timer 
unsigned long MQTT_startTime;


//---------------------------------
//     Event & State Variables     
//---------------------------------
EventManager eventManager;
#define EVENT_WAVE_DETECTED EventManager::kEventUser0
#define EVENT_PHONE_DOCKED EventManager::kEventUser1
#define EVENT_PHONE_REMOVED EventManager::kEventUser2
#define EVENT_FRIEND_AVAILABLE EventManager::kEventUser3
#define EVENT_FRIEND_STARTED_CALL EventManager::kEventUser4
#define EVENT_FRIEND_LEFT_CALL EventManager::kEventUser4

// states for state machine
enum SystemState_t {STATE_ON_CALL, STATE_WAITING, STATE_IDLE};
SystemState_t currentState = STATE_IDLE;

//---------------------------------
//        Functions 
//---------------------------------

/* signalFriends()
 *  turns on my pixel on friends' devices
 *  @param brightnessLevel - brightness level of my LED
 *  @return none */
void signalFriends(int brightnessLevel) {
    pixels.clear(); // set all pixel colors to 'off'

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(MY_LED_ID, pixels.Color(0,255,0)); // bright green color
    pixels.setBrightness(brightnessLevel);
    pixels.show();   // send the updated pixel colors to the hardware
}

/* convertUltraVal
 *  converts ultrasonic reading to analog
 *  @param intVal - raw reading from ultrasonic
 *  @return none */
void convertUltraVal(int intVal) {
  analogUltraVal = map(intVal, 0, 400, 0, 1024);
} 

/* goIdle
 *  turns off my pixel on friends' devices
 *  @param none
 *  @return none  */
 void goIdle() {
    pixels.clear();
    pixels.show(); 
}

/* detectWaveFeature
 *  detects and counts features of a wave
 *  @param none
 *  @return none  */
void detectWaveFeature() {
  int currentTime = millis();
  
  if (analogUltraVal < 70 && feature1) { // dip in reading
    feature1 = false;
    feature2 = true; 
    startTime = millis();
    featureCount++;
  }

  if (analogUltraVal > 90 && feature2) { // peaks in reading 
    feature2 = false; 
    feature1 = true;
    //startTime1 = millis();
    featureCount++;
  }
  
  // resets count after timeout period
  if ((currentTime - startTime) > TIMEOUT) { 
      feature2 = false; 
      feature1 = true;
      featureCount = 0;
  }

}

//---------------------------------
//        Event Checkers  
//---------------------------------
/* Checks if an event has happened, and posts them 
 *  (and any corresponding parameters) to the event 
 * queue, to be handled by the state machine. */

/* readUltrasonic
 *  reads ultrasonic sensor and calls detectWaveFeature()
 *  to determine if user has waved
 *  @param none
 *  @return none  */
void readUltrasonic() {
  int eventParameter = 0; 

  // turns off LED after timeout period
  if (ledState == 1 && (millis() - ledStartTime) > LED_ON_TIME_MS) {
    digitalWrite(LED_PIN, LOW); 
  }

  convertUltraVal(sensor.MeasureInCentimeters());  
  detectWaveFeature();

  if (featureCount == 10) { // if wave detected
    signalFriends(active); // make brighter LED on friends devices 
    eventManager.queueEvent(EVENT_WAVE_DETECTED, eventParameter);
    //Serial.println("wave detected");
    // turn on LED for 2 seconds
    ledState = 1;
    ledStartTime = millis();
    digitalWrite(LED_PIN, HIGH);   
         
    // reset check
    feature2 = false; 
    feature1 = true;
    featureCount = 0;
  }
}

/* detectPhone
 *  detects presence of phone using ambient light sensor
 *  @param none
 *  @return none  */
void detectPhone() {
  int eventParameter = 0; 

  currentLightVal = analogRead(LIGHT_SENSOR_PIN); 
  
  if (currentLightVal < phoneThresh && docked == 0) {
      docked = 1;
     signalFriends(waiting); // turn on LED on friends devices 
     eventManager.queueEvent(EVENT_PHONE_DOCKED, eventParameter);
     //Serial.println("phone docked");
  } else if (currentLightVal > phoneThresh && docked == 1) {
     docked = 0;
     goIdle(); // turn off LEDs on friends devices
     eventManager.queueEvent(EVENT_PHONE_REMOVED, eventParameter);
     //Serial.println("phone removed");
  }
}

//---------------------------------
//           State Machine  
//---------------------------------
/* Responds to events based on the current state. */

void ZOOMBOX_SM( int event, int param) {
  
    SystemState_t nextState = currentState; //initialize next state

    switch (currentState) { 
        case STATE_IDLE:
            if (event == EVENT_PHONE_DOCKED) {
              ZoomBoxFriend_signalAvailability();       
              nextState = STATE_WAITING;
            } 
  
            break;          
        case STATE_WAITING:
            if (event == EVENT_PHONE_REMOVED) {
              //Serial.println("WAITING -> phone removed");
              ZoomBoxFriend_signalLeaveCall();
              nextState = STATE_IDLE;  
            } else if (event == EVENT_WAVE_DETECTED) {
              //Serial.println("WAITING -> wave detected");
              ZoomBoxFriend_signalStartCall(); // launch zoom call
              nextState = STATE_ON_CALL;
            }  

            if (event == EVENT_FRIEND_AVAILABLE) {
              //Serial.print("WAITING -> friend available #");
              //Serial.println(param);
            }
            
            if (event == EVENT_FRIEND_STARTED_CALL) {
              //Serial.print("WAITING -> friend started zoom call: ");
              //Serial.println(getFriendName(param));
              nextState = STATE_ON_CALL;
            }

            break;          
        case STATE_ON_CALL:
            if (event == EVENT_PHONE_REMOVED) {
              //Serial.println("ON CALL-> phone removed");
              nextState = STATE_IDLE;
            } else if(event == EVENT_WAVE_DETECTED) {
              //Serial.println("ON CALL-> wave detected");
              // end zoom call
              signalFriends(waiting); // lower LED brightness
              nextState = STATE_WAITING;
            } 
            
            if (event == EVENT_FRIEND_LEFT_CALL) {
              Serial.print("ON CALL-> friend left call: ");
              Serial.println(getFriendName(param));
              
              goIdle(); // turn off LEDs on friends devices
              nextState = STATE_IDLE;
            }

            break;         
        default:
            Serial.println("STATE: Unknown State");
            break;
      }
       currentState = nextState;
}
