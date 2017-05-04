#include "CardClass.h"
#include "CardBase.h"
#include "Cards.h"
#include "SpellCaster.h"

const class BiteTheHand : public SpellDefinition {
public:
    BiteTheHand() : SpellDefinition("Bite The Hand...", "🃏 ", -1, 2, 3, 0, 0, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }

    void execute(SpellCaster *game, int c, bool verbose) const {
        int target_c = game->target[c];
        game->target[target_c] = PLAYER0+game->owner[target_c];
#ifdef BOARD
        if (verbose) {
            *board << game->description(c, false) << " makes " << game->description(game->target[c], false) << " turn on OWNER";
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
    }
} biteTheHand;

const class Bless : public SpellDefinition {
public:
    Bless() : SpellDefinition("Bless", "🙏🏾 " ,1, 0, 2,  1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE, /* requirements */
                    CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::UNDEAD) { }
    void execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
        if (verbose) {
            *board << game->description(c, false) << " blessing "
                 << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->properties[game->target[c]] |= CardProperty::BLESSED;
        game->card_end_from(Location::EXECUTING, c, verbose);
    }
} bless;

const class Burn : public BlastBase {
public:
    Burn() : BlastBase("Burn", "🔥 ", -1, 4, 0, 0, 0, 3,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT | CardProperty::FIRE_RESISTANT) { }
    void animate(shared_ptr<const SpellCaster> game, shared_ptr<BoardBase> board, int card, int target, bool verbose) const override {
#ifdef BOARD
        if (verbose) {
            cout << "BBBBBBBBBUUUUUUUUURRRRRRRRRRRRNNNNNNNNNNNN!!!!!!!!!" << endl;
            board->flame(Vector3f(1.0, 0.0, 0.0), card, target, now(), now()+3.0);
        }
#endif
    }
} fire;

const class Haste : public SpellDefinition {
public:
    Haste() : SpellDefinition("Haste", "⬇️️ ", 0, 0, 2, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER | CardClass::SPELL | CardClass::ARTIFACT,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::BLUE_MAGIC_RESISTANT) { }

    void execute(shared_ptr<SpellCaster> game, int c, bool verbose) const {
        auto c_location = find(game->in_play.begin(),
                               game->in_play.end(),
                               game->target[c]);
        assert(c_location != game->in_play.end());
        game->in_play.erase(c_location);
        game->in_play.push_back(game->target[c]);
#ifdef BOARD
        if (verbose) {
            *board << game->description(game->target[c], false);
            *board << " POPPED to top of stack";
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
    }
} haste;

const class Sloth : public SpellDefinition {
public:
    Sloth() : SpellDefinition("Sloth", "⬆️️ ", 0, 1, 1, 1, 1, 1,
                    CardClass::SPELL,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::RED_MAGIC_RESISTANT | CardProperty::BLUE_MAGIC_RESISTANT) { }

    void execute(shared_ptr<SpellCaster> game, int c, bool verbose) const {
        auto c_location = find(game->in_play.begin(),
                               game->in_play.end(),
                               game->target[c]);
        assert(c_location != game->in_play.end());
        game->in_play.erase(c_location);
        game->in_play.insert(game->in_play.begin(), game->target[c]);
#ifdef BOARD
        if (verbose) {
            *board << game->description(game->target[c], false);
            *board << " PUSHED to bottom of stack";
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
    }
} sloth;
