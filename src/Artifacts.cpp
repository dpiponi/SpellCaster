#include "CardClass.h"
#include "CardBase.h"
#include "Cards.h"
#include "SpellCaster.h"


const class BrickWall : public ArtifactDefinition {
public:
    BrickWall() : ArtifactDefinition("Brick Wall", "🔴 ", -1, 2, 2, 1, 1, 1,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }

    void execute(shared_ptr<BoardBase> board, shared_ptr<SpellCaster> game, int card, bool verbose) const {
        int target = game->target[card];
        int attack = computeAttack(game, board, card, target, verbose);
#ifdef BOARD
        if (verbose) {
            *board << game->description(card, false) << " blocks "
                 << game->description(target, false);
            game->end_message();
        }
#endif
        game->properties[target] |= CardProperty::IMMOBILE;
        game->damage_card(target, attack, verbose);
        game->card_end_from(Location::EXECUTING, card, verbose);
    }
} brickWall;

const class WarMachine : public ArtifactDefinition {
public:
    WarMachine() : ArtifactDefinition("War Machine", "🗼 ", -1, 0, 3, 1, 1, 3,
                    CardClass::ARTIFACT,
                    CardClass::MONSTER,
                    CardProperty::NONE,
                    CardProperty::NONE,
                    CardProperty::FLYING | CardProperty::ETHEREAL) { }

    void execute(shared_ptr<BoardBase> board, shared_ptr<SpellCaster> game, int card, bool verbose) const {
        int target = game->target[card];
        int attack = computeAttack(game, board, card, target, verbose);
        if (game->basehp[target] < attack) {
#ifdef BOARD
            if (verbose) {
                *board << game->description(card, false) << " can't attack "
                     << game->description(target, false) << ", it's too small";
                game->end_message();
            }
#endif
        } else {
#ifdef BOARD
            if (verbose) {
                *board << game->description(card, false) << " attacks "
                     << game->description(target, false);
                game->end_message();
            }
#endif
            game->damage_card(target, attack, verbose);
        }
        game->card_end_from(Location::EXECUTING, card, verbose);
    }
} warMachine;
