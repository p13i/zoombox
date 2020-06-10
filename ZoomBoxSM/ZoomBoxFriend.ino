
class Friend {
  public:
    const static char AVAILABLE = 'A';
    const static char START_CALL = 'S';
    const static char LEAVE_CALL = 'L';
    
    String name;
    char id;
    Friend(String name, char id) {
      this->name = name;
      this->id = id;
    }
};

Friend friends[] = { 
  Friend(
    /* name: */ "Delara",
    /* id: */ 'D'), 
  Friend(
    /* name: */ "Pramod",
    /* id: */ 'P'), 
  Friend(
    /* name: */ "Eric",
    /* id: */ 'E') };

String getFriendName(char id) {
  for (int i = 0; i < ARRAY_LENGTH(friends); i++) {
    if (friends[i].id == id) {
      return friends[i].name;
    }
  }
  return "<unknown>";
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
