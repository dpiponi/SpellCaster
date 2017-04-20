#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>

using std::time;

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <deque>

#include "json11/json11.hpp"
using json11::Json;

#include "UCTRave.h"
#include "SpellCaster.h"
#include "read_file.h"

using std::cout;
using std::endl;
using std::vector;
using std::deque;
using std::ifstream;

int play(const Config &config,
          const vector<const Definition *> &player_deck,
          const vector<const Definition *> &computer_deck,
          const UCTParameters &player_params,
          const UCTParameters &computer_params) {

    shared_ptr<SpellCaster> game = make_shared<SpellCaster>(config, player_deck, computer_deck);

    while (true) {
        // Player 0 moves
        auto result = uctSearch(player_params, game, false).move;
        game->doMove(result);
        double value = game->evaluate();
        cout << "HP: " << game->hp[0] << ' ' << game->hp[1] << endl;
        if (value == -1.0) {
            // Loss for computer
            cout << "Computer wins" << endl;
            return -1;            
        }
        if (value == 1.0) {
            // Loss for player
            cout << "Player wins" << endl;
            return 1;
        }
        if (value == 0.0) {
            cout << "Draw" << endl;
            return 0;
        }
        cout << "Computer to move" << endl;

        // Player 1 moves
        result = uctSearch(computer_params, game, false).move;
        game->doMove(result);
        value = game->evaluate();
        cout << "HP: " << game->hp[0] << ' ' << game->hp[1] << endl;
        if (value == 1.0) {
            // Loss for player
            cout << "Computer wins" << endl;
            return -1;            
        }
        if (value == -1.0) {
            // Loss for computer
            cout << "Player wins" << endl;
            return 1;            
        }
        if (value == 0.0) {
            cout << "Draw" << endl;
            return 0;
        }
        cout << "Player to move" << endl;
    }
}

float uniform() {
    return rand()/float(RAND_MAX);
}

UCTParameters mutate(UCTParameters params) {
    int axis = int(uniform()*4);
    float c;
    float bias;
    float log_heuristic_amaf_multiplier;
    float log_heuristic_mc_multiplier;
    float range = 0.25;
    switch (axis) {
    case 0:
        c = params.c+range*(uniform()-0.5*range);
        if (c < 0.0) {
            c = 0;
        }
        params.c = c;
        return params;
    case 1:
        bias = params.bias+range*(uniform()-0.5*range);
        if (bias < 0.0) {
            bias = 0;
        }
        params.bias = bias;
        return params;
    case 2:
        log_heuristic_amaf_multiplier = log(params.heuristic_amaf_multiplier);
        log_heuristic_amaf_multiplier += range*(uniform()-0.5*range);
        if (log_heuristic_amaf_multiplier < 0.0) {
            log_heuristic_amaf_multiplier = 0;
        }
        params.heuristic_amaf_multiplier = exp(log_heuristic_amaf_multiplier);
        return params;
    case 3:
        log_heuristic_mc_multiplier = log(params.heuristic_mc_multiplier);
        log_heuristic_mc_multiplier += range*(uniform()-0.5*range);
        if (log_heuristic_mc_multiplier < 0.0) {
            log_heuristic_mc_multiplier = 0;
        }
        params.heuristic_mc_multiplier = exp(log_heuristic_mc_multiplier);
        return params;
    default:
        return params;
    }
}

void show(UCTParameters params) {
    cout << params.c << ' ' << params.bias << ' ' << params.heuristic_amaf_multiplier << ' ' << params.heuristic_mc_multiplier;
}

int main(int argc, char *argv[]) {
    srand(time(0));

    typedef SpellCaster Game;
    map<string, const Definition *> database = make_database(all_cards);
    vector<const Definition *> player_deck = read_deck(database, "player.txt");
    vector<const Definition *> computer_deck = read_deck(database, "computer.txt");

    Config config {/* draw_on_turn_end */           false,
                   /* max_cards_in_hand */          8,
                   false,
                   /* draw_cards_after_execution */ true,
                   /* auto_discard */               false,
                   /* must_self_discard */          false,
                   /* can_self_discard */           true,
                   /* auto_mana */                  false,
                   /* single pass */                true,
                   /* random_draw */                false,
                   /* draw_to_fill_hand */          true,
                   /* discard_for_mana */           true,
                   /* can_discard_any_for_mana */   false,
                   /* initial_hp */                 7};

    //2.8661 3.57922 64.4584 58.6079

    // Base
    UCTParameters params1 {
        /* allowed_time */              2.5,
        /* c */                         0.0,
        /* McRaveSchedule */            MINIMUM_MSE,
        /* k */                         0,
        /* bias */                      2.0,
        /* heuristic_amaf_multiplier */ 0,
        /* heuristic_mc_multiplier */   10,
        /* use_heuristic */             true};
    
    // Mutation
    UCTParameters params2 {
        /* allowed_time */              2.5,
        /* c */                         0.5,
        /* McRaveSchedule */            MINIMUM_MSE,
        /* k */                         0,
        /* bias */                      2.0,
        /* heuristic_amaf_multiplier */ 0,
        /* heuristic_mc_multiplier */   10,
        /* use_heuristic */             true};

    float n = 0;
    float m = 0;
    for (int i = 0; i < 1000; ++i) {
        int result = play(config, player_deck, computer_deck, params1, params2);
        m += 0.5*(result+1);
        ++n;
            cout << "So far:" << m << '/' << n << endl;
            cout << "z = " << (m-0.5*n)/sqrt(0.25*n) << endl;
    }
    exit(1);

#if 0
    float games = 0.0;
    float player_score = 0.0;
    while (true) {
        UCTParameters mutant = mutate(params);

        show(params);
        cout << " vs. ";
        show(mutant);
        cout << endl;
        int result = play(config,
                          player_deck,
                          computer_deck,
                          params,
                          mutant);
        if (result < 0) {
            params = mutant;
        }

        cout << "Current winner: ";
        show(params);
        cout << endl;
    }
#endif
}
