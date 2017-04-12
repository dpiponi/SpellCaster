#ifndef CARD_PROPERTY_H
#define CARD_PROPERTY_H

#include <string>
#include <sstream>

using std::ostringstream;
using std::string;

enum class CardProperty : unsigned short {
    NONE            = 0,
    UNDEAD          = 1,
    MAGIC_RESISTANT = 2,
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
    INSTANT         = 8192
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
    if (toBool(property & CardProperty::MAGIC_RESISTANT)) {
        description << "Magic Resistant\n";
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
        description << "About to die\n";
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
