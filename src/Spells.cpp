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
            board << game->description(c, false) << " makes " << game->description(game->target[c], false) << " turn on OWNER";
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
            board << game->description(c, false) << " blessing "
                 << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->properties[game->target[c]] |= CardProperty::BLESSED;
        game->card_end_from(Location::EXECUTING, c, verbose);
    }
} bless;

