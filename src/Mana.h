#ifndef MANA_H
#define MANA_H

#include <algorithm>

using std::max;

struct Mana {
    int world;
    int astral;
};

inline bool operator<(const Mana &mana0, const Mana &mana1) {
    return mana0.world < mana1.world &&
           mana0.astral < mana1.astral;
}

inline bool operator<=(const Mana &mana0, const Mana &mana1) {
    return mana0.world <= mana1.world &&
           mana0.astral <= mana1.astral;
}

inline bool operator>(const Mana &mana0, const Mana &mana1) {
    return mana0.world > mana1.world &&
           mana0.astral > mana1.astral;
}

inline bool operator>=(const Mana &mana0, const Mana &mana1) {
    return mana0.world >= mana1.world &&
           mana0.astral >= mana1.astral;
}

inline bool operator==(const Mana &mana0, const Mana &mana1) {
    return mana0.world == mana1.world &&
           mana0.astral == mana1.astral;
}

inline Mana &operator+=(Mana &mana0, const Mana &mana1) {
    mana0.world += mana1.world;
    mana0.astral += mana1.astral;
    return mana0;
}

inline Mana &operator-=(Mana &mana0, const Mana &mana1) {
    mana0.world -= mana1.world;
    mana0.astral -= mana1.astral;
    return mana0;
}

inline Mana max(const Mana &mana0, const Mana &mana1) {
    return Mana{max(mana0.world, mana1.world), max(mana0.astral, mana1.astral)};
}

inline Mana operator-(const Mana &mana0, const Mana &mana1) {
    return Mana{mana0.world-mana1.world, mana0.astral-mana1.astral};
}

inline Mana operator+(const Mana &mana0, const Mana &mana1) {
    return Mana{mana0.world+mana1.world, mana0.astral+mana1.astral};
}

#endif
