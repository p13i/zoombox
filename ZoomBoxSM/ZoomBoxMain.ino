
long lastMqttRun;

void setup() {
    Serial.begin(9600); // initialize serial
    while (!Serial); // wait for serial to connect
    
    pixels.begin(); // initialize NeoPixel strip object 
    pixels.clear();

    pinMode(LED_PIN, OUTPUT); // arduino LED

    // Setup MQTT
    ZoomBoxWiFi_setup();
    
    signalWiFiConnected();
    
    ZoomBoxMQTT_setup();

    MQTT_startTime = millis();

    ZoomBoxMQTT_subscribe(ZoomBoxMQTT_SharedFeed);
    Serial.println("Subscribed to ");
    Serial.println(ZoomBoxMQTT_SharedFeed);

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
    eventManager.processEvent();
    
    detectWave();
    detectPhone();

    unsigned long MQTT_currentTime = millis();
    if ((MQTT_currentTime - MQTT_startTime) > 5000) {
      ZoomBoxMQTT_loop();
      MQTT_startTime = MQTT_currentTime;
    }

    delay(250);
}
