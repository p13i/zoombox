
class Friend {
  public:
    const static char AVAILABLE = 'A';
    const static char START_CALL = 'S';
    const static char LEAVE_CALL = 'L';
    const static char UNAVAILABLE = 'U';
    
    String name;
    char id;
    int ledStartIndex;
    int ledEndIndex;
    uint32_t ledColor;
    Friend(String name, char id, int ledStartIndex, int ledEndIndex, uint32_t ledColor) {
      this->name = name;
      this->id = id;
      this->ledStartIndex = ledStartIndex;
      this->ledEndIndex = ledEndIndex;
      this->ledColor = ledColor;
    }
};

Friend friends[] = { 
  Friend(
    /* name: */ "Delara",
    /* id: */ 'D',
    /* ledStartIndex: */ 0,
    /* ledEndIndex: */ 2,
    /* ledColor: */ pixels.Color(0, 0, 255)), 
  Friend(
    /* name: */ "Pramod",
    /* id: */ 'P',
    /* ledStartIndex: */ 3,
    /* ledEndIndex: */ 5,
    /* ledColor: */ pixels.Color(255, 255, 0)), 
  Friend(
    /* name: */ "Eric",
    /* id: */ 'E',
    /* ledStartIndex: */ 6,
    /* ledEndIndex: */ 8,
    /* ledColor: */ pixels.Color(255, 0, 255)) };

String getFriendName(char id) {
  for (int i = 0; i < ARRAY_LENGTH(friends); i++) {
    if (friends[i].id == id) {
      return friends[i].name;
    }
  }
  return "<unknown>";
}

int getFriendLedStartIndex(char id) {
  for (int i = 0; i < ARRAY_LENGTH(friends); i++) {
    if (friends[i].id == id) {
      return friends[i].ledStartIndex;
    }
  }
  return -1;
}

int getFriendLedEndIndex(char id) {
  for (int i = 0; i < ARRAY_LENGTH(friends); i++) {
    if (friends[i].id == id) {
      return friends[i].ledEndIndex;
    }
  }
  return -1;
}

uint32_t getFriendLedColor(char id) {
  for (int i = 0; i < ARRAY_LENGTH(friends); i++) {
    if (friends[i].id == id) {
      return friends[i].ledColor;
    }
  }
  return -1;
}

Friend me = friends[ZoomBoxFriend_MeIndex];

void ZoomBoxFriend_signalAvailability() {
  String message = String(Friend::AVAILABLE) + " " + String(me.id);
  ZoomBoxMQTT_publish(ZoomBoxMQTT_SharedFeed, message.c_str());
}

void ZoomBoxFriend_signalStartCall() {
  String message = String(Friend::START_CALL) + " " + String(me.id);
  ZoomBoxMQTT_publish(ZoomBoxMQTT_SharedFeed, message.c_str());
}

void ZoomBoxFriend_signalLeaveCall() {
  String message = String(Friend::LEAVE_CALL) + " " + String(me.id);
  ZoomBoxMQTT_publish(ZoomBoxMQTT_SharedFeed, message.c_str());
}

void ZoomBoxFriend_signalUnavailable() {
  String message = String(Friend::UNAVAILABLE) + " " + String(me.id);
  ZoomBoxMQTT_publish(ZoomBoxMQTT_SharedFeed, message.c_str());
}
