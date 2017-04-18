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

// Globals
shared_ptr<SpellCaster> game;

int play(const Config &config,
          const vector<const Definition *> &player_deck,
          const vector<const Definition *> &computer_deck,
          const UCTParameters &player_params,
          const UCTParameters &computer_params) {

    game = make_shared<SpellCaster>(config, player_deck, computer_deck);

    while (true) {
        // Player 0 moves
        auto result = uctSearch(player_params, game, false).move;
        game->doMove(result);
        double value = game->evaluate();
        cout << "HP: " << game->hp[0] << ' ' << game->hp[1] << endl;
        cout << "value = " << value << endl;
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
        cout << "value = " << value << endl;
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

int main(int argc, char *argv[]) {
    srand(time(0));

    typedef SpellCaster Game;
    map<string, const Definition *> database = make_database(all_cards);
    vector<const Definition *> player_deck = read_deck(database, "player.txt");
    vector<const Definition *> computer_deck = read_deck(database, "computer.txt");

    UCTParameters player_params {
        /* allowed_time */              5.0,
        /* c */                         2.0,
        /* McRaveSchedule */            MINIMUM_MSE,
        /* k */                         0,
        /* bias */                      2.0,
        /* heuristic_amaf_multiplier */ 10,
        /* heuristic_mc_multiplier */   10,
        /* use_heuristic */             true};

    UCTParameters computer_params {
        /* allowed_time */              0.5,
        /* c */                         1/sqrt(2.0),
        /* McRaveSchedule */            MINIMUM_MSE,
        /* k */                         0,
        /* bias */                      0.0,
        /* heuristic_amaf_multiplier */ 10,
        /* heuristic_mc_multiplier */   1000,
        /* use_heuristic */             false};

    Config config {/* draw_on_turn_end */ false,
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

    float games = 0.0;
    float player_score = 0.0;
    for (int i = 0; i < 100; ++i) {
        int result = play(config,
                          player_deck,
                          computer_deck,
                          player_params,
                          computer_params);
        ++games;
        player_score += 0.5*(1+result);
        cout << player_score << "/" << games << endl;
    }
}
