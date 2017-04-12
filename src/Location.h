#ifndef LOCATION_H
#define LOCATION_H

enum class Location : unsigned char {
    DECK,
    HAND0,
    HAND1,
    IN_PLAY,
    EXECUTING,
    GRAVEYARD,
    NOWHERE
};

static const char *show_location(Location l) {
    switch (l) {
    case Location::HAND0:
        return "HAND0";
    case Location::HAND1:
        return "HAND1";
    case Location::IN_PLAY:
        return "IN_PLAY";
    case Location::GRAVEYARD:
        return "GRAVEYARD";
    default:
        return "<ERROR>";
    }
}

inline Location HAND(int i) {
    return Location(int(Location::HAND0)+i);
}

#endif
