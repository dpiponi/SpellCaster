
#include <vector>
#include <map>
#include <iostream>
#include <cassert>
#include <memory>

using std::vector;
using std::ostream;
using std::istream;
using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::map;
using std::shared_ptr;
using std::make_shared;

#include "SpellCaster.h"

Config default_config {/* draw_on_turn_end */ false,
                       /* max_cards_in_hand */ 8,
                       false,
                       /* draw_cards_after_execution */ true,
                       /* auto_discard */ false,
                       /* must_self_discard */ false,
                       /* can_self_discard */ true,
                       /* auto_mana */ false,
                       /* single pass */ true,
                       /* random_draw */ false,
                       /* draw_to_fill_hand */ true,
                       /* discard_for_mana */ true,
                       /* can_discard_any_for_mana */ false};

map<string, const Definition *>
make_database(const vector<const Definition *> &definitions) {
    map<string, const Definition *> database;

    for (auto i : definitions) {
        database[i->name] = i;
    }

    return database;
}

#define VIDEO

#ifdef VIDEO
const char *normal = "";
const char *bold = "";
const char *red = "";
const char *green = "";
const char *white = "";
const char *yellow = "";
const char *blue = "";
const char *magenta = "";
const char *black_bg = "";
const char *white_bg = "";
#endif

#ifdef TERMINAL
const char *normal = "\x1b[0m";
const char *bold = "\x1b[1m";
const char *red = "\x1b[1;31m";
const char *green = "\x1b[32m";
const char *white = "\x1b[37m";
const char *yellow = "\x1b[33m";
const char *blue = "\x1b[1;34m";
const char *magenta = "\x1b[35m";
const char *black_bg = "\x1b[40m";
const char *white_bg = "\x1b[47m";
#endif

vector<const Definition *>
read_deck(const map<string, const Definition *> &database,
          const char *filename) {
    string line;
    ifstream infile(filename);
    vector<const Definition *> deck;
    int i = 0;
    while (getline(infile, line)) {
        if (!line.empty() && line[line.length()-1] == '\n') {
            line.erase(line.length()-1);
        }
        auto p = database.find(line);
        if (p != database.end()) {
            deck.push_back(p->second);
            ++i;
        } else {
            cout << "Failed to find " << line << endl;
        }
    }
    return deck;
}

AntiMagic antiMagic;
AnEvilBlast anEvilBlast;
Octopus octopus;
//OdinsWhetstone odinsWhetstone;
GiantLizard giantLizard;
Blast blast;
ManOWar manOWar;
AnIllWind anIllWind;
Bless bless;
Link link0;
Curse curse;
Bribe bribe;
Crocodile crocodile;
Daylight daylight0;
Djinn djinn;
Double double0;
Dragon dragon;
DrainPower drainPower;
ManaCard manaCard;
Henge henge;
Temple church;
ZombieHorde zombieHorde;
Fear fear;
FireElemental fireElemental;
Fortress fortress;
Freeze freeze;
MagicSpear magicSpear;
Ghoul ghoul;
FightingManual fightingManual;
ElephantMan elephantMan;
//Gleipnir gleipnir;
Plague plague;
Spirit spirit;
Archers archers;
Golem golem;
Goblin goblin;
Shard shard;
FieryChariot fieryChariot;
GiantBoar giantBoar;
PerpetualMachine perpetualMachine;
ManaWheel manaWheel;
HolySymbol holySymbol;
//ARollingStone aRollingStone;
HolyWater holyWater;
Library library;
Lightning lightning;
Darkness darkness;
BlueSkies blueSkies;
Loyalty loyalty;
Flight flight;
FountainOfYouth fountainOfYouth;
Ground ground;
MakeArtifact makeArtifact;
Mephistopheles mephistopheles;
Mimic mimic;
Misfire misfire;
Excalibur excalibur;
Troll troll;
Improve improve;
Recall recall;
Suspend suspend;
Sacrifice sacrifice;
Shield shield;
Sickness sickness;
BlessedLion blessedLion;
Ambush ambush;
Sleep sleep0;
Stampede stampede;
Ogre ogre;
WarMachine warMachine;
RedDragon redDragon;
BlueDragon blueDragon;
BrickWall brickWall;
AvengingAngel avengingAngel;
JestersWish jestersWish;
Giant giant;
WormDemon wormDemon;
Roc roc;
SmashArtifact smashArtifact;
EtherealRing etherealRing;
Push push;
Pop pop;
ArtemisBow artemisBow;
AutomatonArmy automatonArmy;
Steal steal;
Wight wight;
Spectre spectre;
Strength strength;
SuddenDeath suddenDeath;
DeathsDoor deathsDoor;
Banshee banshee;
Tornado tornado;
BiteTheHand traitor;
NeptunesTrident neptunesTrident;
Unspell unspell;
UpgradeArtifact upgradeArtifact;
Volcano volcano;
AngryGhost angryGhost;
Weakling weakling;
Rage rage;
Tempest tempest;
SpellEater spellEater;
Skeleton skeleton;
SkeletonLord skeletonLord;
AntSwarm antSwarm;
LocustPlague locustPlague;
VorpalBunny vorpalBunny;
GelatinousCube gelatinousCube;
Hydra hydra;

vector<const Definition *> all_cards = {
    &tempest,
    &ambush,
    &spellEater,
//    &aRollingStone,
    &artemisBow,
    &rage,
    &locustPlague,
    &angryGhost,
    &anEvilBlast,
    &etherealRing,
    &anIllWind,
//    &odinsWhetstone,
    &antSwarm,
    &redDragon,
    &blueDragon,
    &antiMagic,
    &archers,
    &brickWall,
    &suspend,
    &fieryChariot,
    &vorpalBunny,
    &automatonArmy,
    &hydra,
    &batColony,
    &blast,
    &bless,
    &shard,
    &link0,
    &spirit,
    &curse,
    &blessedLion,
    &blowfish,
    &bribe,
    &deathsDoor,
    &banshee,
    &manaCard,
    &henge,
    &church,
    &comet,
    &crocodile,
    &artemisBow,
    &flight,
    &fountainOfYouth,
    &darkness,
    &blueSkies,
    &fightingManual,
    &daylight0,
    &djinn,
    &double0,
    &giantBoar,
    &warMachine,
    &dragon,
    &drainPower,
    &dwarf,
    &fear,
    &fireElemental,
    &fortress,
    &freeze,
    &magicSpear,
    &manOWar,
    &ground,
    &ghoul,
    &elephantMan,
    &giantLizard,
//    &gleipnir,
    &goblin,
    &golem,
    &giantBoar,
    &holySymbol,
    &holyWater,
    &improve,
    &stampede,
    &kraken,
    &library,
    &lightning,
    &loyalty,
    &makeArtifact,
    &manaCard,
    &mephistopheles,
    &mimic,
    &misfire,
    &excalibur,
    &octopus,
    &gelatinousCube,
//    &offByOne,
    &ogre,
    &avengingAngel,
    &jestersWish,
    &giant,
    &troll,
    &wormDemon,
    &perpetualMachine,
    &manaWheel,
    &plague,
    &pop,
    &push,
    &ratSwarm,
    &recall,
    &roc,
    &sacrifice,
    &scorpion,
    &shield,
    &sickness,
    &skeleton,
    &skeletonLord,
    &sleep0,
    &smashArtifact,
    &snake,
    &spider,
    &steal,
    &strength,
    &suddenDeath,
    &tornado,
    &traitor,
    &neptunesTrident,
    &unicorn,
    &unspell,
    &upgradeArtifact,
    &vampire,
    &volcano,
    &weakling,
    &web,
    &wight,
    &spectre,
    &wraith,
    &zombieHorde
};

void SpellCaster::return_card_from(Location loc, int c, bool verbose) {
    assert(location[c] == loc);
    if (verbose) {
#if 0
        board << description(c, verbose) << " returning to hand";
        end_message();
#endif
        board.noHighlightCard(c);
    }
    location[c] = owner[c] ? Location::HAND1 : Location::HAND0;
    if (config.restore_hp_on_return || hasProperty(c, CardProperty::REGENERATING)) {
        cardhp[c] = basehp[c];
    }
    properties[c] &= ~CardProperty::IMMOBILE;
    target[c] = -1;
    in_play.erase(std::remove(in_play.begin(), in_play.end(), c), in_play.end());
    hand[owner[c]].push_back(c);
    // YYY
    if (verbose) {
        board.setUpBoard(this);
        end_message();
    }
}

void SpellCaster::card_end_from(Location loc, int c, bool verbose) {
    switch (card_class[c]) {
    case CardClass::PLAYER:
//    case CardClass::MANA:
//        assert(false);
    case CardClass::MONSTER:
        if (hasProperty(c, CardProperty::IMMINENT_DEATH)) {
            destroy_card_from(loc, c, verbose);
            break;
        } else {
            return_card_from(loc, c, verbose);
            break;
        }
    case CardClass::SPELL:
        destroy_card_from(loc, c, verbose);
        break;
    case CardClass::ARTIFACT:
        return_card_from(loc, c, verbose);
        break;
    }
}

bool SpellCaster::targetNotLegal(int c, bool verbose) {
    const char *reason = cantCardTarget(c, target[c]);
    if (reason) {
        if (verbose) {
            board << description(c) << ' ' << reason;
            end_message();
        }
        card_end_from(Location::EXECUTING, c, verbose);
        return true;
    } else {
        return false;
    }
}

bool SpellCaster::cardImmobile(int c, bool verbose) {
    if (hasProperty(c, CardProperty::IMMOBILE)) {
        if (verbose) {
            board << description(c);
            switch (card_class[c]) {
            case CardClass::PLAYER:
//            case CardClass::MANA:
//                assert(false);
            case CardClass::SPELL:
                board << " fails";
                break;
            case CardClass::ARTIFACT:
                board << " fails to operate";
                break;
            case CardClass::MONSTER:
                board << " unable to move";
                break;
            }
            end_message();
        }

        card_end_from(Location::EXECUTING, c, verbose);

        return true;
    } else {
        return false;
    }
}

bool SpellCaster::cardCombining(int c, bool verbose) {
    if (!hasProperty(c, CardProperty::COMBINABLE)) return false;
    int t = target[c];
    if (t == PLAYER0 || t == PLAYER1) return false;
    if (!hasProperty(t, CardProperty::COMBINABLE)) return false;
    if (owner[c] != owner[t]) return false;
    if (definitions[c]->key() == definitions[t]->key()) {
        basehp[t] += basehp[c];
        cardhp[t] += cardhp[c];
        attack[t] += attack[c];
        if (verbose) {
            board << description(c) << " combines with " << description(t);
            end_message();
        }
        kill_card(c);
        return true;
    } else {
        return false;
    }
}

void
SpellCaster::determinise() {
    int player = nextPlayer;
    int otherPlayer = 1-nextPlayer; // YYY swap
    shuffle(deck[player]);
    // Want to shuffle cards not exposed to player
    int n = deck[otherPlayer].size();
    // We don't need shuffle_pile. Could do directly on deck.
    //vector<int> shuffle_pile = deck[otherPlayer];
    for (auto p : hand[otherPlayer]) {
        if (!exposedTo[player][p]) {
            deck[otherPlayer].push_back(p);
        }
    }
    shuffle(deck[otherPlayer]);
    for (auto &p : hand[otherPlayer]) {
        if (!exposedTo[player][p]) {
            p = deck[otherPlayer].back();
            deck[otherPlayer].pop_back();
            location[p] = HAND(otherPlayer);
        }
    }
    for (auto p : deck[otherPlayer]) {
        location[p] = Location::DECK;
    }
    //deck[otherPlayer] = shuffle_pile;
    assert(n == deck[otherPlayer].size());
    checkConsistency();
}

void
SpellCaster::doExecution(bool verbose) {
    checkConsistency();

    // Pass triggers an execution.
    execute(verbose);
    checkConsistency();
    nextPlayer = 1-nextPlayer; // YYY swap
    passed = false;

    // Draw card
    if (config.draw_on_turn_end) {
        if (hp[0] > 0 && hp[1] > 0) {
            if (deck[nextPlayer].size() >= 1) {
                drawCard(nextPlayer, verbose);
                if (config.auto_discard) {
                    trimExcess(nextPlayer, verbose);
                }
            }
            if (config.auto_mana) {
                ++mana[nextPlayer].world;
                ++mana[nextPlayer].astral;
            }
        }
    }
    if (verbose) {
        board.setUpBoard(this);
        end_message();
    }
    checkConsistency();
    return;
}

// Do I need to check if DISCARD is legal here? XXX
// I think it's OK to make assumption that a move is legal as
// isLegal() should be called before this
//shared_ptr<const SpellCaster>
void
SpellCaster::doMove(Move m, bool verbose) {
    checkConsistency();
    assert(m.target != DISCARD || location[m.card] == Location::HAND0 || location[m.card] == Location::HAND1);

    if (m.card == PASS) {
        if (verbose) {
            board << "PASS";
            end_message();
        }

        if (config.single_pass || passed) {
            doExecution(verbose);
            checkConsistency();
            return;
        } else {
            // Pass doesn't trigger execution.
            passed = true;

            // YYY swap player
            nextPlayer = 1-nextPlayer;
            if (config.draw_on_turn_end) {
                // DRAW
                if (hp[0] > 0 && hp[1] > 0) {
                    if (deck[nextPlayer].size() >= 1) {
                        drawCard(nextPlayer, verbose);
                        if (config.auto_discard) {
                            trimExcess(nextPlayer, verbose);
                        }
                    }
                    if (config.auto_mana) {
                        ++mana[nextPlayer].world;
                        ++mana[nextPlayer].astral;
                    }
                }
            }
            if (verbose) {
                board.setUpBoard(this);
                end_message();
            }
            checkConsistency();
            return;
        }
    }
    int c = m.card;
    assert(c >= 0);
    int card_number = find(hand[nextPlayer].begin(), hand[nextPlayer].end(), m.card)-hand[nextPlayer].begin();
    assert(hand[nextPlayer][card_number] == c);
    if (m.target == DISCARD) {
        assert(config.can_self_discard || config.must_self_discard);
        hand[nextPlayer].erase(hand[nextPlayer].begin()+card_number);
        if (config.discard_for_mana) {
#if 0
            if (config.can_discard_any_for_mana || toBool(card_class[c] & CardClass::MANA)) {
                mana[nextPlayer] += cost[c];
            }
#endif
        }
        checkConsistency();
        assert(location[c] == Location::HAND0 || location[c] == Location::HAND1);
        location[c] = Location::GRAVEYARD;
        checkConsistency();
        if (verbose) cout << "!!!!!!!!!!!!!GONETOGRAVEYARD!!!!!!!!!!!!!!!!!!!" << endl;
        graveyard.push_back(c);
        exposedTo[0][c] = true;
        exposedTo[1][c] = true;
        if (verbose) {
            board.setUpBoard(this);
            end_message();
        }
#if 0
        if (hasProperty(c, CardProperty::INSTANT)) {
            executeInstant(c, verbose);
        }
#endif
        if (verbose) {
            board << description(c, verbose) << " discarded";
            end_message();
        }
        // swap YYY
        nextPlayer = 1-nextPlayer;
        passed = false;
        checkConsistency();
        return;
    }
    if (hasProperty(c, CardProperty::INSTANT)) {
        exposedTo[0][c] = true;
        exposedTo[1][c] = true;
        mana[nextPlayer] -= cost[c];
        hand[nextPlayer].erase(hand[nextPlayer].begin()+card_number);
        location[c] = Location::EXECUTING;
        int t = m.target;//m.target >= 1000 ? m.target : in_play[m.target];
        target[c] = t;
        executeInstant(c, verbose);
        location[c] = Location::GRAVEYARD;
        graveyard.push_back(c);
        if (verbose) {
            board.setUpBoard(this);
            end_message();
        }
        nextPlayer = 1-nextPlayer;
        passed = false;
        checkConsistency();
        return;
    }

    // Move card into play.
    // YYY swap
    int otherPlayer = 1-nextPlayer;
    passed = false;
    mana[nextPlayer] -= cost[c];
    hand[nextPlayer].erase(hand[nextPlayer].begin()+card_number);
    in_play.push_back(c);
    exposedTo[0][c] = true;
    exposedTo[1][c] = true;
    int t = m.target;//m.target >= 1000 ? m.target : in_play[m.target];
    target[c] = t;
    location[c] = Location::IN_PLAY;

    if (verbose) {
        board.setUpBoard(this);
        end_message();
    }

    nextPlayer = otherPlayer;
    if (config.draw_on_turn_end) {
        // DRAW
        if (hp[0] > 0 && hp[1] > 0) {
            if (deck[nextPlayer].size() >= 1) {
                drawCard(nextPlayer, verbose);
                if (config.auto_discard) {
                    trimExcess(nextPlayer, verbose);
                }
            }
            if (config.auto_mana) {
                ++mana[nextPlayer].world;
                ++mana[nextPlayer].astral;
            }
        }
    }
    // YYY
    if (verbose) {
        board.setUpBoard(this);
        end_message();
    }
    checkConsistency();
}

vector<SpellCaster::Move> SpellCaster::legalMoves() const {
    assert(this);
    vector<Move> moves;
    if (!config.must_self_discard || hand[nextPlayer].size() <= config.max_cards_in_hand) {
        moves.push_back(Move(nextPlayer, PASS, PASS));
        for (int i = 0; i < hand[nextPlayer].size(); ++i) {
            for (int j = 0; j < in_play.size(); ++j) {
                if (isLegalId(Move(nextPlayer, hand[nextPlayer][i], in_play[j]))) {
                    moves.push_back(Move(nextPlayer, hand[nextPlayer][i], in_play[j]));
                }
            }
            if (toBool(target_class[hand[nextPlayer][i]] & CardClass::PLAYER)) {
                if (isLegalId(Move(nextPlayer, hand[nextPlayer][i], PLAYER0))) {
                    moves.push_back(Move(nextPlayer, hand[nextPlayer][i], PLAYER0));
                }
                if (isLegalId(Move(nextPlayer, hand[nextPlayer][i], PLAYER1))) {
                    moves.push_back(Move(nextPlayer, hand[nextPlayer][i], PLAYER1));
                }
            }
        }
        if (config.can_self_discard) {
            for (int i = 0; i < hand[nextPlayer].size(); ++i) {
                moves.push_back(Move(nextPlayer, hand[nextPlayer][i], DISCARD));
            }
        }
    } else {
        for (int i = 0; i < hand[nextPlayer].size(); ++i) {
            moves.push_back(Move(nextPlayer, hand[nextPlayer][i], DISCARD));
        }
    }
    return moves;
}

bool SpellCaster::isLegalId(Move m, bool verbose) const {
    assert(this);
    if (!config.must_self_discard || hand[nextPlayer].size() <= config.max_cards_in_hand) {
        //board << m ;
        if (m.card == -1) {
            return true;
        }
        if (m.card < 0) {
            if (verbose) {
                board << "Not a valid card" ;
                cout << "Not a valid card" << endl;
            }
            return false;
        }
        
        if (find(hand[nextPlayer].begin(), hand[nextPlayer].end(), m.card) == hand[nextPlayer].end()) {
            if (verbose) {
                board << "That card isn't in hand" ;
                cout << "Card not in hand" << endl;
            }
            return false;
        }
        // We've established it's a valid card.

        if (!config.can_self_discard) {
            if (m.target == DISCARD) {
                return true;
            }
        }
        if (m.target < 0) {
            if (verbose) {
                board << "Not a valid target" ;
                cout << "Invalid target" << endl;
            }
            return false;
        }
        if (!(find(in_play.begin(), in_play.end(), m.target) != in_play.end() || m.target == PLAYER0 || m.target == PLAYER1 || m.target == DISCARD)) {
            if (verbose) {
                board << "Target is neither a player nor in-play";
                cout << "Target not player or in play" << endl;
            }
            return false;
        }
        if (m.target != DISCARD && cost[m.card].world > mana[nextPlayer].world) {
            if (verbose) {
                board << "Not enough world mana" ;
                cout << "Not enough world mana" << endl;
            }
            return false;
        }
        if (m.target != DISCARD && cost[m.card].astral > mana[nextPlayer].astral) {
            if (verbose) {
                board << "Not enough astral mana" ;
                cout << "Not enough astral mana" << endl;
            }
            return false;
        }

        if (m.target != DISCARD) {
            const char *reason = cantCardTarget(m.card, m.target);
            if (reason) {
                if (verbose) {
                    board << reason ;
                    cout << reason << endl;
                }
                return false;
            } else {
                return true;
            }
        }
        return true;
    } else {
        if (m.card >= 0 && find(hand[nextPlayer].begin(), hand[nextPlayer].end(), m.card) != hand[nextPlayer].end() && m.target == DISCARD) {
            return true;
        }
        if (verbose) {
            cout << "Illegal discard" << endl;
        }
        return false;
    }
}

const char *SpellCaster::cantCardTarget(int c, int t) const {
    // Deal with player being targetted...
    CardClass allowed_classes = target_class[c];
    if (t == -1) {
        return "target card no longer in play";
    }
    if ((t==PLAYER0 || t==PLAYER1)) {
        if (toBool(allowed_classes & CardClass::PLAYER)) {
            return 0;
        } else {
            return "can't target player";
        }
    }

    // ...otherwise deal with card.
    // First classes...
    CardClass class_of_target = card_class[t];
    if (toBool(class_of_target & CardClass::MONSTER) && !toBool(allowed_classes & CardClass::MONSTER)) {
        return "can't target monster";
    }
    if (toBool(class_of_target & CardClass::SPELL) && !toBool(allowed_classes & CardClass::SPELL)) {
        return "can't target spell";
    }
    if (toBool(class_of_target & CardClass::ARTIFACT) && !toBool(allowed_classes & CardClass::ARTIFACT)) {
        return "can't target artifact";
    }

    CardProperty properties_of_target = properties[t];
    
    // ...requirements...
    CardProperty requirements_of_card = requirements[c];

    if (toBool(requirements_of_card & CardProperty::UNDEAD) && !toBool(properties_of_target & CardProperty::UNDEAD)) {
        return "target must be undead";
    }
    if (toBool(requirements_of_card & CardProperty::BLUE_MAGIC_RESISTANT) && !toBool(properties_of_target & CardProperty::BLUE_MAGIC_RESISTANT)) {
        return "target must be blue magic resistant";
    }
    if (toBool(requirements_of_card & CardProperty::RED_MAGIC_RESISTANT) && !toBool(properties_of_target & CardProperty::RED_MAGIC_RESISTANT)) {
        return "target must be red magic resistant";
    }
    if (toBool(requirements_of_card & CardProperty::FEARLESS) && !toBool(properties_of_target & CardProperty::FEARLESS)) {
        return "target must be fearless";
    }
    if (toBool(requirements_of_card & CardProperty::FIRE_RESISTANT) && !toBool(properties_of_target & CardProperty::FIRE_RESISTANT)) {
        return "target must be fire resistant";
    }
    if (toBool(requirements_of_card & CardProperty::FLYING) && !toBool(properties_of_target & CardProperty::FLYING)) {
        return "target must be fly";
    }
    if (toBool(requirements_of_card & CardProperty::BLESSED) && !toBool(properties_of_target & CardProperty::BLESSED)) {
        return "target must be blessed";
    }
    if (toBool(requirements_of_card & CardProperty::LINKED) && !toBool(properties_of_target & CardProperty::LINKED)) {
        return "target must be linked";
    }
    if (toBool(requirements_of_card & CardProperty::ETHEREAL) && !toBool(properties_of_target & CardProperty::ETHEREAL)) {
        return "target must be ethereal";
    }
    if (toBool(requirements_of_card & CardProperty::AQUATIC) && !toBool(properties_of_target & CardProperty::AQUATIC)) {
        return "target must be aquatic";
    }
    if (toBool(requirements_of_card & CardProperty::REGENERATING) && !toBool(properties_of_target & CardProperty::REGENERATING)) {
        return "target must be regenerating";
    }
    
    // ...and then properties.
    CardProperty exclusions_of_card = exclusions[c];

    if (toBool(exclusions_of_card & CardProperty::UNDEAD) && toBool(properties_of_target & CardProperty::UNDEAD)) {
        return "can't target undead";
    }
    if (toBool(exclusions_of_card & CardProperty::BLUE_MAGIC_RESISTANT) && toBool(properties_of_target & CardProperty::BLUE_MAGIC_RESISTANT)) {
        return "can't target blue magic resistant";
    }
    if (toBool(exclusions_of_card & CardProperty::RED_MAGIC_RESISTANT) && toBool(properties_of_target & CardProperty::RED_MAGIC_RESISTANT)) {
        return "can't target red magic resistant";
    }
    if (toBool(exclusions_of_card & CardProperty::FIRE_RESISTANT) && toBool(properties_of_target & CardProperty::FIRE_RESISTANT)) {
        return "can't target fire resistant";
    }
    if (toBool(exclusions_of_card & CardProperty::FEARLESS) && toBool(properties_of_target & CardProperty::FEARLESS)) {
        return "can't target fearless";
    }
    if (toBool(exclusions_of_card & CardProperty::FLYING) && toBool(properties_of_target & CardProperty::FLYING)) {
        return "can't target flying";
    }
    if (toBool(exclusions_of_card & CardProperty::BLESSED) && toBool(properties_of_target & CardProperty::BLESSED)) {
        return "can't target blessed";
    }
    if (toBool(exclusions_of_card & CardProperty::LINKED) && toBool(properties_of_target & CardProperty::LINKED)) {
        return "can't target linked";
    }
    if (toBool(exclusions_of_card & CardProperty::ETHEREAL) && toBool(properties_of_target & CardProperty::ETHEREAL)) {
        return "can't target ethereal";
    }
    if (toBool(exclusions_of_card & CardProperty::AQUATIC) && toBool(properties_of_target & CardProperty::AQUATIC)) {
        return "can't target aquatic";
    }
    if (toBool(exclusions_of_card & CardProperty::REGENERATING) && toBool(properties_of_target & CardProperty::REGENERATING)) {
        return "can't target regenerating";
    }

    return 0;
}

void SpellCaster::execute(bool verbose) {
    checkConsistency();
    while (in_play.size() > 0) {
        int c = in_play.back();
        assert(location[c] == Location::IN_PLAY);
        if (verbose) {
            board << "Executing ";
            board << description(c);
            end_message();
        }
        in_play.pop_back();
        location[c] = Location::EXECUTING;
        checkConsistency();
        executeCard(c, verbose);
        checkConsistency();
        assert(location[c] != Location::EXECUTING);
        // YYY
        if (verbose) {
            board.setUpBoard(this);
            end_message();
        }
    }
    checkConsistency();
    if (config.draw_cards_after_execution) {
        if (hp[0] > 0 && hp[1] > 0) {
            if (deck[0].size() >= 1) {
                drawCard(0, verbose);
                if (config.auto_discard) {
                    trimExcess(0, verbose);
                }
                if (config.auto_mana) {
                    ++mana[0].world;
                    ++mana[0].astral;
                }
            }
            checkConsistency();
            if (deck[1].size() >= 1) {
                drawCard(1, verbose);
                if (config.auto_discard) {
                    trimExcess(1, verbose);
                }
                if (config.auto_mana) {
                    ++mana[1].world;
                    ++mana[1].astral;
                }
            }
            checkConsistency();
        }
    }
    checkConsistency();
}

int SpellCaster::damage_card(int card, int damage, bool verbose) {
    int old_hp = cardhp[card];
    int new_hp = max(0, old_hp-damage);
    cardhp[card] = new_hp;
    int damage_done = old_hp-new_hp;
    if (verbose) {
        board.highlightCard(card);
        board << description(card, false) << " takes "
             << damage_done << " damage." ;
             cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DAMAGE CARD!!!!!!!!!" << endl;
        board.shake_card(card);
        end_message();
        board.noHighlightCard(card);
    }
    if (cardhp[card] <= 0) {
        kill_card(card);
    }
    if (hasProperty(card, CardProperty::LINKED)) {
        int old_hp = hp[owner[card]];
        int new_hp = max(0, old_hp-damage_done);
        hp[owner[card]] = new_hp;
        damage_done = old_hp-new_hp;
        if (verbose) {
            board << "Link means player " << owner[card] << " takes "
                 << damage_done << " damage." ;
            end_message();
        }

    }
    return damage_done;
}

int SpellCaster::damage_player(int player, int damage, bool verbose) {
    int old_hp = hp[player];
    int new_hp = max(0, old_hp-damage);
    hp[player] = new_hp;
    int damage_done = old_hp-new_hp;
    if (verbose) {
        board << "PLAYER " << player << " takes " << damage_done << " damage" ;
        board.shakePlayer(player);
        end_message();
    }
    return damage_done;
}

void SpellCaster::kill_card(int c, bool verbose) {
    location[c] = Location::GRAVEYARD;
    in_play.erase(remove(in_play.begin(), in_play.end(), c),
                  in_play.end());
    graveyard.push_back(c);
    exposedTo[0][c] = true;
    exposedTo[1][c] = true;
    if (verbose) {
        board << description(c, false)
             << " is killed." ;
        end_message();
    }
    // YYY
    if (verbose) {
        board.setUpBoard(this);
        end_message();
    }
}

void SpellCaster::destroy_card_from(Location loc, int c, bool verbose) {
//    cout << "dcf " << int(loc) << ' ' << int(location[c]) << endl;
    assert(location[c] == loc);
    if (verbose) {
        board << description(c, verbose) << " to GRAVEYARD" ;
    }
    in_play.erase(std::remove(in_play.begin(), in_play.end(), c), in_play.end());
    location[c] = Location::GRAVEYARD;
    graveyard.push_back(c);
    exposedTo[0][c] = true;
    exposedTo[1][c] = true;
    // YYY
    if (verbose) {
        board.setUpBoard(this);
        end_message();
    }
}

bool SpellCaster::targetNotInPlay(int c, bool verbose) {
    if (target[c] >= PLAYER0) {
        return false;
    }
    if (location[target[c]] == Location::IN_PLAY) {
        return false;
    }
    if (verbose) {
        board << description(c);
        board << " target no longer in play" ;
        end_message();
    }
    card_end_from(Location::EXECUTING, c, verbose);
    return true;
}

string SpellCaster::description(int c, bool verbose) const {
    ostringstream dout;
    if (c == 1000) {
        dout << green << "PLAYER 0" << normal;
        return dout.str();
    }
    if (c == 1001) {
        dout << red << "PLAYER 1" << normal;
        return dout.str();
    }
    dout << cardColour(c);

    definitions[c]->describe(this, dout, c);

    if (verbose && target[c] >= 0) {
        dout << " targetting " << description(target[c], false) << cardColour(c);
    }
    dout << normal;

    return dout.str();
}

void SpellCaster::showInPlay() const {
    int n = in_play.size();
    vector<vector<int> > bar(n, vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        int c = in_play[i];
        if (target[c] == PLAYER0) {
            for (int j = 0; j < i; ++j) {
                bar[j][i] = 2;
            }
            bar[i][i] = 3;
        } else if (target[c] == PLAYER1) {
            bar[i][i] = 1;
            for (int j = i+1; j < n; ++j) {
                bar[j][i] = 2;
            }
        } else if (target[c] >= 0) {
            int s = location_in_hand(target[c]);
            bar[s][i] = 1;
            for (int j = s+1; j < i; ++j) {
                bar[j][i] = 2;
            }
            bar[i][i] = 3;
        }
    }
    cout << yellow << "IN PLAY" << normal << endl;
    for (int i = 0; i < in_play.size(); ++i) {
        cout << "        ";
        for (int j = 0; j < in_play.size(); ++j) {
            switch (bar[i][j]) {
            case 0:
                cout << "  ";
                break;
            case 1:
                cout << "┌ "; 
                break;
            case 2:
                cout << "│ "; 
                break;
            case 3:
                cout << "└ "; 
                break;
            }
        }
        cout << definitions[in_play[i]]->icon << ' ' << i << ". ";
        cout << description(in_play[i], false);
        cout << endl;
    }
}

void SpellCaster::show() const {
    cout << "--------------" << endl;
    cout << bold << blue << "Player " << nextPlayer << " to go" << normal << endl;
    cout << "Player 0 HP = " << hp[0] << " mana: " << red
         << mana[0].world << normal << ","
         << blue << mana[0].astral << normal << endl;
    cout << yellow << "PLAYER 0 HAND" << normal << endl;
    for (int i = 0; i < hand[0].size(); ++i) {
        cout << definitions[hand[0][i]]->icon << ' ' << i << ". ";
        if (!(cost[hand[0][i]] <= mana[0])) {
            cout << '(';
        }
        cout << description(hand[0][i]);
        if (!(cost[hand[0][i]] <= mana[0])) {
            cout << ')';
        }
        cout << endl;
    }

    showInPlay();

    cout << yellow << "PLAYER 1 HAND" << normal << endl;
    cout << "Player 1 HP = " << hp[1] << " mana: "
         << red << mana[1].world << normal << ","
         << blue << mana[1].astral << normal << endl;
    for (int i = 0; i < hand[1].size(); ++i) {
        if (true || exposedTo[0][hand[1][i]]) {
            cout << definitions[hand[1][i]]->icon << ' ' << i << ". ";
            if (!(mana[1] >= cost[hand[1][i]])) {
                cout << '(';
            }
            cout << description(hand[1][i]);
            if (!(mana[1] >= cost[hand[1][i]])) {
                cout << ')';
            }
            cout << endl;
        } else {
            cout << "   " << i << ". " << red
                 << "[" << hand[1][i]<< "]************" << normal << endl;
        }
    }
    cout << "--------------" << endl;
}
