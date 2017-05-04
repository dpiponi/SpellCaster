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
    float player_row;
    float computer_row;
    float player_column;
    float player_stats_row;
    float computer_stats_row;
    float player_stats_column;
public:
    BoardConfig() { } // XXX SHouldn't ever be used
    BoardConfig(Json json);
};

class BoardBase {
public:
    virtual void setUpBoard(shared_ptr<const SpellCaster> game, double time0, double time1) = 0;
    virtual void launch(int source_card, int target_card, double start_time, double end_time) = 0;
    virtual void glow(Vector3f colour, int source_card, int target_card, double start_time, double end_time, Vector2f target) = 0;
    virtual void claw(Vector3f colour, int source_card, int target_card, double start_time, double end_time, int attack) = 0;
    virtual void flame(Vector3f colour, int source_card, int target_card, double start_time, double end_time) = 0;
    virtual void zzz(Vector3f colour, int source_card, int target_card, double start_time, double end_time) = 0;
    virtual void expose(int card) = 0;
    virtual Vector2f positionOfPlayer(int player) const = 0; // Must go XXX
    virtual void focus(int player, int card, float delay, bool wobble = true) = 0;
    virtual void publicSetGraveyardPosition(double time, int c) = 0;
    virtual int arena(int card1, int card2, double start_time, double end_time) = 0;
    virtual void unArena(int arena_id, int card1, int card2, double time0, double time1) = 0;
    virtual void publicSetPlayerPosition(double time, double z = 0.0) = 0;

    virtual const BoardBase &operator<<(int x) const = 0;
    virtual const BoardBase &operator<<(string x) const = 0;

    virtual ~BoardBase() { }
};

class MockBoard : public BoardBase {
    void setUpBoard(shared_ptr<const SpellCaster> game, double time0, double time1) override { }
    void launch(int source_card, int target_card, double start_time, double end_time) override { }
    void glow(Vector3f colour, int source_card, int target_card, double start_time, double end_time, Vector2f target) override { }
    void claw(Vector3f colour, int source_card, int target_card, double start_time, double end_time, int attack) override { }
    void flame(Vector3f colour, int source_card, int target_card, double start_time, double end_time) override { }
    void zzz(Vector3f colour, int source_card, int target_card, double start_time, double end_time) override { }
    Vector2f positionOfPlayer(int player) const override { return Vector2f(0.0f, 0.0f); } // Must go XXX
    void expose(int card) override { }
    void focus(int player, int card, float delay, bool wobble = true) override { }
    void publicSetGraveyardPosition(double time, int c) override {}
    int arena(int card1, int card2, double start_time, double end_time) override { return 0; }
    void unArena(int arena_id, int card1, int card2, double time0, double time1) override { }
    void publicSetPlayerPosition(double time, double z = 0.0) override { }

    const MockBoard &operator<<(int x) const override { return *this; }
    const MockBoard &operator<<(string x) const override { return *this; }
};

class Board : public BoardBase {
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
        return Vector2f(config.hand_left+i*config.hand_spacing, player ? config.computer_row : config.player_row);
    }

    void setHandPosition(double time, int card, int player, int position_in_hand,
                         float size, float z, float offsetx, float offsety);

    void setUpHand(int player, int focus = -1, float delay = 0.5) {
        int n = hand[player].size();
#if 0
        float left_edge = config.hand_left-0.5*0.125;
        float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

        float left_centre = left_edge+0.5*0.125;
        float right_centre = right_edge-0.5*0.125;
#endif

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
    void launch4(int source_card, int target_card, double start_time, double end_time);
    void launch2(int source_card);
    void launch(int source_card, int target_card, double start_time, double end_time) override;
    void glow(Vector3f colour, int source_card, int target_card, double start_time, double end_time, Vector2f target) override;
    void launch5(int source_card, int target_card, double start_time, double end_time);
    void flame(Vector3f colour, int source_card, int target_card, double start_time, double end_time) override;
    void zzz(Vector3f colour, int source_card, int target_card, double start_time, double end_time) override;
    void claw(Vector3f colour, int source_card, int target_card, double start_time, double end_time, int attack) override;

    void initBackground();

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

    void expose(int card) override {
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

    int arena(int card1, int card2, double start_time, double end_time) override;
    void unArena(int arena_id, int card1, int card2, double time0, double time1) override;
    void unFocus(int player, int card, float delay);
    void focus(int player, int card, float delay, bool wobble = true) override;

    void setUpBoard(shared_ptr<const SpellCaster> game, double time0, double time1) override;

#if 0
    void setUpBoard(shared_ptr<const SpellCaster> game, double time0, double time1) {
        setUpBoard(game.get(), time0, time1);
    }
#endif

    template<class C>
    const Board &write(const C &x) const {
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

    const Board &operator<<(string x) const override {
        return write(x);
    }
    const Board &operator<<(int x) const override {
        return write(x);
    }

    void setNewMessage() const {
        std::lock_guard<std::mutex> guard(board_mutex);
        auto non_const = const_cast<Board *>(this);
        non_const->new_message = true;
    }

    void drawz(float zlo, float zhi, float ratio);

    void draw(float ratio) {
        std::lock_guard<std::mutex> guard(board_mutex);
        for (int i = 0; i < 10; ++i) {
            drawz(0.1*i, 0.1*(i+1), ratio);
        }

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GEQUAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
    }

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
        setText(word_stats0, ss.str().c_str(), config.player_stats_column, config.player_stats_row);
    }

    void renderComputerStats() {
        ostringstream st;
        st << "hp:" << hp[1]
           << "\nworld:" << mana[1].world
           << "\nastral:" << mana[1].astral;
        setText(word_stats1, st.str().c_str(), config.player_stats_column, config.computer_stats_row);
    }

    void publicSetHandPosition(double time, int card, int player, int position_in_hand,
                         float size, float z, float offsetx, float offsety) {
        std::lock_guard<std::mutex> guard(board_mutex);
        setHandPosition(time, card, player, position_in_hand, size, z, offsetx, offsety);
    }

    void publicSetPlayerPosition(double time, double z = 0.0) override {
        std::lock_guard<std::mutex> guard(board_mutex);
        setPlayerPosition(time, z);
    }

    void publicSetGraveyardPosition(double time, int c) override {
        std::lock_guard<std::mutex> guard(board_mutex);
        setGraveyardPosition(time, c);
    }

    Vector2f positionOfPlayer(int player) const override {
            switch (player) {
            case 0:
                return Vector2f(config.player_column, config.player_row);
            case 1:
                return Vector2f(config.player_column, config.computer_row);
            default:
                assert(false);
            }
            return Vector2f(0.0, 0.0);
    }
};

#endif 
