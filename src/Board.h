#ifndef BOARD_H
#define BOARD_H

#include <mutex>
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <algorithm>
#include <Eigen/Core>

#include "CardBase.h"
#include "Rectangle.h"
#include "TextRectangle.h"
#include "Letter.h"
#include "Mana.h"
#include "Location.h"

#include "json11/json11.hpp"
using json11::Json;

using std::vector;
using std::mutex;
using std::lock_guard;
using std::cout;
using std::endl;
using std::max;
using std::ostringstream;
using std::default_random_engine;
using std::uniform_real_distribution;

//
// Timing for instants
// 0:1s     Move to arena
// 1:2s     Effect
// 2:3s     Return and discard
//

GLuint create_texture(const char *filename, bool repeat = false, bool mipmap = true);
extern GLuint blob_tex, fire_tex, stroke_tex;

struct BoardConfig {
    float in_play_left;
    float hand_left;
    float in_play_spacing;
    float hand_spacing;
    float offset_for_player;
    float annotation_x;
    float annotation_y;
    float annotation_height;
    RGB player_highlight;
    RGB computer_highlight;
    float border_line_width;
    string computer_icon;
    string player_icon;
    string background;
    float vertical_text_space;
public:
    BoardConfig() { } // XXX SHouldn't ever be used
    BoardConfig(Json json) {
        in_play_left = json["in_play_left"].number_value();
        hand_left = json["hand_left"].number_value();
        in_play_spacing = json["in_play_spacing"].number_value();
        hand_spacing = json["hand_spacing"].number_value();
        offset_for_player = json["offset_for_player"].number_value();
        annotation_x = json["annotation_x"].number_value();
        annotation_y = json["annotation_y"].number_value();
        annotation_height = json["annotation_height"].number_value();
        player_highlight.r = json["player_highlight"].array_items()[0].number_value();
        player_highlight.g = json["player_highlight"].array_items()[1].number_value();
        player_highlight.b = json["player_highlight"].array_items()[2].number_value();
        computer_highlight.r = json["computer_highlight"].array_items()[0].number_value();
        computer_highlight.g = json["computer_highlight"].array_items()[1].number_value();
        computer_highlight.b = json["computer_highlight"].array_items()[2].number_value();
        border_line_width = json["border_line_width"].number_value();
        computer_icon = json["computer_icon"].string_value();
        player_icon = json["player_icon"].string_value();
        background = json["background"].string_value();
        vertical_text_space = json["vertical_text_space"].number_value();
    }
};

class Board {
    BoardConfig config;
    mutable std::mutex board_mutex;
    int ncards;
    vector<GLuint> tex;
    GLuint back_texture;
    Rectangle background, player, computer, passbutton, discardbutton;
    Rectangle annotation;
    shared_ptr<LinearGroup> word;
    shared_ptr<LinearGroup> word_annotation;
    shared_ptr<LinearGroup> word_stats0;
    shared_ptr<LinearGroup> word_stats1;
    Group drawables;

public:
    //vector<Rectangle> cards;
    vector<shared_ptr<Rectangle>> cards;
    vector<vector<int>> hand;
private:
    vector<int> graveyard;
    vector<int> in_play;
    vector<Location> location;
    vector<bool> exposed;
    vector<CardClass> card_class;
    vector<CardClass> target_class;
    vector<int> target;
    vector<int> owner;
    vector<int> cardhp;
    vector<int> basehp;
    vector<int> attack;
    vector<CardProperty> properties;
    vector<CardProperty> requirements;
    vector<CardProperty> exclusions;
    vector<const Definition *> definitions;
    int hp[2];
    Mana mana[2];

    bool new_message;
    ostringstream text_stream;

    void setNoText(shared_ptr<LinearGroup> word) {
        word->reset();
    }

    void setText(shared_ptr<LinearGroup> word, const char *msg, float x, float y);

    void setGraveyardPosition(double time, int c);

    void setInPlayPosition(double time, int c, int i);

    int contains(Point point) const;

    void clear() {
        text_stream.clear();
        text_stream.str("");
    }

    void dump();

    void initTextures(const vector<const Definition *> &player_deck,
                      const vector<const Definition *> &computer_deck);

    void drawConnection(float ratio, RGB rgb, int i, int j, int k);
    void setComputerPosition(double time, double z = 0.0);
    void setPlayerPosition(double time, double z = 0.0);

    Vector2f handPosition(int player, int i) {
        return Vector2f(config.hand_left+i*config.hand_spacing, player ? 0.6 : -0.6);
    }

    void setHandPosition(double time, int card, int player, int position_in_hand,
                         float size, float z, float offsetx, float offsety);

#if 0
    // Get position of ith card in a hand of n cards help by player p
    void positionInHand(int p, int i, int n) {
        float x = -0.5+0.1*i;
        float y = 
    }
#endif

    void setUpHand(int player, int focus = -1, float delay = 0.5) {
        int n = hand[player].size();
        float left_edge = config.hand_left-0.5*0.125;
        float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

        float left_centre = left_edge+0.5*0.125;
        float right_centre = right_edge-0.5*0.125;

        int i = 0;
        for (auto p : hand[player]) {
            cards[p]->visible = true;
            float a = n-1 > 0 ? float(i)/(n-1) : 0;
            setHandPosition(now()+delay, p, player, i, 0.125, 0.1+0.001*i, 0.0, 0.0);
            ++i;
        }
    }

// This is now the PUBLIC API
// Most stuff here needs protection with mutexes
public:
    Board() : new_message(true) {
        word = make_shared<LinearGroup>();
        word_annotation = make_shared<LinearGroup>();
        word_stats0 = make_shared<LinearGroup>();
        word_stats1 = make_shared<LinearGroup>();
    }

    // Shouldn't need mutex as this is done during startup.
    void setConfig(const BoardConfig &config0) {
        config = config0;
    }

    void setNumCards(int n) {
        ncards = n;
        cards.resize(n);
        for (int i = 0; i < n; ++i) {
            cards[i] = make_shared<Rectangle>();
        }
        hand.resize(2);
    }

    void launch3(int source_card);
    void launch(int source_card, int target_card, double start_time, double end_time);
    void launch4(int source_card, int target_card, double start_time, double end_time);
    void launch2(int source_card);

    //
    // Most of public API protected through mutexes because
    // board may be update by game in middle of rendering.
    //
    void highlightPlayer(int p) {
        std::lock_guard<std::mutex> guard(board_mutex);
        switch (p) {
        case 0:
            player.setHighlight(config.player_highlight);
            break;
        case 1:
            computer.setHighlight(config.computer_highlight);
            break;
        default:
            break;
        }
    }

    void shake_card(int c);
    void shakePlayer(int p);

    void noHighlightPlayer(int p) {
        std::lock_guard<std::mutex> guard(board_mutex);
        switch (p) {
        case 0:
            player.setNoHighlight();
            break;
        case 1:
            computer.setNoHighlight();
            break;
        default:
            break;
        }
    }

    void noHighlightPass() {
        std::lock_guard<std::mutex> guard(board_mutex);
        passbutton.setNoHighlight();
    }

    void highlightPass() {
        std::lock_guard<std::mutex> guard(board_mutex);
        passbutton.setHighlight(config.player_highlight);
    }

    void noHighlightDiscard() {
        std::lock_guard<std::mutex> guard(board_mutex);
        discardbutton.setNoHighlight();
    }

    void highlightDiscard() {
        std::lock_guard<std::mutex> guard(board_mutex);
        discardbutton.setHighlight(config.player_highlight);
    }

    void unHighlightAll();

    void highlightCard(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        if (owner[card] == 1) {
            cards[card]->setHighlight(config.computer_highlight);
        } else {
            cards[card]->setHighlight(config.player_highlight);
        }
    }

    void noHighlightCard(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        cards[card]->setNoHighlight();
    }

    void expose(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        cards[card]->setTexture(tex[card]);
    }

    void setAnnotation(int card);

    void setNoAnnotation() {
        std::lock_guard<std::mutex> guard(board_mutex);
        annotation.visible = false;
        setNoText(word_annotation);
    }

    void unExpose(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        cards[card]->setTexture(back_texture);
    }

    void initCards(const vector<const Definition *> &player_deck,
                   const vector<const Definition *> &computer_deck) {
        std::lock_guard<std::mutex> guard(board_mutex);
        initTextures(player_deck, computer_deck);
        int number_of_cards = player_deck.size()+computer_deck.size();
        for (int i = 0; i < number_of_cards; ++i) {
            cards[i]->setTexture(back_texture);
        }
    }

    void initPlayers();

    int arena(int card1, int card2, double start_time, double end_time);
    void unArena(int arena_id, int card1, int card2, double time0, double time1);
    void unFocus(int player, int card, float delay);
    void focus(int player, int card, float delay);

    void setUpBoard(const SpellCaster *game, double time0, double time1);

    void setUpBoard(shared_ptr<const SpellCaster> game, double time0, double time1) {
        setUpBoard(game.get(), time0, time1);
    }

    template<class C>
    const Board &operator<<(const C &x) const {
        std::lock_guard<std::mutex> guard(board_mutex);
        auto non_const = const_cast<Board *>(this);
        if (new_message) {
            non_const->clear();
            non_const->new_message = false;
        }
        non_const->text_stream << x;
        non_const->setText(non_const->word, text_stream.str().c_str(), -0.9, 0.9);
        return *this;
    }

    void setNewMessage() const {
        std::lock_guard<std::mutex> guard(board_mutex);
        auto non_const = const_cast<Board *>(this);
        non_const->new_message = true;
    }

    void draw(float ratio);

    int mouse_press(Point point) const {
        std::lock_guard<std::mutex> guard(board_mutex);
        int c = contains(point);
        if (c >= 0) {
//            cout << "Card clicked = " << c << endl; // ' ' << show_location(location[c]) << endl;
            return c;
            cout.flush();
        }
        cout.flush();
        return -1;
    }

    void renderPlayerStats() {
        ostringstream ss;
        ss << "hp:" << hp[0]
                    << "\nworld:" << mana[0].world
                    << "\nastral:" << mana[0].astral;
        setText(word_stats0, ss.str().c_str(), 1.1, -0.75);
    }

    void renderComputerStats() {
        ostringstream st;
        st << "hp:" << hp[1]
           << "\nworld:" << mana[1].world
           << "\nastral:" << mana[1].astral;
        setText(word_stats1, st.str().c_str(), 1.1, 0.85);
    }

    void publicSetHandPosition(double time, int card, int player, int position_in_hand,
                         float size, float z, float offsetx, float offsety) {
        std::lock_guard<std::mutex> guard(board_mutex);
        setHandPosition(time, card, player, position_in_hand, size, z, offsetx, offsety);
    }

    void publicSetPlayerPosition(double time, double z = 0.0) {
        std::lock_guard<std::mutex> guard(board_mutex);
        setPlayerPosition(time, z);
    }
};

#endif 
