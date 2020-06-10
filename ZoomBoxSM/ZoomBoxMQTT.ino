#include <WiFiNINA.h>
#include <PubSubClient.h>

//---------------------------------
//      Defintions & Variables
//---------------------------------
#define LED_PIN LED_BUILTIN

// MQTT Parameters (defined in config.h)
const char* ZoomBoxMQTT_server            = "io.adafruit.com";
const char* ZoomBoxMQTT_username          = "p13i";
const char* ZoomBoxMQTT_key               = "aio_yydd88xOlSg83368OaTYO00Olv0z";
const int   ZoomBoxMQTT_port              = 1883;
const int   ZoomBoxMQTT_subTopicsMaxCount = 12;
const char* ZoomBoxMQTT_subTopics[ZoomBoxMQTT_subTopicsMaxCount];
int ZoomBoxMQTT_subTopicsCount            = 0;

WiFiClient ZoomBoxMQTT_wifiClient;
PubSubClient ZoomBoxMQTT_client(ZoomBoxMQTT_wifiClient);

// General variables
long lastMsgTime = 0;
int counter = 0;


void ZoomBoxMQTT_setup() {
  // Connect to WiFi:
  Serial.print("Connecting to ");
  Serial.println(ZoomBoxWiFi_SSID);
  WiFi.begin(ZoomBoxWiFi_SSID, ZoomBoxWiFi_Password);

  while (WiFi.status() != WL_CONNECTED)
  {
    // wait while we connect...
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("------------------------");

  // Set up MQTT
  ZoomBoxMQTT_client.setServer(ZoomBoxMQTT_server, ZoomBoxMQTT_port);
  ZoomBoxMQTT_client.setCallback(ZoomBoxMQTT_callback);
}

void ZoomBoxMQTT_subscribe(const char *topic) {
  if (ZoomBoxMQTT_subTopicsCount == ZoomBoxMQTT_subTopicsMaxCount) {
    Serial.println("Error! Maximum subscribed topics exceeded!");
    return;
  }

  if (!topic) {
    Serial.println("Error! Subscribe topic is empty!");
    return;
  }

  ZoomBoxMQTT_subTopics[ZoomBoxMQTT_subTopicsCount++] = topic;

  Serial.print("Added topic for subscribing ");
  Serial.println(topic);
}

void ZoomBoxMQTT_connect() {
  // If we're not yet connected, reconnect
  while (!ZoomBoxMQTT_client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID
    String clientId = "ArduinoClient-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (ZoomBoxMQTT_client.connect(clientId.c_str(), ZoomBoxMQTT_username, ZoomBoxMQTT_key)) {
      // Connection successful
      Serial.println("successful!");

      // Subscribe to desired topics
      for (int i = 0; i < ZoomBoxMQTT_subTopicsCount; i++) {
        ZoomBoxMQTT_client.subscribe(ZoomBoxMQTT_subTopics[i]);
      }
    }
    else {
      // Connection failed. Try again.
      Serial.print("failed, state = ");
      Serial.print(ZoomBoxMQTT_client.state());
      Serial.println(". Trying again in 5 seconds.");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void ZoomBoxMQTT_loop() {
  // Process incoming messages and maintain connection to MQTT server
  ZoomBoxMQTT_client.loop();
}

bool ZoomBoxMQTT_publish(const char *topic, const char *payload) {
  return ZoomBoxMQTT_client.publish(topic, payload);
}

bool ZoomBoxMQTT_publish(const char *topic, const char payload) {
  return ZoomBoxMQTT_publish(topic, String(payload).c_str());
}

void ZoomBoxMQTT_callback(char *topic, byte *payload, unsigned int payloadLength) {

  char message = (char) payload[0];
  char friendId = (char) payload[2];

  Serial.print("ZoomBoxMQTT_callback [");
  Serial.print(topic);
  Serial.print("] message=");
  Serial.print(message);
  Serial.print(", friendId=");
  Serial.print(friendId);
  Serial.println();


  if (message == Friend::AVAILABLE) {
    eventManager.queueEvent(EVENT_FRIEND_AVAILABLE, friendId);
  } else if (message == Friend::START_CALL) {
    eventManager.queueEvent(EVENT_FRIEND_STARTED_CALL, friendId);
  } else if (message == Friend::LEAVE_CALL) {
    eventManager.queueEvent(EVENT_FRIEND_LEFT_CALL, friendId);
  } else if (message = Friend::UNAVAILABLE) {
    eventManager.queueEvent(EVENT_FRIEND_UNAVAILABLE, friendId);
  }
}
