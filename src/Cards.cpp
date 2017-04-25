#include "CardClass.h"
#include "CardBase.h"
#include "Cards.h"
#include "SpellCaster.h"

inline void describe_cost(ostream &out, const Mana &mana) {
    if (mana.world > 0) {
        out << red << mana.world << normal;
    }
    if (mana.world > 0 && mana.astral > 0) {
        out << ',';
    }
    if (mana.astral > 0) {
        out << blue << mana.astral << normal;
    }
}

// XXX For now
// Assumes it is targetting card.
int Definition::computeAttack(SpellCaster *game, int card, int target, bool verbose) const {
    return game->attack[card];
}

void
MonsterDefinition::describe(const SpellCaster *game,
                            ostream &out, int c) const {
        out << name << normal << "[" << c << ":cost=";
        describe_cost(out, game->cost[c]);
        out << ",hp=" << game->cardhp[c] << ",att=" << game->attack[c] << "]";
}

void
SpellDefinition::describe(const SpellCaster *game,
                          ostream &out, int c) const {
        out << name << normal << "[" << c << ":cost=";
        describe_cost(out, game->cost[c]);
        out << ",att=" << game->attack[c] << "]";
}

void ArtifactDefinition::describe(const SpellCaster *game, ostream &out, int c) const {
        out << name << normal << "[" << c << ":cost=";
        describe_cost(out, game->cost[c]);
        out << ",att=" << game->attack[c] << "]";
}

void MagicWeapon::execute(SpellCaster *game, int card, bool verbose) const {
    int target = game->target[card];
#ifdef BOARD
    if (verbose) {
        board << game->description(card, false) << " ATTACKS "
             << game->description(game->target[card], false);
        game->end_message();
    }
#endif
    int attack = computeAttack(game, card, target, verbose);
    game->damage_card(target, attack, verbose);
    game->card_end_from(Location::EXECUTING, card, verbose);
}

void Pop::execute(SpellCaster *game, int c, bool verbose) const {
    auto c_location = find(game->in_play.begin(),
                           game->in_play.end(),
                           game->target[c]);
    assert(c_location != game->in_play.end());
    game->in_play.erase(c_location);
    game->in_play.push_back(game->target[c]);
#ifdef BOARD
    if (verbose) {
        board << game->description(game->target[c], false);
        board << " POPPED to top of stack";
        game->end_message();
    }
#endif
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Push::execute(SpellCaster *game, int c, bool verbose) const {
    auto c_location = find(game->in_play.begin(),
                           game->in_play.end(),
                           game->target[c]);
    assert(c_location != game->in_play.end());
    game->in_play.erase(c_location);
    game->in_play.insert(game->in_play.begin(), game->target[c]);
#ifdef BOARD
    if (verbose) {
        board << game->description(game->target[c], false);
        board << " PUSHED to bottom of stack";
        game->end_message();
    }
#endif
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void JestersWish::executeInstant(SpellCaster *game, int c, bool verbose) const {
    int player = game->owner[c];
    if (game->target[c] != PLAYER0+player) {
#ifdef BOARD
        if (verbose) {
            board << "Can't discard opponent's cards";
            game->end_message();
        }
#endif
    } else if (game->hand[player].size() == 0) {
#ifdef BOARD
        if (verbose) {
            board << "No cards to discard";
            game->end_message();
        }
#endif
    } else {
#ifdef BOARD
        if (verbose) {
            board << "Discarding all cards in hand";
            game->end_message();
        }
#endif
        while (game->hand[player].size() > 0) {
            int d = game->hand[player].back();
            game->hand[player].pop_back();
            game->location[d] = Location::GRAVEYARD;
            game->graveyard.push_back(d);
            game->exposedTo[0][d] = true;
            game->exposedTo[1][d] = true;
        }
    }
};

void ManaCard::executeInstant(SpellCaster *game, int c, bool verbose) const {
    game->mana[game->target[c]-PLAYER0] += Mana {3, 2}; // XXX Should be target no?
};

void Henge::executeInstant(SpellCaster *game, int c, bool verbose) const {
    game->mana[game->target[c]-PLAYER0] += Mana {4, 1};
#ifdef BOARD
    if (verbose) {
        board << "Some mana for you";
        game->end_message();
    }
#endif
};

void Temple::executeInstant(SpellCaster *game, int c, bool verbose) const {
    int count = 0;
    for (auto p = game->in_play.begin(); p != game->in_play.end(); ++p) {
        count += game->hasProperty(*p, CardProperty::BLESSED);
    }
    game->mana[game->target[c]-PLAYER0] += Mana {1, 4+count};
#ifdef BOARD
    if (verbose) {
        board << "Some mana for you";
        game->end_message();
        board << "Plus extra " << count << " astral";
        game->end_message();
    }
#endif
};

void DrainPower::execute(SpellCaster *game, int c, bool verbose) const {
    int target_c = game->target[c];
    int damage = game->damage_card(target_c, game->attack[c], verbose);
    game->mana[game->owner[c]].world += damage;
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " drains "
             << damage << " power from "
             << game->description(game->target[c], false)
             << " into world mana";
        game->end_message();
    }
#endif
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Sacrifice::execute(SpellCaster *game, int c, bool verbose) const {
    int target_c = game->target[c];
    if (game->owner[c] == game->owner[target_c]) {
        int damage = game->attack[c];
        game->mana[game->owner[c]] += Mana{damage, damage};
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " gains " << damage << " power from sacrificing " << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->kill_card(target_c);
        game->card_end_from(Location::EXECUTING, c, verbose);
    } else {
#ifdef BOARD
        if (verbose) {
            board << "Can only sacrifice own card";
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
    }
}

void BlastBase::execute(SpellCaster *game, int card, bool verbose) const {
    int target = game->target[card];
    int attack = computeAttack(game, card, target, verbose);
    game->damage_card(target, attack, verbose);
    game->card_end_from(Location::EXECUTING, card, verbose);
}

// Write damage_player()? XXX
void MonsterDefinition::execute(SpellCaster *game, int card, bool verbose) const {
    int target = game->target[card];
    int attack = computeAttack(game, card, target, verbose);
    if (target == PLAYER0) {
#ifdef BOARD
        if (verbose) {
            board << game->description(card, false) << " ATTACKS PLAYER 0";
            game->end_message();
        }
#endif
        game->damage_player(0, attack, verbose);
        game->card_end_from(Location::EXECUTING, card, verbose);
#ifdef BOARD
        if (verbose) {
            board.publicSetPlayerPosition(now()+0.5, 0.95);
        }
#endif
        return;
    } else if (target == PLAYER1) {
#ifdef BOARD
        if (verbose) {
            board << game->description(card, false) << " ATTACKS PLAYER 1";
            game->end_message();
        }
#endif
        game->damage_player(1, attack, verbose);
        game->card_end_from(Location::EXECUTING, card, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(card, false) << " ATTACKS " << game->description(game->target[card], false);
        game->end_message();
    }
#endif
    game->damage_card(target, attack, verbose);
    game->card_end_from(Location::EXECUTING, card, verbose);
}

void GhoulBase::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS " << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    // Combine with damage_card? XXX
    game->cardhp[game->target[c]] -= game->attack[c];
    if (game->cardhp[game->target[c]] <= 0) {
        game->kill_card(game->target[c]);
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " KILLS " << game->description(game->target[c], false) << " and gains 1 HP";
            game->end_message();
        }
#endif
        ++game->basehp[c];
        ++game->cardhp[c];
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void SpellEater::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS " << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int target = game->target[c];
    if (toBool(game->card_class[c] & CardClass::MONSTER)) {
        game->damage_card(target, game->attack[c], verbose);
    } else {
        // Spell
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " destroyed " << game->description(target, false);
            game->end_message();
        }
#endif
        game->destroy_card_from(Location::IN_PLAY, target, verbose);
        ++game->attack[c];
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void SummonMonster::describe(const SpellCaster *game, ostream &out, int c) const {
    if (toBool(game->card_class[c] & CardClass::MONSTER)) {
        out << name << "[" << c << " (already summoned)" << normal << ",cost=";
        describe_cost(out, game->cost[c]);
        out << ",hp=" << game->cardhp[c] << ",att=" << game->attack[c] << "]";
    } else {
        out << name << "[" << c << " (summoning)" << normal << ",cost=";
        describe_cost(out, game->cost[c]);
        out << ",att=" << game->attack[c] << ",hp=" << game->cardhp[c] << "]";
    }
}

void SummonMonster::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->card_class[c] == CardClass::SPELL || game->card_class[c] == CardClass::ARTIFACT) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " summonned";
        }
#endif
        game->card_class[c] = CardClass::MONSTER;
    }
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS " << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->damage_card(game->target[c], game->attack[c]);
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void HolySymbol::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->card_class[game->target[c]] == CardClass::MONSTER && toBool(game->properties[game->target[c]] & CardProperty::UNDEAD)) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c) << " RESISTS ";
            board << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->card_end_from(Location::IN_PLAY, game->target[c], verbose);
    } else {
#ifdef BOARD
        if (verbose) {
            board << game->description(c) << " TARGETS non-suitable CARD";
            game->end_message();
        }
#endif
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Vampire::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS and sucks life from ";
        board << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int availableHp = game->damage_card(game->target[c], game->attack[c]);
#ifdef BOARD
    if (verbose) {
        board << "PLAYER " << game->owner[c] << " receives " << availableHp
             << " mana.";
    }
#endif
    game->mana[game->owner[c]] += Mana{availableHp, availableHp};
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Mimic::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS ";
        board << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    // XXX Use damage_card maybe?
    int availableHp = min(game->cardhp[game->target[c]], game->attack[c]);
    game->cardhp[game->target[c]] -= availableHp;
    if (game->cardhp[game->target[c]] <= 0) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " KILLS and takes on characteristics of " << game->description(game->target[c], false);
            game->basehp[c] = game->basehp[game->target[c]];
            game->attack[c] = game->attack[game->target[c]];
            game->end_message();
        }
#endif
        game->kill_card(game->target[c]);
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void MagicSpear::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->card_class[game->target[c]] == CardClass::SPELL) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " destroys " << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->card_end_from(Location::IN_PLAY, game->target[c], verbose);
        game->card_end_from(Location::EXECUTING, c, verbose);
    } else {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS " << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->damage_card(game->target[c], game->attack[c]);
        game->card_end_from(Location::EXECUTING, c, verbose);
    }
}

void PoisonMonster::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } 
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS " << game->description(game->target[c], false);
        board << " with POISON";
        game->end_message();
    }
#endif
    // combine with damage_card? XXX
    game->cardhp[game->target[c]] -= game->attack[c];
    game->basehp[game->target[c]] -= game->attack[c];
    if (game->cardhp[game->target[c]] <= 0 || game->basehp[game->target[c]] <= 0) {
        game->kill_card(game->target[c]);
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false);
            board << " KILLS ";
            board << game->description(game->target[c], false);
            game->end_message();
        }
#endif
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void StrengthBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false);
        board << " INCREASES attack OF ";
        board << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int bonus = game->attack[c];
    game->attack[game->target[c]] += bonus;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Weakling::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c) << " LOWERS STRENGTH OF ";
        board << game->description(game->target[c]) << " to 1";
        game->end_message();
    }
#endif
    game->attack[game->target[c]] = 1;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void TakeBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false);
        board << " TAKES ";
        board << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->owner[game->target[c]] = game->owner[c];
    game->return_card_from(Location::IN_PLAY, game->target[c], verbose);
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void ShieldBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " INCREASES HP OF "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int bonus = game->attack[c];
    game->basehp[game->target[c]] += bonus;
    game->cardhp[game->target[c]] += bonus;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void DestroyBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " destroyed " << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->destroy_card_from(Location::IN_PLAY, game->target[c], verbose);
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void DoubleBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " doubled in power " << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->attack[game->target[c]] *= game->attack[c];
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void ReturnBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c) << " RETURNS "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->return_card_from(Location::IN_PLAY, game->target[c], verbose);
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Sickness::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " DECREASES HEALTH OF "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    //XXX min()
    int bonus = game->attack[c];
    game->basehp[game->target[c]] -= bonus;
    game->cardhp[game->target[c]] -= bonus;
    // XXX damage_card?
    if (game->cardhp[game->target[c]] <= 0) {
        game->kill_card(game->target[c]);
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " KILLS "
                 << game->description(game->target[c], false);
            game->end_message();
        }
#endif
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void MakeArtifact::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " out of ";
        board << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->card_class[game->target[c]] &= ~CardClass::SPELL;
    game->card_class[game->target[c]] |= CardClass::ARTIFACT;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

// Exactly like TakeBase except you need to own target.
void Recall::executeInstant(SpellCaster *game, int c, bool verbose) const {
    int target = game->target[c];
    if (game->owner[c] == game->owner[target]) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c) << " recalls ";
            board << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->return_card_from(Location::IN_PLAY, target, verbose);
        // Clean up dangling targets
        for (auto p = game->in_play.begin(); p != game->in_play.end(); ++p) {
            if (game->target[*p] == target) {
                game->target[*p] = -1;
            }
        }
    } else {
#ifdef BOARD
        if (verbose) {
            board << "Can only recall own card";
            game->end_message();
        }
#endif
    }
    //game->card_end_from(Location::EXECUTING, c, verbose);
}

void Mephistopheles::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS " << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    // damage_card?
    game->cardhp[game->target[c]] -= game->attack[c];
    if (game->cardhp[game->target[c]] <= 0) {
        game->kill_card(game->target[c]);
        --game->hp[game->owner[c]];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " KILLS "
                 << game->description(game->target[c], false);
            if (game->hp[game->owner[c]] <= 0) {
                 board << " and KILLS PLAYER" << game->owner[c];
            } else {
                 board << " and DAMAGES PLAYER" << game->owner[c];
            }
            game->end_message();
        }
#endif
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void VorpalBunny::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS " << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    // damage_card?
    game->cardhp[game->target[c]] -= game->attack[c];
    if (game->cardhp[game->target[c]] <= 0) {
        game->kill_card(game->target[c]);
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " KILLS "
                 << game->description(game->target[c], false);
            game->end_message();
        }
#endif
    } else {
        --game->hp[game->owner[c]];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " FAILS To KILL "
                 << game->description(game->target[c], false);
            board << " and player loses life";
            game->end_message();
            if (game->hp[game->owner[c]] <= 0) {
                 board << " and KILLS PLAYER" << game->owner[c];
            } else {
                 board << " and DAMAGES PLAYER" << game->owner[c];
            }
            game->end_message();
        }
#endif
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void AntiAstralBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " giving astral magic-resistance to "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->properties[game->target[c]] |= CardProperty::BLUE_MAGIC_RESISTANT;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void AntiWorldlyBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " giving worldly magic-resistance to "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->properties[game->target[c]] |= CardProperty::RED_MAGIC_RESISTANT;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Curse::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " cursing "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->properties[game->target[c]] &= ~CardProperty::BLESSED;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void ImminentDeathBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " causing imminent death for "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->properties[game->target[c]] |= CardProperty::IMMINENT_DEATH;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void SleepBase::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->cardhp[game->target[c]] > game->attack[c]) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " not powerful enough to affect "
                 << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " immobilises "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->properties[game->target[c]] |= CardProperty::IMMOBILE;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Loyalty::execute(SpellCaster *game, int c, bool verbose) const {
    Mana current_cost = game->cost[game->target[c]];
    Mana new_cost = max(current_cost-Mana{game->attack[c], game->attack[c]}, Mana{0, 0});
    game->cost[game->target[c]] = new_cost;
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " reduces world cost of "
             << game->description(game->target[c], false) << " to " << new_cost.world
             << " and astral cost to " << new_cost.astral;
        game->end_message();
    }
#endif
    game->card_end_from(Location::EXECUTING, c, verbose);
}

#if 0
void OffByOne::execute(SpellCaster *game, int c, bool verbose) const {
    int target_card = game->target[c];
    // Safe to assume target_card not a player.
    int targets_target = game->target[target_card];
    if (targets_target == PLAYER0 || targets_target == PLAYER1) {
        if (verbose) {
            board << game->description(c, false) << " has no effect on "
                 << game->description(target_card, false)
                 << " as it targets a player";

        }
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
    auto target_location = find(game->in_play.begin(), game->in_play.end(), targets_target);
    if (target_location == game->in_play.end()) {
        if (verbose) {
            board << game->description(c, false) << " has no effect on "
                 << game->description(target_card, false)
                 << " as its target is no longer in play";
        }
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
    assert(*target_location == targets_target);
    if (verbose)
        board << target_location-game->in_play.begin();
    if (target_location == game->in_play.begin()) {
        if (verbose) {
            board << game->description(c, false) << " has no effect on "
                 << game->description(game->target[c], false)
                 << " as it is already targetting last card";
            game->end_message();
            game->card_end_from(Location::EXECUTING, c, verbose);
            return;
        }
    } else {
        --target_location;
        int new_target = *target_location;
        assert(*target_location != targets_target);
        game->target[target_card] = new_target;
        if (verbose) {
            board << game->description(c, false) << " causes "
                 << game->description(target_card, false)
                 << " to target " << game->description(new_target);
            game->end_message();
        }
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}
#endif

void Darkness::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " makes darkness fall on "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int bonus = game->attack[c];
    for (auto p = game->in_play.begin(); p != game->in_play.end(); ++p) {
        int card = *p;
        if (toBool(game->properties[card] & CardProperty::UNDEAD)) {
            game->attack[card] += bonus;
#ifdef BOARD
            if (verbose) {
                board << game->description(card, false) << " gains attack of "
                     << bonus;
            }
#endif
        }
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void BlueSkies::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " clears skies for " 
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int bonus = game->attack[c];
    for (auto p = game->in_play.begin(); p != game->in_play.end(); ++p) {
        int card = *p;
        if (toBool(game->properties[card] & CardProperty::FLYING)) {
            game->attack[card] += bonus;
#ifdef BOARD
            if (verbose) {
                board << game->description(card, false) << " gains attack of "
                     << bonus;
            }
#endif
        }
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

// Plague doesn't kill. Do I want that?
void Plague::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " causes plague on "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int bonus = game->attack[c];
    for (auto p = game->in_play.begin(); p != game->in_play.end(); ++p) {
        int card = *p;
        if (toBool(game->card_class[card] & CardClass::MONSTER)) {
            int old_hp = game->cardhp[*p];
            int new_hp = max(1, old_hp-bonus);
            game->cardhp[*p] = new_hp;
#ifdef BOARD
            if (verbose) {
                board << game->description(card, false) << " health drops to "
                     << new_hp;
            }
#endif
        }
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

#if 0
void Henge::execute(SpellCaster *game, int c, bool verbose) const {
    game->card_end_from(Location::EXECUTING, c, verbose);
}
#endif

void PerpetualMachineBase::execute(SpellCaster *game, int c, bool verbose) const {
    int bonus = game->attack[c];
    int player_number = game->target[c]-PLAYER0;
    game->mana[player_number] += Mana{bonus, bonus};
#ifdef BOARD
    if (verbose) {
        board << "Player " << player_number << " receives " << bonus << " MANA";
        game->end_message();
    }
#endif
    game->card_end_from(Location::EXECUTING, c, verbose);
}

#if 0
void ARollingStone::execute(SpellCaster *game, int c, bool verbose) const {
    int target_card = game->target[c];
    auto target_location = find(game->in_play.begin(),
                                game->in_play.end(),
                                target_card);
    assert(*target_location == target_card);
    int damage = game->attack[c];

    for (auto p = target_location; true; --p) {
        int card = *p;

        if (toBool(game->card_class[card] & CardClass::MONSTER)) {
            game->damage_card(card, damage, verbose);
        }

        if (p == game->in_play.begin()) {
            break;
        }
    }

    game->card_end_from(Location::EXECUTING, c, verbose);
}
#endif

// XXX Use this somewhere!
void FollowThroughBase::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " damages "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int target = game->target[c];
    game->cardhp[target] -= game->attack[c];
    if (game->cardhp[target] <= 0) {
        game->kill_card(target);
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false);
            board << " KILLS ";
            board << game->description(target, false);
            board << " and follows through to owner";
            game->end_message();
        }
#endif

        --game->hp[game->owner[target]];

        if (game->hp[game->owner[target]] <= 0) {
#ifdef BOARD
            if (verbose) {
                board << "And KILLS PLAYER" << game->owner[c];
                game->end_message();
            }
#endif
        }
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void ImmobilisingMonsterDefinition::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
    game->properties[game->target[c]] |= CardProperty::IMMOBILE;
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS and immobilises " << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->damage_card(game->target[c], game->attack[c]);
    game->card_end_from(Location::EXECUTING, c, verbose);
}

// Damage done goes to owner as mana
void VampiricMonster::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " ATTACKS and sucks life from ";
        board << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    int availableHp = game->damage_card(game->target[c], game->attack[c]);
    game->mana[game->owner[c]] += Mana{availableHp, availableHp};
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void dump_cards() {
#if 0
    for (auto p = all_cards.begin(); p != all_cards.end(); ++p) {
        board << (*p)->name;
    }
#endif
}

void Ambush::execute(SpellCaster *game, int c, bool verbose) const {
    int target_card = game->target[c];
    // Safe to assume target_card not a player.
    int targets_target = game->target[target_card];
    if (targets_target == PLAYER0 || targets_target == PLAYER1) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " has no effect on "
                 << game->description(target_card, false)
                 << " as it targets a player";

        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
    if (game->card_class[target_card] != CardClass::MONSTER) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " can only target "
                 << "a monster targetting a monster.";
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }

    auto card_target_location = find(game->in_play.begin(), game->in_play.end(), target_card);
    auto target_target_location = find(game->in_play.begin(), game->in_play.end(), targets_target);

    if (target_target_location == game->in_play.end()) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " has no effect on "
                 << game->description(target_card, false)
                 << " as its target is no longer in play";
        }
#endif
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }

    assert(*card_target_location == target_card);
    assert(*target_target_location == targets_target);
    swap(*card_target_location, *target_target_location);
    game->target[targets_target] = target_card;

#ifdef BOARD
    if (verbose) {
        board << game->description(targets_target, false) << " ambushes "
             << game->description(target_card, false);
    }
#endif

    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Link::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " linking "
             << game->description(game->target[c], false) 
             << " to owner.";
        game->end_message();
    }
#endif
    game->properties[game->target[c]] |= CardProperty::LINKED;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void ShardBase::execute(SpellCaster *game, int c, bool verbose) const {
    int total_damage = game->attack[c];
    int target_card = game->target[c];
    int damage = game->damage_card(target_card, game->attack[c], verbose);
    if (damage < total_damage) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c, false) << " follows through to player";
        }
#endif
        game->damage_player(game->owner[target_card], total_damage-damage);
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void EtherealRing::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " making "
             << game->description(game->target[c], false)
             << " ethereal";
        game->end_message();
    }
#endif
    game->properties[game->target[c]] |= CardProperty::ETHEREAL;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Rage::execute(SpellCaster *game, int c, bool verbose) const {
    int target_card = game->target[c];
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " sends "
             << game->description(target_card, false)
             << " into crazed frenzy.";
        game->end_message();
    }
#endif
    game->attack[target_card] += game->cardhp[target_card]-1;
    game->cardhp[target_card] = 1;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Flight::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " bestows flight on "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->properties[game->target[c]] |= CardProperty::FLYING;
    game->exclusions[game->target[c]] &= ~CardProperty::FLYING;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void FountainOfYouth::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " bestows regeneration on "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->properties[game->target[c]] |= CardProperty::REGENERATING;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Ground::execute(SpellCaster *game, int c, bool verbose) const {
#ifdef BOARD
    if (verbose) {
        board << game->description(c, false) << " grounds "
             << game->description(game->target[c], false);
        game->end_message();
    }
#endif
    game->properties[game->target[c]] &= ~CardProperty::FLYING;
    game->exclusions[game->target[c]] |= CardProperty::FLYING;
    game->card_end_from(Location::EXECUTING, c, verbose);
}

void Suspend::execute(SpellCaster *game, int c, bool verbose) const {
    if (game->owner[c] == game->owner[game->target[c]]) {
#ifdef BOARD
        if (verbose) {
            board << game->description(c) << " recalls ";
            board << game->description(game->target[c], false);
            game->end_message();
        }
#endif
        game->cost[c] = Mana{0, 0};
        game->return_card_from(Location::IN_PLAY, game->target[c], verbose);
    } else {
#ifdef BOARD
        if (verbose) {
            board << "Can only recall own card";
            game->end_message();
        }
#endif
    }
    game->card_end_from(Location::EXECUTING, c, verbose);
}

#if 0
void SpellEater::execute(SpellCaster *game, int c, bool verbose) const {
    int card_target = game->target[c];
    if (game->card_class[c] == CardClass::SPELL) {
        if (verbose) {
            board << game->description(c, false) << " eats spell " << game->description(target_card, false);
            game->end_message();
        }
        game->cardhp[c] += game->attack[target_card];
        game->basehp[c] += game->attack[target_card];
        game->destroy_card_from(Location::IN_PLAY, c, verbose);
        game->card_end_from(Location::EXECUTING, c, verbose);

        return;
    }
    if (game->target[c] == PLAYER0) {
        game->hp[0] -= game->attack[c];
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 0 doing damage " << game->attack[c];
            game->end_message();
        }
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    } else if (game->target[c] == PLAYER1) {
        game->hp[1] -= game->attack[c];
        if (verbose) {
            board << game->description(c, false) << " ATTACKS PLAYER 1 doing damage " << game->attack[c];
            game->end_message();
        }
        game->card_end_from(Location::EXECUTING, c, verbose);
        return;
    }
    if (verbose) {
        board << game->description(c, false) << " ATTACKS " << game->description(game->target[c], false);
        game->end_message();
    }
    game->damage_card(game->target[c], game->attack[c], verbose);
    game->card_end_from(Location::EXECUTING, c, verbose);
}
#endif

int ArtemisBow::computeAttack(SpellCaster *game, int card, int target, bool verbose) const {
    int attack = game->attack[card];
    if (toBool(game->properties[target] & CardProperty::FLYING)) {
        ++attack;
#ifdef BOARD
        if (verbose) {
            board << "Extra +1 damage because target is flying";
            game->end_message();
        }
#endif
    }
    return attack;
}

int AvengingAngel::computeAttack(SpellCaster *game, int card, int target, bool verbose) const {
    if (target == PLAYER0 || target == PLAYER1) {
        return game->attack[card];
    }
    return game->attack[target];
}

int Lightning::computeAttack(SpellCaster *game, int card, int target, bool verbose) const {
    int attack = game->attack[card];
    if (toBool(game->properties[target] & CardProperty::AQUATIC)) {
        ++attack;
#ifdef BOARD
        if (verbose) {
            board << "Extra +1 damage against aquatic monsters";
            game->end_message();
        }
#endif
    }
    return attack;
}

int SkeletonLord::computeAttack(SpellCaster *game, int card, int target, bool verbose) const {
    int attack = game->attack[card];
    int count = 0;
    for (auto p = game->in_play.begin(); p != game->in_play.end(); ++p) {
        count += game->hasProperty(*p, CardProperty::UNDEAD);
    }
    attack += count;
    if (count > 0) {
#ifdef BOARD
        if (verbose) {
            board << "Extra +" << count << " damage due to presence of undead";
            game->end_message();
        }
#endif
    }
    return attack;
}
