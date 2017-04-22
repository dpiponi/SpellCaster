#ifndef SPELLCASTER_H
#define SPELLCASTER_H

#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <deque>

#include "CardClass.h"
#include "CardProperty.h"
#include "CardBase.h"
#include "Cards.h"
#include "UCTRave.h"
#include "Location.h"

#define PAUSE 1000000

using std::pair;
using std::make_pair;
using std::swap;
using std::remove;
using std::endl;
using std::min;
using std::max;
using std::map;
using std::cout;
using std::make_shared;
using std::string;
using std::ostringstream;
using std::deque;

extern const char *normal;
extern const char *bold;
extern const char *red;
extern const char *green;
extern const char *white;
extern const char *yellow;
extern const char *blue;
extern const char *magenta;
extern const char *black_bg;
extern const char *white_bg;

template<class X>
void shuffle(vector<X> &v) {
    int n = v.size();
    for (int i = n-1; i >= 1; --i) {
        int j = rand() % (i+1);
        swap(v[i], v[j]);
    }
}

#include "CardBase.h"

//class Definition;

class Config {
public:
    int max_cards_in_hand;
    int initial_hp;
};

extern Config default_config;

const int PLAYER0 = 1000;
const int PLAYER1 = 1001;
const int DISCARD = 1002;
const int PASS = -1;

#include "Mana.h"
#include "Board.h"

#ifdef BOARD
extern Board<SpellCaster> board;
#endif

class SpellCaster {
public:
    vector<vector<int> > deck;
    vector<vector<int> > hand;
    int hp[2];
    Mana mana[2];
    bool passed;
    int nextPlayer;
    int number_of_cards;
    vector<int> graveyard;
    vector<int> in_play;
    // Target is card index, or 1000 for player 0, 1001 for player 1.
    vector<int> target;
    vector<int> owner; // Is 0 or 1
    vector<Location> location;
    vector<Mana> cost;
    vector<int> basehp;
    vector<int> cardhp;
    vector<int> attack;
    vector<vector<bool>> exposedTo;
    vector<CardClass> card_class;
    vector<CardClass> target_class;
    vector<CardProperty> properties;
    vector<CardProperty> requirements;
    vector<CardProperty> exclusions;
    struct Move {
        int player;
    public:
        int card;
        int target;
        int getPlayer() const {
            return player;
        }
        explicit Move(int p, int c, int t) : player(p), card(c), target(t) { }
    };
    typedef vector<const Definition *> Specification;
    Specification definitions;
    const Config &config;

    void checkConsistency() {
#ifndef NDEBUG
        vector<bool> card_flags(number_of_cards, false);
        for (auto p : in_play) {
            assert(location[p] == Location::IN_PLAY);
            assert(!card_flags[p]);
            card_flags[p] = true;
        }
        for (auto p : hand[0]) {
            assert(location[p] == Location::HAND0);
            assert(!card_flags[p]);
            card_flags[p] = true;
        }
        for (auto p : hand[1]) {
            assert(location[p] == Location::HAND1);
            assert(!card_flags[p]);
            card_flags[p] = true;
        }
        for (auto p : deck[0]) {
            assert(location[p] == Location::DECK);
            assert(!card_flags[p]);
            card_flags[p] = true;
        }
        for (auto p : deck[1]) {
            assert(location[p] == Location::DECK);
            assert(!card_flags[p]);
            card_flags[p] = true;
        }
#endif
    }

    friend bool operator<(const Move &a, const Move &b) {
        if (a.card < b.card) {
            return true;
        }
        if (a.card > b.card) {
            return false;
        }
        return a.target < b.target;
    }

    friend bool operator==(const Move &a, const Move &b) {
        if (a.card != b.card) {
            return false;
        }
        if (a.card == PASS) {
            return true;
        }
        return a.target == b.target;
    }

    friend std::ostream &operator<<(ostream &out, SpellCaster::Move &m) {
        return out << magenta << m.card << ' ' << m.target << normal;
    }

    //
    // The move that got us here.
    //

    ~SpellCaster() { }

    void initWith(const Definition *def) {
        cost.push_back(Mana {def->world_cost, def->astral_cost});
        basehp.push_back(def->basehp);
        cardhp.push_back(def->basehp);
        attack.push_back(def->attack);
        card_class.push_back(def->card_class);
        target_class.push_back(def->target_class);
        properties.push_back(def->properties);
        requirements.push_back(def->requirements);
        exclusions.push_back(def->exclusions);
    }

    SpellCaster(const Config &config0,
                const Specification &definitions_player,
                const Specification &definitions_computer) :
//                    definitions(_definitions),
                config(config0),
                hand(2),
                deck(2),
                exposedTo(2),
                nextPlayer(0) { 
        int i = 0;
        for (auto p : definitions_player) {
            definitions.push_back(p);
            deck[0].push_back(i);
            target.push_back(-1);
            ++i;
        }
        for (auto p : definitions_computer) {
            definitions.push_back(p);
            deck[1].push_back(i);
            target.push_back(-1);
            ++i;
        }
        number_of_cards = i;
        target.resize(number_of_cards);
        owner.resize(number_of_cards);
        location.resize(number_of_cards);
        shuffle(deck[0]);
        shuffle(deck[1]);
        exposedTo[0].resize(number_of_cards, false);
        exposedTo[1].resize(number_of_cards, false);
        for (int i = 0; i < number_of_cards; ++i) {
            initWith(definitions[i]);
        }
        for (int i = 0; i < number_of_cards; ++i) {
            location[i] = Location::DECK;
        }

        hp[0] = config.initial_hp;
        hp[1] = config.initial_hp;
        mana[0] = Mana {0, 0};
        mana[1] = Mana {0, 0};
        passed = false;

        // Initial constructor needs verbosity???
        _drawCard(0);
        _drawCard(1);
        checkConsistency();
    }

    void end_message() const {
#ifdef BOARD
        board.setNewMessage();
        usleep(PAUSE);
#endif
    }

    bool hasProperty(int card, CardProperty property) {
        return toBool(properties[card] & property);
    }

    int __drawCard(int player, bool verbose = false) {
        int c = deck[player].back();
        assert(location[c] == Location::DECK);
        deck[player].pop_back();
        hand[player].push_back(c);
        location[c] = player ? Location::HAND1 : Location::HAND0;
        owner[c] = player;
        exposedTo[player][c] = true;
        checkConsistency();
        return c;
    }

    void _drawCard(int player, bool verbose = false) {
        while (deck[player].size() > 0 && hand[player].size() < config.max_cards_in_hand) {
            __drawCard(player, verbose);
        }
    }

    void drawCard(int player, bool verbose = false) {
        _drawCard(player, verbose);
    }

    void trimExcess(int player, bool verbose = false) {
        while (hand[player].size() > config.max_cards_in_hand) {
            int c = hand[player].front();
#ifdef BOARD
            if (verbose) {
                board << "Player " << player << " loses ";
                board << description(c, false) ;
            }
#endif
            //hand[player].pop_front();
            hand[player].erase(hand[player].begin(), hand[player].begin()+1);
            location[c] = Location::GRAVEYARD;
            graveyard.push_back(c);
            exposedTo[0][c] = true;
            exposedTo[1][c] = true;
        }
    }

    int heuristicCardValue(int c) const {
        if (c == -1) { // XXX Not in play. Could do better.
            return 0;
        }
        assert(c == PLAYER0 || c == PLAYER1 || c >= 0 & c < definitions.size());
        if (c == PLAYER0) {
            return nextPlayer==0 ? 1 : -1;
        }
        if (c == PLAYER1) {
            return nextPlayer==1 ? 1 : -1;
        }
        // Happens when cards are rearranged (e.g. Recall)
        if (location[c] != Location::IN_PLAY) {
            return 0; // XXX Could do better
        }
        return definitions[c]->positivity*heuristicCardValue(target[c]);
    }

    pair<int, int> heuristicValue(Move m) const {
        if (m.card == -1) {
            // PASS
            return make_pair(0, 10);
        }
//        if (m.target == DISCARD && toBool(definitions[m.card]->card_class & CardClass::MANA)) {
//            return make_pair(100, 100);
//        }
        if (m.target == DISCARD) {
            return make_pair(0, 5);
        }
        int card_positivity = definitions[m.card]->positivity;
        int target_positivity = heuristicCardValue(m.target);
        int overall_positivity = 10*card_positivity*target_positivity;
        return make_pair(overall_positivity, 10);
    }

    bool isLegalId(Move m, bool verbose = false) const;

    vector<Move> legalMoves() const;

    // Value for player who just moved.
    double evaluate() const {
        int currentPlayer = 1-nextPlayer;
        if (hp[currentPlayer] <= 0 && hp[nextPlayer] <= 0) {
            return 0.0;
        }
        if (hp[currentPlayer] <= 0) {
            return -1.0;
        }
        if (hp[nextPlayer] <= 0) {
            return 1.0;
        }
        if (deck[currentPlayer].size() < 1 && deck[nextPlayer].size() < 1) {
            return 0.0;
        }
        if (deck[nextPlayer].size() < 1) {
            return 1.0;
        }
        if (deck[currentPlayer].size() < 1) {
            return -1.0;
        }
        return NON_TERMINAL;
    }

    void doExecution(bool verbose);
    void doMove(Move m, bool verbose = false);

    void determinise();

    // Returns damage actually done.
    int damage_player(int player, int damage, bool verbose = false);
    int damage_card(int card, int damage, bool verbose = false);
    void kill_card(int c, bool verbose = false);
    void return_card_from(Location loc, int c, bool verbose);
    void destroy_card_from(Location loc, int c, bool verbose);
    void card_end_from(Location loc, int c, bool verbose);

    // The argument is zero or 1
    const char *playerColour(int player) const {
        switch (player) {
        case 0: return green;
        case 1: return red;
        default: assert(false);
        }
        __builtin_unreachable();
    }

    const char *cardColour(int c) const {
        if (c >= 1000) {
            return playerColour(c-1000);
        } else {
            return playerColour(owner[c]);
        }
    }

    bool targetNotInPlay(int c, bool verbose);
    bool cardImmobile(int c, bool verbose);
    bool cardCombining(int c, bool verbose);
    bool targetNotLegal(int c, bool verbose);

    //
    // No optional<> yet :-(
    // Returns NULL pointer if can target
    // Otherwise returns explanation for why not.
    //
    const char *cantCardTarget(int c, int t) const;

    void executeCard(int c, bool verbose) {
        if (targetNotLegal(c, verbose)) return;
        if (targetNotInPlay(c, verbose)) return;
        if (cardImmobile(c, verbose)) return;
        if (cardCombining(c, verbose)) return;
        definitions[c]->execute(this, c, verbose);
#ifdef BOARD
        if (verbose) {
            board.setUpBoard(this, now(), now()+0.5);
        }
#endif
    }

    void executeInstant(int c, bool verbose);

    void handleInstant(int c, int card_number, int t, bool verbose);

    void execute(bool verbose);

    int location_in_hand(int c) const {
        return find(in_play.begin(), in_play.end(), c)-in_play.begin();
    }

    string description(int c, bool verbose = true) const;

    void describeId(Move m) const {
        if (m.card == PASS) {
            cout << "PASS";
        } else if (m.target == DISCARD) {
            cout << "DISCARD " << description(m.card);
        } else {
            cout << description(m.card, false) << " targetting " << description(m.target, false);
        }
    }

    void showInPlay() const;
    void show() const;
};

extern vector<const Definition *> all_cards;

void dump_cards();

map<string, const Definition *> make_database(const vector<const Definition *> &definitions);

vector<const Definition *> read_deck(const map<string, const Definition *> &database, const char *filename);

#endif
