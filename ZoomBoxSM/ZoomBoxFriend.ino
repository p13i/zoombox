
class Friend {
  public:
    const static char AVAILABLE = 'A';
    const static char START_CALL = 'S';
    const static char LEAVE_CALL = 'L';
    
    String name;
    String feed;
    int id;
    Friend(String name, String feed, int id) {
      this->name = name;
      this->feed = feed;
      this->id = id;
    }
};

Friend friends[] = { 
  Friend(
    /* name: */ "Delara",
    /* feed: */ "dmohtasham/feeds/zoombox",
    /* id: */ 1), 
  Friend(
    /* name: */ "Pramod",
    /* feed: */ "p13i/feeds/zoombox",
    /* id: */ 2), 
  Friend(
    /* name: */ "Eric",
    /* feed: */ "",
    /* id: */ 3) };

String getFriendName(int id) {
  for (int i = 0; i < ARRAY_LENGTH(friends); i++) {
    if (friends[i].id == id) {
      return friends[i].name;
    }
  }
  return "<unknown>";
}

int getFriendId(String feed) {
  for (int i = 0; i < ARRAY_LENGTH(friends); i++) {
    if (friends[i].feed.equals(feed)) {
      return friends[i].id;
    }
  }
  return -1;
}

Friend me = friends[1];

void ZoomBoxFriend_signalAvailability() {
  ZoomBoxMQTT_publish(me.feed.c_str(), Friend::AVAILABLE);
}

void ZoomBoxFriend_signalStartCall() {
  ZoomBoxMQTT_publish(me.feed.c_str(), Friend::START_CALL);
}

void ZoomBoxFriend_signalLeaveCall() {
  ZoomBoxMQTT_publish(me.feed.c_str(), Friend::LEAVE_CALL);
}
