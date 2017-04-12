#ifndef CARD_CLASS_H
#define CARD_CLASS_H

enum class CardClass : unsigned char {
    PLAYER   = 1,
    MONSTER  = 2,
    SPELL    = 4,
    ARTIFACT = 8,
    MANA     = 16
};

inline CardClass operator&(CardClass a, CardClass b) {
    return CardClass(int(a) & int (b));
}

inline CardClass operator|(CardClass a, CardClass b) {
    return CardClass(int(a) | int (b));
}

inline CardClass &operator&=(CardClass &a, CardClass b) {
    return a = CardClass(int(a) & int (b));
}

inline CardClass &operator|=(CardClass &a, CardClass b) {
    return a = CardClass(int(a) | int (b));
}

inline CardClass operator~(CardClass a) {
    return CardClass(~int(a));
}

inline bool toBool(CardClass a) {
    return int(a) != 0;
}

#endif
