#ifndef CARD_PROPERTY_H
#define CARD_PROPERTY_H

#include <string>
#include <sstream>

using std::ostringstream;
using std::string;

enum class CardProperty : unsigned int {
    NONE            = 0,
    UNDEAD          = 1,
    FEARLESS        = 4,
    FLYING          = 8, 
    FIRE_RESISTANT  = 16,
    BLESSED         = 32,
    IMMOBILE        = 64,
    COMBINABLE      = 128,
    IMMINENT_DEATH  = 256,
    LINKED          = 512,
    ETHEREAL        = 1024,
    AQUATIC         = 2048,
    REGENERATING    = 4096,
    INSTANT         = 8192,
    RED_MAGIC_RESISTANT = 16384,
    BLUE_MAGIC_RESISTANT = 32768,
};

inline CardProperty operator&(CardProperty a, CardProperty b) {
    return CardProperty(int(a) & int (b));
}

inline CardProperty operator|(CardProperty a, CardProperty b) {
    return CardProperty(int(a) | int (b));
}

inline CardProperty &operator|=(CardProperty &a, CardProperty b) {
    return a = CardProperty(int(a) | int (b));
}

inline CardProperty &operator&=(CardProperty &a, CardProperty b) {
    return a = CardProperty(int(a) & int (b));
}

inline CardProperty operator~(CardProperty a) {
    return CardProperty(~int(a));
}

inline bool toBool(CardProperty a) {
    return int(a) != 0;
}

static string describe(CardProperty property) {
    ostringstream description;
    if (toBool(property & CardProperty::UNDEAD)) {
        description << "Undead\n";
    }
    if (toBool(property & CardProperty::RED_MAGIC_RESISTANT)) {
        description << "Red Resistant\n";
    }
    if (toBool(property & CardProperty::BLUE_MAGIC_RESISTANT)) {
        description << "Blue Resistant\n";
    }
    if (toBool(property & CardProperty::FEARLESS)) {
        description << "Fearless\n";
    }
    if (toBool(property & CardProperty::FLYING)) {
        description << "Flying\n";
    }
    if (toBool(property & CardProperty::FIRE_RESISTANT)) {
        description << "Fire Resistant\n";
    }
    if (toBool(property & CardProperty::BLESSED)) {
        description << "Blessed\n";
    }
    if (toBool(property & CardProperty::IMMOBILE)) {
        description << "Immobilised\n";
    }
    if (toBool(property & CardProperty::COMBINABLE)) {
        description << "Combinable\n";
    }
    if (toBool(property & CardProperty::IMMINENT_DEATH)) {
        description << "Fated";
    }
    if (toBool(property & CardProperty::LINKED)) {
        description << "Damage Linked\n";
    }
    if (toBool(property & CardProperty::ETHEREAL)) {
        description << "Ethereal\n";
    }
    if (toBool(property & CardProperty::AQUATIC)) {
        description << "Aquatic\n";
    }
    if (toBool(property & CardProperty::REGENERATING)) {
        description << "Regenerating\n";
    }
    if (toBool(property & CardProperty::INSTANT)) {
        description << "Instant\n";
    }
    return description.str();
}

#endif
