#ifndef CARDS_H
#define CARDS_H

// TODO
// Avenging Angel
// Giant
// Flowers of Evil
// Terrible Lizard
// Warp Spasm
// Harpies
// Clock
// Clock2

inline bool isCard(int c) {
    return c >= 0 && c < 1000;
}

class Pop : public SpellDefinition {
public:
    Pop() : SpellDefinition("Haste", "⬇️️ ", 0, 0, 2, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER | CardClass::SPELL | CardClass::ARTIFACT,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};

const class JestersWish : public SpellDefinition {
public:
    JestersWish() : SpellDefinition("Jester's Wish", "🃏 ", 0, 0, 3, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::PLAYER,
                    CardProperty::INSTANT,
                    CardProperty::NONE, /* requirements */
                    CardProperty::NONE) { }
    virtual void executeInstant(SpellCaster *game, int c, bool verbose) const;
} jestersWish;

const class PerpetualMachine : public PerpetualMachineBase {
public:
    PerpetualMachine() : PerpetualMachineBase("Perpetual Machine", "⚙️ ", +1, 4, 4, 1, 1, 6,
                    CardClass::ARTIFACT,
                    CardClass::PLAYER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::NONE) { }
} perpetualMachine;

const class ManaWheel : public PerpetualMachineBase {
public:
    ManaWheel() : PerpetualMachineBase("Mana Wheel", "⚙️ ", +1, 2, 2, 1, 1, 3,
                    CardClass::ARTIFACT,
                    CardClass::PLAYER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::NONE) { }
} manaWheel;

class Push : public SpellDefinition {
public:
    Push() : SpellDefinition("Sloth", "⬆️️ ", 0, 1, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};

const class Darkness : public SpellDefinition {
public:
    Darkness() : SpellDefinition("Darkness", "🌘 ", +1, 1, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::UNDEAD,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} darkness;

const class BlueSkies : public SpellDefinition {
public:
    BlueSkies() : SpellDefinition("Blue Skies", "☁️ ", +1, 0, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::FLYING,
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} blueSkies;

const class Plague : public SpellDefinition {
public:
    Plague() : SpellDefinition("Plague", "☣️ ", -1, 2, 0, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::ETHEREAL) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} plague;

#if 0
const class OffByOne : public SpellDefinition {
public:
    OffByOne() : SpellDefinition("Off By One", "➡️ ", -1, 2, 0, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER | CardClass::SPELL | CardClass::ARTIFACT,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} offByOne;
#endif

#if 0
const class ARollingStone : public SpellDefinition {
public:
    ARollingStone() : SpellDefinition("A Rolling Stone...", "🗻 ", -1, 4, 0, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FLYING | CardProperty::ETHEREAL) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};
#endif

const class Ambush : public SpellDefinition {
public:
    Ambush() : SpellDefinition("Ambush", "💢 ", -1, 2, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} ambush;

const class Goblin : public MonsterDefinition {
public:
    Goblin() : MonsterDefinition("Goblin", "👺 ", -1, 1, 0, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} goblin;

const class Archers : public MonsterDefinition {
public:
    Archers() : MonsterDefinition("Archers", "🏹 ", -1, 2, 0, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::NONE | CardProperty::ETHEREAL) { }
} archers;

const class BlessedLion : public MonsterDefinition {
public:
    BlessedLion() : MonsterDefinition("Blessed Lion", "🦁 ", -1, 1, 1, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::BLESSED | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} blessedLion;

const class ElephantMan : public MonsterDefinition {
public:
    ElephantMan() : MonsterDefinition("Elephant Man", "🐘 ", -1, 2, 1, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} elephantMan;

const class Kraken : public MonsterDefinition {
public:
    Kraken() : MonsterDefinition("Kraken", "🦑 ", -1, 3, 2, 4, 4, 3,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} kraken;

const class Dwarf : public MonsterDefinition {
public:
    Dwarf() : MonsterDefinition("Dwarf", "⛏️ ", -1, 2, 0, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} dwarf;

const class Skeleton : public MonsterDefinition {
public:
    Skeleton() : MonsterDefinition("Skeleton", "☠️ ", -1, 1, 1, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::UNDEAD | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::BLESSED | CardProperty::ETHEREAL) { }
} skeleton;

const class SkeletonLord : public MonsterDefinition {
public:
    SkeletonLord() : MonsterDefinition("Skeleton Lord", "☠️ ", -1, 2, 2, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::UNDEAD | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::BLESSED | CardProperty::ETHEREAL) { }
    virtual int computeAttack(SpellCaster *game, int card, int target, bool verbose) const override;
} skeletonLord;

const class AvengingAngel : public MonsterDefinition {
public:
    AvengingAngel() : MonsterDefinition("Avenging Angel", "👼 ", -1, 2, 2, 3, 3, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::FLYING,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
    virtual int computeAttack(SpellCaster *game, int card, int target, bool verbose) const override;
} avengingAngel;

const class AngryGhost : public MonsterDefinition {
public:
    AngryGhost() : MonsterDefinition("Angry Ghost", "👻 ", -1, 0, 2, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::UNDEAD | CardProperty::FEARLESS | CardProperty::ETHEREAL,
                    CardProperty::NONE,
                    CardProperty::BLESSED) { }
} angryGhost;

const class Wraith : public MonsterDefinition {
public:
    Wraith() : MonsterDefinition("Wraith", "☠️ ", -1, 2, 2, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::UNDEAD | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::BLESSED) { }
} wraith;

const class Wight : public MonsterDefinition {
public:
    Wight() : MonsterDefinition("Wight", "👽 ", -1, 2, 2, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::UNDEAD | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::BLESSED) { }
} wight;

const class Spectre : public MonsterDefinition {
public:
    Spectre() : MonsterDefinition("Spectre", "👽 ", -1, 2, 2, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::UNDEAD | CardProperty::FEARLESS | CardProperty::ETHEREAL,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::BLESSED) { }
} spectre;

// Can fly, but if involved in combat then it's not in flying form.
// Need XXX to make vampiric again
const class Vampire : public VampiricMonster {
public:
    Vampire() : VampiricMonster("Vampire", "🦇 ", -1, 2, 2, 1, 1, 2,
                    CardClass::MONSTER, /* is a */
                    CardClass::PLAYER | CardClass::MONSTER, /* targets */
                    CardProperty::UNDEAD | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::BLESSED | CardProperty::ETHEREAL) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} vampire;

// Can capture flying with web
const class Spider : public PoisonMonster {
public:
    Spider() : PoisonMonster("Spider", "🕷 ", -1, 2, 0, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::ETHEREAL) { }
} spider;

const class Scorpion : public PoisonMonster {
public:
    Scorpion() : PoisonMonster("Scorpion", "🦂 ", -1, 2, 0, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} scorpion;

const class Blowfish : public PoisonMonster {
public:
    Blowfish() : PoisonMonster("Blowfish", "🐡 ", -1, 3, 0, 1, 1, 3,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING) { }
} blowfish;

const class Ogre : public MonsterDefinition {
public:
    Ogre() : MonsterDefinition("Ogre", "👹 ", -1, 2, 0, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} ogre;

const class RedDragon : public MonsterDefinition {
public:
    RedDragon() : MonsterDefinition("Red Dragon", "🐊 ", -1, 5, 0, 4, 4, 3,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::FLYING | CardProperty::FEARLESS | CardProperty::RED_MAGIC_RESISTANT,
                    CardProperty::NONE,
                    CardProperty::ETHEREAL) { }
} redDragon;

const class BlueDragon : public MonsterDefinition {
public:
    BlueDragon() : MonsterDefinition("Blue Dragon", "🐊 ", -1, 0, 5, 4, 4, 3,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::FLYING | CardProperty::FEARLESS | CardProperty::BLUE_MAGIC_RESISTANT,
                    CardProperty::NONE,
                    CardProperty::ETHEREAL) { }
} blueDragon;

const class Giant : public MonsterDefinition {
public:
    Giant() : MonsterDefinition("Giant", "👹 ", -1, 3, 1, 3, 3, 3,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::ETHEREAL) { }
} giant;

const class Troll : public MonsterDefinition {
public:
    Troll() : MonsterDefinition("Troll", "👹 ", -1, 3, 1, 3, 3, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::REGENERATING,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} troll;

const class Hydra : public GhoulBase {
public:
    Hydra() : GhoulBase("Hydra", "🐍 ", -1, 3, 2, 3, 3, 3,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::REGENERATING,
                    CardProperty::NONE,
                    CardProperty::ETHEREAL) { }
} hydra;

const class WormDemon : public MonsterDefinition {
public:
    WormDemon() : MonsterDefinition("Worm Demon", "🐛 ", -1, 2, 0, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} wormDemon;

const class Stampede : public MonsterDefinition {
public:
    Stampede() : MonsterDefinition("Stampede", "🐘 ", -1, 4, 0, 3, 3, 4,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} stampede;

const class Unicorn : public MonsterDefinition {
public:
    Unicorn() : MonsterDefinition("Unicorn", "🦄 ", -1, 1, 1, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::BLESSED,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} unicorn;

// Need neutral positivity because casting on self is good.
const class RatSwarm : public MonsterDefinition {
public:
    RatSwarm() : MonsterDefinition("Rat Swarm", "🐀 ", 0, 2, 0, 1, 1, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::COMBINABLE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
    int key () const { return 1; }
} ratSwarm;

const class BatColony : public MonsterDefinition {
public:
    BatColony() : MonsterDefinition("Bat Colony", "🦇 ", 0, 2, 0, 1, 1, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::COMBINABLE | CardProperty::FLYING,
                    CardProperty::NONE,
                    CardProperty::ETHEREAL) { }
    int key () const { return 4; }
} batColony;

// Need neutral positivity
const class AntSwarm : public MonsterDefinition {
public:
    AntSwarm() : MonsterDefinition("Ant Swarm", "🐜 ", 0, 2, 0, 2, 2, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::COMBINABLE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
    int key () const { return 2; }
} antSwarm;

const class LocustPlague : public MonsterDefinition {
public:
    LocustPlague() : MonsterDefinition("Locust Plague", "🐜 ", 0, 2, 0, 2, 2, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::COMBINABLE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
    int key () const { return 2; }
} locustPlague;

const class ZombieHorde : public MonsterDefinition {
public:
    ZombieHorde() : MonsterDefinition("Zombie Horde", "💀 ", 0, 2, 0, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::COMBINABLE | CardProperty::UNDEAD | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::BLESSED | CardProperty::ETHEREAL) { }
    int key () const { return 3; }
} zombieHorde;

const class AutomatonArmy : public MonsterDefinition {
public:
    AutomatonArmy() : MonsterDefinition("Automaton Army", "⚙️ ", 0, 2, 0, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::COMBINABLE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
    int key () const { return 4; }
} automatonArmy;

const class Snake : public MonsterDefinition {
public:
    Snake() : MonsterDefinition("Snake", "🐍 ", -1, 2, 0, 1, 1, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    /* Exclusions */ CardProperty::FLYING | CardProperty::ETHEREAL) { }
} snake;

const class Roc : public MonsterDefinition {
public:
    Roc() : MonsterDefinition("Roc", "🦅 ", -1, 2, 2, 3, 3, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    /* Properties */ CardProperty::FLYING,
                    /* Requirements */ CardProperty::NONE,
                    /* Exclusions */ CardProperty::NONE | CardProperty::ETHEREAL) { }
} roc;

const class Dragon : public MonsterDefinition {
public:
    Dragon() : MonsterDefinition("Dragon", "🐉 ", -1, 2, 2, 3, 3, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::FIRE_RESISTANT | CardProperty::FLYING,
                    CardProperty::NONE,
                    CardProperty::NONE | CardProperty::ETHEREAL) { }
} dragon;

const class Strength : public StrengthBase {
public:
    Strength() : StrengthBase("Strength", "💪 ", +1, 2, 0, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT) { }
} strength;

const class FightingManual : public StrengthBase {
public:
    FightingManual() : StrengthBase("Fighting Manual", "📜 ", +1, 2, 0, 1, 1, 1,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT) { }
} fightingManual;

#if 0
const class OdinsWhetstone : public StrengthBase {
public:
    OdinsWhetstone() : StrengthBase("Odin's Whetstone", "💪 ", +1, 2, 0, 1, 1, 1,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER | CardClass::ARTIFACT,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
};
#endif

const class Improve : public StrengthBase {
public:
    Improve() : StrengthBase("Improve", "✨ ", +1, 1, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::SPELL,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} improve;

const class Weakling : public SpellDefinition {
public:
    Weakling() : SpellDefinition("Weakling", "🥀 ", -1, 1, 1, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} weakling;

const class Volcano : public BlastBase {
public:
    Volcano() : BlastBase("Volcano", "🌋 ", -1, 4, 1, 0, 0, 3,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FIRE_RESISTANT) { }
} volcano;

const class AnEvilBlast : public FollowThroughBase {
public:
    AnEvilBlast() : FollowThroughBase("An Evil Blast", "💢 ", -1, 1, 3, 0, 0, 2,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::BLESSED) { }
} anEvilBlast;

const class Daylight : public BlastBase {
public:
    Daylight() : BlastBase("Daylight", "🌞 ", -1, 2, 2, 0, 0, 3,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::UNDEAD,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} _daylight;

const class HolySymbol : public ArtifactDefinition {
public:
    HolySymbol() : ArtifactDefinition("Holy Symbol", "☦ ", -1, 1, 2, 0, 0, 1,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::UNDEAD, // Can only target undead.
                    CardProperty::NONE) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} holySymbol;

const class Unspell : public DestroyBase {
public:
    Unspell() : DestroyBase("Unspell", "🔮 ", -1, 1, 2, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::SPELL,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} unspell;

const class SmashArtifact : public DestroyBase {
public:
    SmashArtifact() : DestroyBase("Smash Artifact", "💢 ", -1, 2, 1, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::ARTIFACT,   /* target */
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::ETHEREAL) { }
} smashArtifact;

const class SuddenDeath : public DestroyBase {
public:
    SuddenDeath() : DestroyBase("Sudden Death", "⚰️ ", -1, 3, 2, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,   /* target */
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::BLESSED) { }
} suddenDeath;

const class Double : public DoubleBase {
public:
    Double() : DoubleBase("Double", "➁ ", +1, 1, 2, 0, 0, 2,
                    CardClass::SPELL,
                    CardClass::SPELL,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} _double;

const class UpgradeArtifact : public DoubleBase {
public:
    UpgradeArtifact() : DoubleBase("Upgrade Artifact", "⚙️ ", +1, 2, 1, 0, 0, 2,
                    CardClass::SPELL,
                    CardClass::ARTIFACT,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} upgradeArtifact;

const class Fortress : public ShieldBase {
public:
    Fortress() : ShieldBase("Fortress", "🏯 ", +1, 2, 0, 1, 1, 1,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT) { }
} fortress;

const class Fear : public ReturnBase {
public:
    Fear() : ReturnBase("Fear", "😱 ", -1, 0, 1, 2, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FEARLESS) { }
} fear;

const class Misfire : public ReturnBase {
public:
    Misfire() : ReturnBase("Misfire", "💥 ", -1, 0, 2, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::SPELL,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
} misfire;

const class Shield : public ShieldBase {
public:
    Shield() : ShieldBase("Shield", "🛡 ", +1, 2, 0, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT) { }
} shield;

const class Sickness : public SpellDefinition {
public:
    Sickness() : SpellDefinition("Sickness", "🤢 ", -1, 2, 0, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} sickness;

const class Bribe : public TakeBase {
public:
    Bribe() : TakeBase("Bribe", "💰 ", -1, 3, 1, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} bribe;

const class Steal : public TakeBase {
public:
    Steal() : TakeBase("Steal", "👀 ", -1, 3, 1, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::ARTIFACT,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} steal;

const class Library : public TakeBase {
public:
    Library() : TakeBase("Library", "📚 ", -1, 0, 4, 0, 0, 1,
                    CardClass::ARTIFACT,
                    CardClass::SPELL,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
} library;

const class MakeArtifact : public SpellDefinition {
public:
    MakeArtifact() : SpellDefinition("Make Artifact", "🔨 ", +1, 2, 1, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::SPELL,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} makeArtifact;

const class Suspend : public SpellDefinition {
public:
    Suspend() : SpellDefinition("Suspend", "⚖️ ", 0, 0, 2, 0, 0, 1,
                    CardClass::ARTIFACT,
                    CardClass::SPELL,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} suspend;

const class Excalibur : public MagicWeapon {
public:
    Excalibur() : MagicWeapon("Excalibur", "⚔️ ", -1, 2, 0, 0, 0, 1,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING) { }
} excalibur;

const class DrainPower : public SpellDefinition {
public:
    DrainPower() : SpellDefinition("Drain Power", "🌀 ", -1, 0, 2, 0, 0, 2,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} drainPower;

const class Sacrifice : public SpellDefinition {
public:
    Sacrifice() : SpellDefinition("Sacrifice", "🗡 ", 0, 1, 1, 0, 0, 3,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::UNDEAD) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} sacrifice;

const class Crocodile : public MonsterDefinition {
public:
    Crocodile() : MonsterDefinition("Crocodile", "🐊 ", -1, 3, 0, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} crocodile;

const class GiantBoar : public MonsterDefinition {
public:
    GiantBoar() : MonsterDefinition("Giant Boar", "🐗 ", -1, 2, 0, 1, 1, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} giantBoar;

const class GiantLizard : public MonsterDefinition {
public:
    GiantLizard() : MonsterDefinition("Giant Lizard", "🦎 ", -1, 3, 0, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} giantLizard;

const class Octopus : public ImmobilisingMonsterDefinition {
public:
    Octopus() : ImmobilisingMonsterDefinition("Octopus", "🐙 ", -1, 3, 0, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} octopus;

const class GelatinousCube : public ImmobilisingMonsterDefinition {
public:
    GelatinousCube() : ImmobilisingMonsterDefinition("Gelatinous Cube", "⬜ ", -1, 2, 2, 3, 3, 3,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} gelatinousCube;

const class ManOWar : public ImmobilisingMonsterDefinition {
public:
    ManOWar() : ImmobilisingMonsterDefinition("Man O' War", "⬜ ", -1, 2, 1, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} manOWar;

const class Blast : public BlastBase {
public:
    Blast() : BlastBase("Blast", "💥 ", -1, 1, 1, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FIRE_RESISTANT) { }
} blast;

const class AnIllWind : public BlastBase {
public:
    AnIllWind() : BlastBase("An Ill Wind", "🌬️ ", -1, 1, 1, 0, 0, 3,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    /* Requirements */ CardProperty::FLYING,
                    /* Exclusions */ CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT
                    ) { }
} anIllWind;

const class Tornado : public BlastBase {
public:
    Tornado() : BlastBase("Tornado", "🌪 ", -1, 2, 2, 0, 0, 2,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} tornado;

const class Freeze : public BlastBase {
public:
    Freeze() : BlastBase("Freeze", "❄️ ", -1, 3, 0, 0, 0, 2,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT) { }
} freeze;

const class Lightning : public BlastBase {
public:
    Lightning() : BlastBase("Lightning", "⚡ ", -1, 2, 2, 0, 0, 4,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::FLYING,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual int computeAttack(SpellCaster *game, int card, int target, bool verbose) const override;
} lightning;

const class Comet : public BlastBase {
public:
    Comet() : BlastBase("Comet", "☄ ", -1, 3, 2, 0, 0, 4,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} comet;

const class ShootingStars : public BlastBase {
public:
    ShootingStars() : BlastBase("Shooting Stars", "✨ ", -1, 4, 0, 0, 0, 4,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT) { }
} shootingStars;

const class Golem : public MonsterDefinition {
public:
    Golem() : MonsterDefinition("Golem", "🗿 ", -1, 4, 0, 2, 2, 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }
} golem;

const class Ghoul : public GhoulBase {
public:
    Ghoul() : GhoulBase("Ghoul", "👻 ", -1, 1, 1, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::UNDEAD | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::BLESSED | CardProperty::ETHEREAL) { }
} ghoul;

const class SpellEater : public MonsterDefinition {
public:
    SpellEater() : MonsterDefinition("Spell Eater", "🌵 ", -1, 1, 1, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER | CardClass::SPELL,
                    CardProperty::UNDEAD | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::BLESSED | CardProperty::ETHEREAL) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} spellEater;

// Works even with magic resistance
const class HolyWater : public BlastBase {
public:
    HolyWater() : BlastBase("Holy Water", "💦 ", -1, 1, 1, 0, 0, 2,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::UNDEAD, /* requirements */
                    CardProperty::NONE) { }
} holyWater;

const class Tempest : public BlastBase {
public:
    Tempest() : BlastBase("Tempest", "🌩️ ", -1, 2, 2, 0, 0, 3,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::NONE) { }
} tempest;

const class Djinn : public SummonMonster {
public:
    Djinn() : SummonMonster("Djinn", "👳🏽 ", -1, 1,1,  1, 1, 2,
                    CardClass::SPELL,
                    CardClass::PLAYER | CardClass::MONSTER,
                    /* Properties */ CardProperty::FEARLESS,
                    /* Requirements */ CardProperty::NONE,
                    /* Exclusions */ CardProperty::FLYING) { }
} djinn;

const class FireElemental : public SummonMonster {
public:
    FireElemental() : SummonMonster("Fire Elemental", "🔥 ", -1, 1, 3, 2, 2, 3,
                    CardClass::SPELL,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::FEARLESS | CardProperty::FIRE_RESISTANT,
                    CardProperty::NONE,
                    CardProperty::FIRE_RESISTANT) { }
} fireElemental;

const class Mimic : public MonsterDefinition {
public:
    Mimic() : MonsterDefinition("Mimic", "🃏 ", -1, 1, 1, 1, 1, 1,
                    CardClass::MONSTER, /* is a */
                    CardClass::PLAYER | CardClass::MONSTER, /* targets */
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} mimic;

const class Mephistopheles : public MonsterDefinition {
public:
    Mephistopheles() : MonsterDefinition("Mephistopheles", "👿 ", -1, 2, 2, 3, 3, 3,
                    CardClass::MONSTER, /* is a */
                    CardClass::MONSTER, /* targets */
                    CardProperty::UNDEAD,
                    CardProperty::NONE,
                    CardProperty::BLESSED) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} mephistopheles;

const class VorpalBunny : public MonsterDefinition {
public:
    VorpalBunny() : MonsterDefinition("Vorpal Bunny", "🐰 ", -1, 2, 1, 1, 1, 3,
                    CardClass::MONSTER, /* is a */
                    CardClass::MONSTER, /* targets */
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
} vorpalBunny;

const class MagicSpear : public MagicWeapon {
public:
    MagicSpear() : MagicWeapon("Magic Spear", "⚔️ ", -1, 3, 1, 1, 1, 2,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER | CardClass::SPELL,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::FLYING) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} magicSpear;

const class NeptunesTrident : public MagicWeapon {
public:
    NeptunesTrident() : MagicWeapon("Neptune's Trident", "🔱 " ,-1, 2, 1,  1, 1, 3,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::NONE | CardProperty::ETHEREAL) { }
} neptunesTrident;

const class ArtemisBow : public MagicWeapon {
public:
    ArtemisBow() : MagicWeapon("Artemis' Bow", "🏹 ", -1,  3, 1, 1, 1, 3,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::NONE) { }
    virtual int computeAttack(SpellCaster *game, int card, int target, bool verbose) const;
} artemisBow;

const class ResistAstralMagic : public AntiAstralBase {
public:
    ResistAstralMagic() : AntiAstralBase("Resist Astral Magic", "✨ ", 0, 2, 0, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::RED_MAGIC_RESISTANT) { }
} resistAstralMagic;

const class ResistWorldlyMagic : public AntiWorldlyBase {
public:
    ResistWorldlyMagic() : AntiWorldlyBase("Resist Worldly Magic", "✨ ", 0, 0, 2, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
} resistWorldlyMagic;

const class Flight : public SpellDefinition {
public:
    Flight() : SpellDefinition("Flight", "🦅 ", 1, 1, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FLYING) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} flight;

const class FountainOfYouth : public SpellDefinition {
public:
    FountainOfYouth() : SpellDefinition("Fountain of Youth", "⛲ ", 1, 0, 4, 1, 1, 1,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::REGENERATING) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} fountainOfYouth;

const class Ground : public SpellDefinition {
public:
    Ground() : SpellDefinition("Ground", "🗻 ", -1, 2, 0, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::FLYING, /* requirements */
                    CardProperty::RED_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} ground;

const class Rage : public SpellDefinition {
public:
    Rage() : SpellDefinition("Rage", "🍄 ", 0, 1, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} rage;

const class EtherealRing : public SpellDefinition {
public:
    EtherealRing() : SpellDefinition("Ethereal Ring", "💍 ", 1, 0, 3, 1, 1, 1,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER | CardClass::ARTIFACT,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} etherealRing;

const class Link : public SpellDefinition {
public:
    Link() : SpellDefinition("Link", "🔗 ", -1, 0, 2, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} _link;

const class Curse : public SpellDefinition {
public:
    Curse() : SpellDefinition("Curse", "👁 ", -1, 0, 2, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::UNDEAD) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} curse;

const class DeathsDoor : public ImminentDeathBase {
public:
    DeathsDoor() : ImminentDeathBase("Death's Door", "🚪 ", -1, 1, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::BLESSED) { }
} deathsDoor;

const class Banshee : public ImminentDeathBase {
public:
    Banshee() : ImminentDeathBase("Banshee", "🎶 ", -1, 0, 4, 1, 1, 1,
                    CardClass::MONSTER,
                    CardClass::MONSTER,
                    CardProperty::UNDEAD,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::BLESSED) { }
} banshee;

const class Sleep : public SleepBase {
public:
    Sleep() : SleepBase("Sleep", "💤 ", -1, 0, 2, 1, 1, 2,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::UNDEAD) { }
} _sleep;

#if 0
const class Gleipnir : public SleepBase {
public:
    Gleipnir() : SleepBase("Gleipnir", "⛓️ ", -1, 3, 1, 1, 1, 4,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::ETHEREAL) { }
};
#endif

const class Web : public SleepBase {
public:
    Web() : SleepBase("Web", "🕸️ ", -1, 3, 0, 1, 1, 4,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::ETHEREAL) { }
} web;

const class Loyalty : public SpellDefinition {
public:
    Loyalty() : SpellDefinition("Loyalty", "👑 ", +1, 0, 2, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
} loyalty;

const class Spirit : public MonsterDefinition {
public:
    Spirit() : MonsterDefinition("Spirit", "👻 ", 
                                 /* positivity */ -1,
                                 /* world_cost */ 0,
                                 /* astral_cost */ 2,
                                 /* basehp */ 2, 2,
                                 /* attack */ 3,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::LINKED,
                    CardProperty::NONE,
                    CardProperty::NONE) { }
} spirit;

const class Shard : public ShardBase {
public:
    Shard() : ShardBase("Shard", "💠 ", -1, 3, 1, 0, 0, 3,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }
} shard;

const class FieryChariot : public ShardBase {
public:
    FieryChariot() : ShardBase("Fiery Chariot", "⚙️ ", -1, 4, 1, 0, 0, 4,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FIRE_RESISTANT) { }
} fieryChariot;

#if 0
const class Cavalry : public MonsterDefinition {
public:
    Cavalry() : MonsterDefinition("Cavalry", "🐎 ",
                                  /* positivity */ 0,
                                  /* cost */ 2,
                                  /* basehp */ 2, 2,
                                  /* attack */ 2,
                    CardClass::MONSTER,
                    CardClass::PLAYER | CardClass::MONSTER,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FEARLESS,
                    CardProperty::NONE,
                    CardProperty::FLYING) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};
#endif

#endif
