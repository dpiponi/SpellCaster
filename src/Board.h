#ifndef BOARD_H
#define BOARD_H

#include <mutex>
#include <iostream>
#include <vector>
#include <sstream>

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
using std::ostringstream;

GLuint create_texture(const char *filename);

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
    vector<Rectangle> cards;
    vector<GLuint> tex;
    GLuint back_texture;
    Rectangle background, player, computer, passbutton, discardbutton;
    Rectangle annotation;
    vector<Rectangle> word_annotation;
    vector<Rectangle> word;
    vector<Rectangle> word_stats0;
    vector<Rectangle> word_stats1;

    vector<vector<int>> hand;
    vector<int> graveyard;
    vector<int> in_play;
    vector<Location> location;
    vector<bool> exposed;
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
                word.back().setPosition(0.0,
                                    x+0.5*letter_size*letters[c].width+letter_size*letters[c].left,
                                    y-0.5*letter_size*letters[c].height+letter_size*letters[c].top);
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
        cards[c].setPosition(time, 1.3, 0.0);
        cards[c].setSize(time, 0.0625*1.5, 0.125*1.5);
        cards[c].setBrightness(0.0, 1.0);
        cards[c].setAngle(time, -2*M_PI);
        cards[c].visible = true;
        cards[c].shadow = true;
    }

    void setInPlayPosition(double time, int c, int i) {
        //cards[c].setPosition(time, -1.1+0.27*i, owner[c] ==0 ? -0.025 : 0.025);
        cards[c].setPosition(time, config.in_play_left+config.in_play_spacing*i,
                                   owner[c] == 0 ? -config.offset_for_player : config.offset_for_player);
        cards[c].setSize(0.0, 0.125, 0.25);
        cards[c].setBrightness(0.0, 1.0);
        cards[c].visible = true;
        cards[c].shadow = true;
    }

    void setHandPosition(double time, int c, int h, int i) {
        //cards[c].setPosition(time, -1.1+0.27*i, 1.2*h-0.6);
        cards[c].setPosition(time, config.hand_left+config.hand_spacing*i, 1.2*h-0.6);
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

    void initTextures(int number_of_cards,
                   const vector<const Definition *> &player_deck,
                   const vector<const Definition *> &computer_deck) {
        cout << "Loading textures..." << endl;
        setNumCards(number_of_cards);
        tex.resize(number_of_cards);
        back_texture = create_texture("assets/back.png");
        for (int i = 0; i < number_of_cards/2; ++i) {
            std::stringstream filename;
            filename << "assets/" << player_deck[i]->name << ".png";
            GLuint texture = create_texture(filename.str().c_str());
            if (texture > 0) {
                tex[i] = texture;
                tex[number_of_cards/2+i] = texture;
            } else {
                cout << "Couldn't load " << filename.str() << endl;
                std::stringstream filename2;
                filename2 << "assets/" << i << ".png";
                GLuint texture = create_texture(filename2.str().c_str());
                tex[i] = texture;
                tex[number_of_cards/2+i] = texture;
            }
        }
        for (int i = 0; i < number_of_cards; ++i) {
            cards[i].setTexture(back_texture);
        }

        cout << "...done" << endl;
    }

    void draw_text(float ratio, vector<Rectangle> &word) {
        for (auto p : word) {
            p.draw(ratio, 0.0, 1.0);
        }
    }

public:

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
        annotation.setPosition(now(), config.annotation_x, config.annotation_y);
        annotation.setSize(0.0, 0.5*config.annotation_height, config.annotation_height);
        annotation.setBrightness(0.0, 1.0);
        annotation.shadow = true;
        annotation.visible = true;
        ostringstream summary;
        summary << definitions[card]->name << '\n';
        summary << "\n";
        summary << "Base HP: " << basehp[card] << '\n';
        summary << "Current HP: " << cardhp[card] << '\n';
        summary << "Attack: " << attack[card] << '\n';
        summary << '\n';
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
            summary << "Target must not be:\n";
            summary << describe(exclusions[card]);
            summary << "\n";
        }
        setText(word_annotation, summary.str().c_str(), config.annotation_x-0.5*config.annotation_height, config.annotation_y-config.annotation_height-config.vertical_text_space);
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

    void initCards(int number_of_cards,
                   const vector<const Definition *> &player_deck,
                   const vector<const Definition *> &computer_deck) {
        std::lock_guard<std::mutex> guard(board_mutex);
        initTextures(number_of_cards, player_deck, computer_deck);
        for (int i = 0; i < number_of_cards; ++i) {
            cards[i].setTexture(back_texture);
        }
    }

    void initPlayers() {
        std::lock_guard<std::mutex> guard(board_mutex);
        //background.setTexture(create_texture("assets/Forest.png"));
        background.setTexture(create_texture(config.background.c_str()));
        background.setPosition(0.0, 0.0, 0.0);
        background.setSize(0.0, 1600.0/1172.0, 1.0);
        background.setBrightness(0.0, 1.0);
        background.visible = true;

        //player.setTexture(create_texture("assets/player.png"));
        player.setTexture(create_texture(config.player_icon.c_str()));
        player.setPosition(0.0, 0.95, -0.8);
        player.setSize(0.0, 0.1, 0.1);
        player.setBrightness(0.0, 1.0);
        player.visible = true;
        player.shadow = true;

        //computer.setTexture(create_texture("assets/computer.png"));
        computer.setTexture(create_texture(config.computer_icon.c_str()));
        computer.setPosition(0.0, 0.95, 0.8);
        computer.setSize(0.0, 0.1, 0.1);
        computer.setBrightness(0.0, 1.0);
        computer.visible = true;
        computer.shadow = true;

        passbutton.setTexture(create_texture("assets/passbutton.png"));
        passbutton.setPosition(0.0, 0.95, 0.1);
        passbutton.setSize(0.0, 0.2, 0.2/3.0);
        passbutton.setBrightness(0.0, 1.0);
        passbutton.visible = true;
        passbutton.shadow = true;

        discardbutton.setTexture(create_texture("assets/discardbutton.png"));
        discardbutton.setPosition(0.0, 0.95, -0.1);
        discardbutton.setSize(0.0, 0.2, 0.2/3.0);
        discardbutton.setBrightness(0.0, 1.0);
        discardbutton.visible = true;
        discardbutton.shadow = true;
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
        int i = 0;
        for (auto p : hand[0]) {
            cards[p].visible = true;
            setHandPosition(now()+0.5, p, 0, i++);
        }
        i = 0;
        for (auto p : hand[1]) {
            cards[p].visible = true;
            setHandPosition(now()+0.5, p, 1, i++);
        }
        i = 0;
        for (auto p : in_play) {
            cards[p].visible = true;
            setInPlayPosition(now()+0.5, p, i++);
        }
        for (auto p : graveyard) {
            cards[p].visible = true;
            setGraveyardPosition(now()+0.5, p);
        }
#if 0
        for (auto p = graveyard.begin(); p != graveyard.end(); ++p) {
            cards[*p].visible = false;
        }
        // Currently draws cards in crappy order
        // Need to draw in order coming from order in in_play, ...
        if (graveyard.size() > 1) {
            cards[graveyard[graveyard.size()-2]].visible = true;
            setGraveyardPosition(now()+0.5, graveyard[graveyard.size()-2]);
        }
        if (graveyard.size() > 0) {
            cards[graveyard.back()].visible = true;
            setGraveyardPosition(now()+0.5, graveyard.back());
        }
#endif
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

    void draw(float ratio) {
        std::lock_guard<std::mutex> guard(board_mutex);

        connect_shader();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        background.draw(ratio, config.border_line_width, 0.0);
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
        annotation.draw(ratio, config.border_line_width, 0.0);
        draw_text(ratio, word_annotation);
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