#include <cassert>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <deque>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "json11/json11.hpp"
using json11::Json;

#include "linmath.h"
#include "UCTRave.h"
#include "SpellCaster.h"
#include "geometry.h"
#include "shaders.h"
#include "Draw.h"
#include "Animated.h"
#include "Rectangle.h"
#include "Board.h"
#include "UIState.h"
#include "Application.h"
#include "read_file.h"

using std::cout;
using std::endl;
using std::vector;
using std::deque;
using std::ifstream;

UCTParameters params {
    /* allowed_time */              5.0,
    /* c */                         0.5,
    /* McRaveSchedule */            MINIMUM_MSE,
    /* k */                         0,
    /* bias */                      0.1,
    /* heuristic_amaf_multiplier */ 1,
    /* heuristic_mc_multiplier */   1,
    /* use_heuristic */             false};

// Globals
shared_ptr<SpellCaster> game;
Board<SpellCaster> board;
std::future<Best<SpellCaster>> future_best;
std::future<void> future_player_move;
std::future<void> future_computer_move;
shared_ptr<UIState> ui_state;

class GameApp : public Application {
    Json shader;
    Json shadow_shader;
    Json text_shader;
    Json line_shader;
public:
    GameApp(Json json) {
        shader = json["shader"];
        shadow_shader = json["shadow_shader"];
        text_shader = json["text_shader"];
        line_shader = json["line_shader"];
    }
    void mouse(int button, int action, int mode) override {
        if (action==APP_PRESS) {
            ui_state->mouse(this, button, action, mode);
        }
    }
    void motion() override {
        ui_state->motion(this);
    }
    void key(int key, int scancode, int action, int mods) override {
#if 0
        if (key == APP_KEY_ESCAPE && action == APP_PRESS) {
            closeWindow();
        }
#endif
    }
    void idle() override {
        ui_state->idle(this);
    }
    void draw() override {
        float ratio;
        getFrameBufferSize(width, height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        board.draw(ratio);
        swapBuffers();
    }
    void init() {
        createWindow();
        initOpenGL();

        program = compileProgram(shader);

        glGenBuffers(1, &vertex_buffer);
        glGenBuffers(1, &line_vertex_buffer);
        line_program = compileProgram(line_shader);
        text_program = compileProgram(text_shader);
        shadow_program = compileProgram(shadow_shader);

        connect_shader();
        connect_line_shader();
        connect_text_shader();
    }
};

int main(int argc, char *argv[]) {
    string json_data = read_file("test.json");
    string error;
    Json my_json = Json::parse(json_data, error);
    if (my_json == Json()) {
        cout << "JSON error: " << error << endl;
        exit(1);
    }

    string font_name = my_json["font"].string_value();

    if (argc > 1) {
        int seed = atoi(argv[1]);
        cout << "Seed = " << seed << endl;
        srand(seed);
    }

    ui_state = make_shared<WaitingForFirstCard>();

    typedef SpellCaster Game;
    map<string, const Definition *> database = make_database(all_cards);
    vector<const Definition *> player_deck = read_deck(database, "all.txt");
    vector<const Definition *> computer_deck = read_deck(database, "all.txt");
    vector<const Definition *> definitions;

    definitions = player_deck;
    copy(computer_deck.begin(), computer_deck.end(), back_inserter(definitions));
    game = make_shared<SpellCaster>(default_config, player_deck.size(), computer_deck.size(), definitions);
    game->show();

    // OpenGL stuff starts here
    GameApp application(my_json);
    application.init();

    makeFont(font_name);

    BoardConfig board_config(my_json);
    board.setConfig(board_config);
    board.initCards(game->number_of_cards, player_deck, computer_deck);
    board.initPlayers();
    board << "SpellCaster";
    board.setNewMessage();
    board.setUpBoard(game);
    board << "Select card or pass";
    board.unHighlightAll();

    highlightLegalFirstCard(game);

    application.install();
    application.start();
    application.destroyWindow();
    application.terminate();
    exit(EXIT_SUCCESS);
}
