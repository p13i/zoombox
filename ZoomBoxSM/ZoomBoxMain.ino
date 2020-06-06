
void setup() {
    Serial.begin(9600); // initialize serial
    while (!Serial); // wait for serial to connect
    
    pixels.begin(); // initialize NeoPixel strip object 
    pixels.clear();

    pinMode(LED_PIN, OUTPUT); // arduino LED

    // Setup MQTT
    ZoomBoxWiFi_setup();
    
    ZoomBoxMQTT_setup();

    for (int i = 0; i < ARRAY_LENGTH(friends); i++) {
      if (friends[i].feed && friends[i].feed.length() > 0) {
        const char *feed = friends[i].feed.c_str();
        ZoomBoxMQTT_subscribe(feed);
        Serial.println("Subscribed to ");
        Serial.println(feed);
      }
    }

    ZoomBoxMQTT_connect();

    // initialize state machine as a listener for events
    eventManager.addListener(EVENT_WAVE_DETECTED, ZOOMBOX_SM);
    eventManager.addListener(EVENT_PHONE_DOCKED, ZOOMBOX_SM);
    eventManager.addListener(EVENT_PHONE_REMOVED, ZOOMBOX_SM);
    eventManager.addListener(EVENT_FRIEND_AVAILABLE, ZOOMBOX_SM);

    // initialize LED state machine
    ZOOMBOX_SM(STATE_IDLE, 0);
}

void loop() {
    // handle any events that are in the queue
    eventManager.processAllEvents();
    
     readUltrasonic();
     detectPhone();

    ZoomBoxFriend_signalAvailability();
    ZoomBoxFriend_signalStartCall();
    ZoomBoxFriend_signalLeaveCall();

    ZoomBoxMQTT_loop();

    delay(LOOP_DELAY_PERIOD);
}
