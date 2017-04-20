#ifndef BOARD_H
#define BOARD_H

#include <mutex>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <Eigen/Core>

using Eigen::Vector2f;

#include "CardBase.h"
#include "Rectangle.h"

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

GLuint create_texture(const char *filename, bool repeat = false);
extern GLuint blob_tex, fire_tex;

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

template<class Game>
class Board {
    BoardConfig config;
    mutable std::mutex board_mutex;
    int ncards;
    vector<GLuint> tex;
    GLuint back_texture;
    Rectangle background, player, computer, passbutton, discardbutton;
    Rectangle annotation;
    vector<Rectangle> word_annotation;
    vector<Rectangle> word;
    vector<Rectangle> word_stats0;
    vector<Rectangle> word_stats1;

public:
    vector<Rectangle> cards;
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

    void setNoText(vector<Rectangle> &word) {
        word.resize(0);
    }

    void setText(vector<Rectangle> &word, const char *msg, float x, float y) {
        float startx = x;
        int msg_len = strlen(msg);
        word.resize(0);
        float letter_size = 0.001;
        for (int i = 0; i < msg_len; ++i) {
            int c = msg[i];
            if (c >= 32 && c <= 127) {
                word.push_back(Rectangle());
                word.back().setTexture(letters[c].texture);
                word.back().setPosition(0.0, Vector2f(
                                    x+0.5*letter_size*letters[c].width+letter_size*letters[c].left,
                                    y-0.5*letter_size*letters[c].height+letter_size*letters[c].top));
                word.back().setZ(0.0, 0.9);
                word.back().setSize(0.0,
                                0.5*letter_size*letters[c].width,
                                0.5*letter_size*letters[c].height);
                x = x+letter_size*letters[c].advance/64.0;
                word.back().setBrightness(0.0, 1.0);
                word.back().visible = true;
                word.back().setNoHighlight();
            } else if (c == 10) {
                x = startx;
                y -= config.vertical_text_space;
            }
        }
    }

    void setGraveyardPosition(double time, int c) {
        cards[c].setPosition(time, Vector2f(1.3, 0.0));
        cards[c].setZ(time, 0.0);
        cards[c].setSize(time, 0.0625*1.5, 0.125*1.5);
        cards[c].setBrightness(0.0, 1.0);
        cards[c].setAngle(time, -2*M_PI);
        cards[c].visible = true;
        cards[c].shadow = true;
    }

    void setInPlayPosition(double time, int c, int i) {
        //cards[c].setPosition(time, -1.1+0.27*i, owner[c] ==0 ? -0.025 : 0.025);
        cards[c].setPosition(time, Vector2f(config.in_play_left+config.in_play_spacing*i,
                                            owner[c] == 0 ? -config.offset_for_player : config.offset_for_player));
        cards[c].setZ(time, 0.0);
        cards[c].setSize(0.0, 0.125, 0.25);
        cards[c].setBrightness(0.0, 1.0);
        cards[c].visible = true;
        cards[c].shadow = true;
    }

    int contains(Point point) const {
        if (discardbutton.contains(point)) {
            return 3000;
        }
        if (passbutton.contains(point)) {
            return 2000;
        }
        if (player.contains(point)) {
            return 1000;
        }
        if (computer.contains(point)) {
            return 1001;
        }
#if 0
        for (int i = 0; i < ncards; ++i) {
            if (cards[i].contains(point)) {
                return i;
            }
        }
#endif
        // We want last matching card
        // Or could traverse in reverse order...
        int last_card = -1;
        for (int i = 0; i < 2; ++i) {
            for (auto p : hand[i]) {
                if (cards[p].contains(point)) {
                    last_card = p;
                }
            }
        }
        for (auto p : in_play) {
            if (cards[p].contains(point)) {
                last_card = p;
            }
        }
        for (auto p : graveyard) {
            if (cards[p].contains(point)) {
                last_card = p;
            }
        }
        return last_card;
    }

    void clear() {
        text_stream.clear();
        text_stream.str("");
    }

    void dump() {
        cout << "hand0: ";
        for (int i = 0; i < hand[0].size(); ++i) cout << hand[0][i] << ' ';
        cout << endl;
        for (int i = 0; i < in_play.size(); ++i) cout << in_play[i] << ' ';
        cout << endl;
        cout << "hand1: ";
        for (int i = 0; i < hand[1].size(); ++i) cout << hand[1][i] << ' ';
        cout << endl;
    }

    void initTextures(const vector<const Definition *> &player_deck,
                      const vector<const Definition *> &computer_deck) {
        int number_of_cards = player_deck.size()+computer_deck.size();
        cout << "Loading textures..." << endl;
        setNumCards(number_of_cards);
        tex.resize(number_of_cards);
        back_texture = create_texture("assets/back.jpg");

        int i = 0;
        for (auto p : player_deck) {
            std::stringstream filename;
            //filename << "assets/" << p->name << ".png";
            filename << "assets/" << p->name << ".jpg";
            GLuint texture = create_texture(filename.str().c_str());
            if (texture > 0) {
                tex[i] = texture;
            } else {
                cout << "Couldn't load " << filename.str() << endl;
                exit(1);
            }
            ++i;
        }
        for (auto p : computer_deck) {
            std::stringstream filename;
            filename << "assets/" << p->name << ".jpg";
            GLuint texture = create_texture(filename.str().c_str());
            if (texture > 0) {
                tex[i] = texture;
            } else {
                cout << "Couldn't load " << filename.str() << endl;
                exit(1);
            }
            ++i;
        }

        for (int i = 0; i < number_of_cards; ++i) {
            cards[i].setTexture(back_texture);
        }

        blob_tex = create_texture("assets/blob.png");
        fire_tex = create_texture("assets/fire.png", true);

        cout << "...done" << endl;
    }

    void draw_text(float ratio, vector<Rectangle> &word) {
        for (auto p : word) {
            p.drawText(ratio);
        }
    }

    void drawConnection(float ratio, RGB rgb, int i, int j, int k) {
        float widthi = cards[i].getXSize();
        float widthj = cards[j].getXSize();
        float horizontal_height = 0.3+0.02*k;
        float ix = cards[i].getX()-0.5*widthi;
        float jx = cards[j].getX()+0.5*widthj;
        drawLine(ratio, config.border_line_width, rgb, ix, cards[i].getY()+cards[i].getYSize(),
                        ix, horizontal_height);
        drawLine(ratio, config.border_line_width, rgb, ix, horizontal_height,
                        jx, horizontal_height);
        drawLine(ratio, config.border_line_width, rgb, jx, horizontal_height,
                        jx, cards[j].getY()+cards[j].getYSize());
    }

public:
    vector<Rectangle> particles; // XXX

    Board() : new_message(true) { }

    // Shouldn't need mutex as this is done during startup.
    void setConfig(const BoardConfig &config0) {
        config = config0;
    }

    void setNumCards(int n) {
        ncards = n;
        cards.resize(n);
        hand.resize(2);
    }

    void launch3(int source_card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        particles.resize(0);
        cout << "ACTUAL LAUNCH = " << source_card << ' ' << target[source_card] << endl;

//        static GLuint blob_tex = create_texture("assets/blob.png");

        float sx = cards[source_card].getX();
        float sy = cards[source_card].getY();
        float tx, ty;
        if (target[source_card] == PLAYER0) {
            cout << "Target = PLAYER0" << endl;
            tx = player.getX();
            ty = player.getY();
        } else if (target[source_card] == PLAYER1) {
            tx = computer.getX();
            ty = computer.getY();
        } else {
            cout << "Target = PLAYER1" << endl;
            int target_card = target[source_card];
            tx = cards[target_card].getX();
            ty = cards[target_card].getY();
        }
        Point target {tx, ty};
        for (int i = 0; i < 30; ++i) {
            Point x {sx, sy};
            float velocity_variation = 5.0;
            Point v {velocity_variation*(rand()/float(RAND_MAX)-0.5f), velocity_variation*(rand()/float(RAND_MAX)-0.5f) };

            particles.push_back(Rectangle());
            particles.back().setTexture(blob_tex);

            float dt = 0.02;
            float start_time = now()+0.05*i;
            particles.back().setAngle(start_time, 2*M_PI*rand()/float(RAND_MAX));
            particles.back().setZ(start_time, 0.95);
            particles.back().setPosition(now(), x.x, x.y);
            float animation_length = 2.0;
            for (float t = 0; t < 1.5; t += dt) {
                particles.back().setPosition(start_time+t, x.x, x.y);
                float size = 0.05*triangle(2.0*(t-0.5*animation_length)/animation_length);
                particles.back().setSize(start_time+t, size, size);
                x += dt*v;
                v = (1-15*dt)*v+200.0*dt*(target-x);
                t += dt;
            }
            particles.back().setSize(start_time+1.5, 0.0, 0.0);

            particles.back().setZ(0.0, 0.9);
            particles.back().setBrightness(0.0, 1.0);
            particles.back().visible = true;
            particles.back().setNoHighlight();
        }
    }

    // Fire
    void launch(int source_card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        particles.resize(0);
        cout << "ACTUAL LAUNCH = " << source_card << ' ' << target[source_card] << endl;

        float sx = cards[source_card].getX();
        float sy = cards[source_card].getY();
        Vector2f source(sx, sy);
        float tx, ty;
        if (target[source_card] == PLAYER0) {
            cout << "Target = PLAYER0" << endl;
            tx = player.getX();
            ty = player.getY();
        } else if (target[source_card] == PLAYER1) {
            tx = computer.getX();
            ty = computer.getY();
        } else {
            cout << "Target = PLAYER1" << endl;
            int target_card = target[source_card];
            tx = cards[target_card].getX();
            ty = cards[target_card].getY();
        }
        Vector2f target(tx, ty);

        Vector2f middle = 0.5*(source+target);
        Vector2f delta = target-source;
        float l = delta.norm();
        cout << "l = " << l << endl;

        particles.push_back(Rectangle());
        particles.back().setTexture(fire_tex);

        double start_time = now();
        particles.back().setAngle(start_time, atan2(-delta[0], delta[1]));
        particles.back().setZ(start_time, 0.95);
        particles.back().setPosition(start_time, middle);
        particles.back().setSize(start_time, 0.1, 0.5*l);
        particles.back().setBrightness(start_time, 1.0);
        particles.back().visible = true;

        particles.back().setSize(start_time+2.0, 0.1, 0.5*l);
        particles.back().setSize(start_time+2.1, 0.0, 0.0);
    }

    void launch2(int source_card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        particles.resize(0);
        cout << "ACTUAL LAUNCH = " << source_card << ' ' << target[source_card] << endl;

//        static GLuint blob_tex = create_texture("assets/blob.png");

        float sx = cards[source_card].getX();
        float sy = cards[source_card].getY();
        float tx, ty;
        if (target[source_card] == PLAYER0) {
            cout << "Target = PLAYER0" << endl;
            tx = player.getX();
            ty = player.getY();
        } else if (target[source_card] == PLAYER1) {
            tx = computer.getX();
            ty = computer.getY();
        } else {
            cout << "Target = PLAYER1" << endl;
            int target_card = target[source_card];
            tx = cards[target_card].getX();
            ty = cards[target_card].getY();
        }
        for (int i = 0; i < 20; ++i) {
            particles.push_back(Rectangle());
            particles.back().setTexture(blob_tex);

            float start_time = now()+0.05*i;
            float arrival_time = start_time+0.25;
            float interval = 0.25;
            float max_size = 0.05;
            particles.back().setAngle(start_time, 2*M_PI*rand()/float(RAND_MAX));
            particles.back().setZ(start_time, 0.95);
            particles.back().setPosition(start_time, sx, sy);
            particles.back().setSize(start_time, 0.0, 0.0);
            particles.back().setPosition(arrival_time+interval, tx-0.125, ty-0.25);
            particles.back().setSize(arrival_time+interval, max_size, max_size);
            particles.back().setPosition(arrival_time+2.0*interval, tx-0.125, ty+0.25);
            particles.back().setPosition(arrival_time+3.0*interval, tx+0.125, ty+0.25);
            particles.back().setPosition(arrival_time+4.0*interval, tx+0.125, ty-0.25);
            particles.back().setSize(arrival_time+4.0*interval, max_size, max_size);
            particles.back().setPosition(arrival_time+5.0*interval, tx, ty);
            particles.back().setSize(arrival_time+5.0*interval, 0.0, 0.0);
#if 0
            particles.back().setPosition(now(), -0.5, -0.5);
            particles.back().setPosition(now()+0.1*j+0.1*i,
                                -0.5+0.1*j,                    
                                -0.5+0.1*j);
            float s = 0.01*triangle(2.0*j/10-1);
            particles.back().setSize(now()+0.1*j+0.1*i, s, s);
#endif

            particles.back().setZ(0.0, 0.9);
            particles.back().setBrightness(0.0, 1.0);
            particles.back().visible = true;
            particles.back().setNoHighlight();
        }
    }

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

    void shake_card(int c) {
        std::lock_guard<std::mutex> guard(board_mutex);
        cout << "!!!!!!!!!!!!!!!!!!!!!!!1 SHAKING " << c << endl;
        for (int i = 0; i < 16; ++i) {
            cards[c].setAngle(now()+0.05*i, -0.1+0.2*(i % 2));
        };
        cards[c].setAngle(now()+0.06*16, 0.0);
    }

    void shakePlayer(int p) {
        std::lock_guard<std::mutex> guard(board_mutex);
        if (p==0) {
            cout << "!!!!!!!!!!!!!!!!!!!!!!!1 SHAKING player " << p << endl;
            for (int i = 0; i < 16; ++i) {
                player.setAngle(now()+0.05*i, -0.1+0.2*(i % 2));
            };
            player.setAngle(now()+0.06*16, 0.0);
        } else {
            cout << "!!!!!!!!!!!!!!!!!!!!!!!1 SHAKING player " << p << endl;
            for (int i = 0; i < 16; ++i) {
                computer.setAngle(now()+0.05*i, -0.1+0.2*(i % 2));
            };
            computer.setAngle(now()+0.06*16, 0.0);
        }
    }

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

    void unHighlightAll() {
        std::lock_guard<std::mutex> guard(board_mutex);
        computer.setNoHighlight();
        player.setNoHighlight();
        passbutton.setNoHighlight();
        discardbutton.setNoHighlight();
        for (auto p : in_play) {
            cards[p].setNoHighlight();
        }
        for (auto p : hand[0]) {
            cards[p].setNoHighlight();
        }
        for (auto p : hand[1]) {
            cards[p].setNoHighlight();
        }
    }

    void highlightCard(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        if (owner[card] == 1) {
            cards[card].setHighlight(config.computer_highlight);
        } else {
            cards[card].setHighlight(config.player_highlight);
        }
    }

    void noHighlightCard(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        cards[card].setNoHighlight();
    }

    void expose(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        cards[card].setTexture(tex[card]);
    }

    void setAnnotation(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        annotation.setTexture(tex[card]);;
        annotation.setPosition(now(), Vector2f(config.annotation_x, config.annotation_y));
        annotation.setZ(now(), 0.9);
        annotation.setSize(0.0, 0.5*config.annotation_height, config.annotation_height);
        annotation.setBrightness(0.0, 1.0);
        annotation.shadow = true;
        annotation.visible = true;
        ostringstream summary;
        summary << definitions[card]->name << '\n';
        summary << "\n";
        if (toBool(card_class[card])) {
            summary << describe(card_class[card]);
            summary << "\n";
        }
        summary << "HP: " << cardhp[card] << '/' << basehp[card] << '\n';
        summary << "Attack: " << attack[card] << '\n';
        summary << '\n';
        if (toBool(target_class[card])) {
            summary << "Can target:\n";
            summary << describe(target_class[card]);
            summary << "\n";
        }
        if (toBool(properties[card])) {
            summary << "Properties:\n";
            summary << describe(properties[card]);
            summary << "\n";
        }
        if (toBool(requirements[card])) {
            summary << "Target must be:\n";
            summary << describe(requirements[card]);
            summary << "\n";
        }
        if (toBool(exclusions[card])) {
            summary << "Target can't be:\n";
            summary << describe(exclusions[card]);
            summary << "\n";
        }
        setText(word_annotation, summary.str().c_str(), config.annotation_x-0.5*config.annotation_height+0.01, config.annotation_y-config.annotation_height-config.vertical_text_space);
    }

    void setNoAnnotation() {
        std::lock_guard<std::mutex> guard(board_mutex);
        annotation.visible = false;
        setNoText(word_annotation);
    }

    void unExpose(int card) {
        std::lock_guard<std::mutex> guard(board_mutex);
        cards[card].setTexture(back_texture);
    }

    void initCards(const vector<const Definition *> &player_deck,
                   const vector<const Definition *> &computer_deck) {
        std::lock_guard<std::mutex> guard(board_mutex);
        initTextures(player_deck, computer_deck);
        int number_of_cards = player_deck.size()+computer_deck.size();
        for (int i = 0; i < number_of_cards; ++i) {
            cards[i].setTexture(back_texture);
        }
    }

    void initPlayers() {
        std::lock_guard<std::mutex> guard(board_mutex);
        //background.setTexture(create_texture("assets/Forest.png"));
        background.setTexture(create_texture(config.background.c_str()));
        background.setPosition(0.0, Vector2f(0.0, 0.0));
        background.setZ(0.0, 0.0);
        background.setSize(0.0, 1600.0/1172.0, 1.0);
        background.setBrightness(0.0, 1.0);
        background.visible = true;

        //player.setTexture(create_texture("assets/player.png"));
        player.setTexture(create_texture(config.player_icon.c_str()));
        player.setPosition(0.0, 0.95, -0.8);
        player.setZ(0.0, 0.0);
        player.setSize(0.0, 0.1, 0.1);
        player.setBrightness(0.0, 1.0);
        player.visible = true;
        player.shadow = true;

        //computer.setTexture(create_texture("assets/computer.png"));
        computer.setTexture(create_texture(config.computer_icon.c_str()));
        computer.setPosition(0.0, 0.95, 0.8);
        computer.setZ(0.0, 0.0);
        computer.setSize(0.0, 0.1, 0.1);
        computer.setBrightness(0.0, 1.0);
        computer.visible = true;
        computer.shadow = true;

        passbutton.setTexture(create_texture("assets/passbutton.png"));
        passbutton.setPosition(0.0, 0.95, 0.1);
        passbutton.setZ(0.0, 0.0);
        passbutton.setSize(0.0, 0.2, 0.2/3.0);
        passbutton.setBrightness(0.0, 1.0);
        passbutton.visible = true;
        passbutton.shadow = true;

        discardbutton.setTexture(create_texture("assets/discardbutton.png"));
        discardbutton.setPosition(0.0, 0.95, -0.1);
        discardbutton.setZ(0.0, 0.0);
        discardbutton.setSize(0.0, 0.2, 0.2/3.0);
        discardbutton.setBrightness(0.0, 1.0);
        discardbutton.visible = true;
        discardbutton.shadow = true;
    }

    void setHandPosition(double time, int c, int h, float x, float size, float z, float offsetx, float offsety) {
        //cards[c].setPosition(time, -1.1+0.27*i, 1.2*h-0.6);
        cards[c].setPosition(time, Vector2f(x+offsetx, (h ? 0.6 : -0.6)+offsety));
        cards[c].setZ(time, z);
        cards[c].setSize(time, size, 2*size);
        cards[c].setBrightness(0.0, 1.0);
        cards[c].visible = true;
        cards[c].shadow = true;
    }

    void pack(int n, float width, float l, float r, vector<float> &centres) {
        if (n <= 0) {
            return;
        }
        float eps = 1e-5;
        float l_centre = l+0.5*width;
        float r_centre = r-0.5*width;
        for (int i = 0; i < n; ++i) {
            float a = float(i)/(n-1);
            float c = (1-a)*l_centre+a*r_centre;
            centres.push_back(c);
//            assert(c-0.5*width >= l-eps);
//            assert(c+0.5*width <= r+eps);
        }
    }

    void packFocus(int n, float width, float l, float r, int i, vector<float> &centres) {
        float l_centre = l+width/2.0;
        float r_centre = r-width/2.0;
        float a = n-1 > 0 ? float(i)/(n-1): 0;
        float c = (1-a)*l_centre+a*r_centre;
        float u = c-0.5*width;
        float v = c+0.5*width;
        pack(i, width, l, u, centres);
        centres.push_back(c);
        pack(n-i-1, width, v, r, centres);
        float eps = 1e-5;
        for (int i = 0; i < n; ++i) {
//            assert(centres[i]-0.5*width >= l-eps);
//            assert(centres[i]+0.5*width <= r+eps);
        }
    }

    void unFocus(int player, int focus, float delay) {
        int n = hand[player].size();
        float left_edge = config.hand_left-0.5*0.125;
        float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

        float left_centre = left_edge+0.5*0.125;
        float right_centre = right_edge-0.5*0.125;

        vector<float> centres;
        pack(n, 2.0*0.125, left_edge, right_edge, centres);

        auto p = hand[player][focus];
        setHandPosition(now()+delay, p, player, centres[focus], 0.125, 0.1+0.001*focus, 0.0, 0.0);
    }

    void focus(int player, int focus, float delay) {
        int n = hand[player].size();
        float left_edge = config.hand_left-0.5*0.125;
        float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

        float left_centre = left_edge+0.5*0.125;
        float right_centre = right_edge-0.5*0.125;

        vector<float> centres;
        pack(n, 2.0*0.125, left_edge, right_edge, centres);

        auto p = hand[player][focus];
        setHandPosition(now(), p, player, centres[focus], 0.125, 0.2, 0.0, 0.0);
        setHandPosition(now()+0.25*delay, p, player, centres[focus], 1.1*0.125, 0.5, 0.0, 0.0);
        setHandPosition(now()+0.5*delay, p, player, centres[focus], 1.2*0.125, 1.0, 0.0, 0.0);
        for (int k = 0; k < 100; ++k) {
            setHandPosition(now()+0.5*delay+0.3*k, p, player, centres[focus], 1.2*0.125, 1.0, 0.004*cos(k), -0.004*sin(k));
        }
    }

    void setUpHand(int player, int focus = -1, float delay = 0.5) {
        int n = hand[player].size();
        float left_edge = config.hand_left-0.5*0.125;
        float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

        float left_centre = left_edge+0.5*0.125;
        float right_centre = right_edge-0.5*0.125;

        vector<float> centres;
        pack(n, 2.0*0.125, left_edge, right_edge, centres);

        int i = 0;
        for (auto p : hand[player]) {
            cards[p].visible = true;
            float a = n-1 > 0 ? float(i)/(n-1) : 0;
//            if (i != focus) {
                setHandPosition(now()+delay, p, player, centres[i], 0.125, 0.1+0.001*i, 0.0, 0.0);
//            } else {
//                setHandPosition(now(), p, player, centres[i], 0.125, 0.2, 0.0, 0.0);
//                setHandPosition(now()+0.25*delay, p, player, centres[i], 1.1*0.125, 0.5, 0.0, 0.0);
//                setHandPosition(now()+0.5*delay, p, player, centres[i], 1.2*0.125, 1.0, 0.0, 0.0);
//                for (int k = 0; k < 100; ++k) {
//                    setHandPosition(now()+0.5*delay+0.3*k, p, player, centres[i], 1.2*0.125, 1.0, 0.004*cos(k), -0.004*sin(k));
//                }
//            }
            ++i;
        }
#if 0
        if (focus >= 0) {
            cards[focus].setSize(now()+delay, 0.125, 0.25);
            cards[focus].setSize(now()+2*delay, 1.2*0.125, 1.2*0.25);
        }
#endif
//        cout << endl;
    }

    void setUpBoard(const Game *game) {
        std::lock_guard<std::mutex> guard(board_mutex);
        for (int i = 0; i < 2; ++i) {
            hp[i] = game->hp[i];
            mana[i] = game->mana[i];
            hand[i] = game->hand[i];
        }
        in_play = game->in_play;
        target = game->target;
        cardhp = game->cardhp;
        basehp = game->basehp;
        attack = game->attack;
        properties = game->properties;
        exclusions = game->exclusions;
        requirements = game->requirements;
        card_class = game->card_class;
        target_class = game->target_class;
        definitions = game->definitions;
        exposed = game->exposedTo[0];
        location = game->location;
        owner = game->owner;
        graveyard = game->graveyard;
        for (int c = 0; c < exposed.size(); ++c) {
            cards[c].setTexture(exposed[c] ? tex[c] : back_texture);
        }
	ostringstream ss;
        ss << "hp:" << hp[0]
		    << "\nworld:" << mana[0].world
		    << "\nastral:" << mana[0].astral;
        setText(word_stats0, ss.str().c_str(), 1.1, -0.75);
	ostringstream st;
        st << "hp:" << hp[1]
	   << "\nworld:" << mana[1].world
	   << "\nastral:" << mana[1].astral;
        setText(word_stats1, st.str().c_str(), 1.1, 0.85);
        setUpHand(0);
        setUpHand(1);
        int i = 0;
        for (auto p : in_play) {
            cards[p].visible = true;
            setInPlayPosition(now()+0.5, p, i++);
        }
        for (auto p : graveyard) {
            cards[p].visible = true;
            setGraveyardPosition(now()+0.5, p);
        }
        cout.flush();
    }

    void setUpBoard(shared_ptr<const SpellCaster> game) {
        setUpBoard(game.get());
    }

    template<class C>
    const Board<Game> &operator<<(const C &x) const {
        std::lock_guard<std::mutex> guard(board_mutex);
        auto non_const = const_cast<Board<SpellCaster> *>(this);
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
        auto non_const = const_cast<Board<SpellCaster> *>(this);
        non_const->new_message = true;
    }

    void draw(float ratio) {
        std::lock_guard<std::mutex> guard(board_mutex);

        //connect_shader();
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GEQUAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        background.draw(ratio, config.border_line_width, 0.0);

        ::drawShadow(ratio, -0.15, 0.95, 0.0, 0.0, 0.75, 0.1, 0.35);

        player.draw(ratio, config.border_line_width, 0.0);
        computer.draw(ratio, config.border_line_width, 0.0);
        passbutton.draw(ratio, config.border_line_width, 0.0);
        discardbutton.draw(ratio, config.border_line_width, 0.0);
        draw_text(ratio, word);
        draw_text(ratio, word_stats0);
        draw_text(ratio, word_stats1);
        for (int i = 0; i < 2; ++i) {
            for (auto p : hand[i]) {
                cards[p].draw(ratio, config.border_line_width, 0.0);
            }
        }
        for (auto p : in_play) {
            cards[p].draw(ratio, config.border_line_width, 0.0);
        }
#if 0
        // Only draw last 10 or so in graveyard
        auto g = max(0UL, graveyard.size()-10);
        for (auto i = g; i < graveyard.size(); ++i) {
            cards[graveyard[i]].draw(ratio, config.border_line_width, 0.0);
        }
#endif
        for (auto p : graveyard) {
            cards[p].draw(ratio, config.border_line_width, 0.0);
        }
        int k = 0;
        for (auto p : in_play) {
            int i = p;
            if (cards[i].visible) {
                if (target[i] != -1) {
                    int j = target[i];
                    RGB rgb = owner[i] ? config.computer_highlight : config.player_highlight;
                    if (j < 1000) {
                        if (location[j] == Location::IN_PLAY) {
                            drawConnection(ratio, rgb, i, j, k);
                            ++k;
                        }
                    } else if (j==1001) {
                        float widthi = cards[i].getXSize();
                        float horizontal_height = cards[i].getY()+cards[i].getYSize()+0.05;
                        drawLine(ratio, config.border_line_width, rgb, cards[i].getX()-0.5*widthi, cards[i].getY()+cards[i].getYSize(),
                                        cards[i].getX()-0.5*widthi, horizontal_height);
                    } else if (j==1000) {
                        float widthi = cards[i].getXSize();
                        float horizontal_height = cards[i].getY()-cards[i].getYSize()-0.05;
                        drawLine(ratio, config.border_line_width, rgb, cards[i].getX()-0.5*widthi, cards[i].getY()-cards[i].getYSize(),
                                        cards[i].getX()-0.5*widthi, horizontal_height);
                    }
                }
            }
        }
        if (annotation.visible) {
            ::drawShadow(ratio, -1.1, 0.0, 0.0, 0.0, 0.2, 0.8, 0.35);
            annotation.draw(ratio, config.border_line_width, 0.0);
            draw_text(ratio, word_annotation);
        }
        // Particles
        if (particles.size() > 0) {
            for (auto p : particles) {
                p.draw(ratio, 0.0, 0.0);
            }
        }
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
};

#endif 
