#include <map>
#include <string>

using std::map;
using std::string;

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL_image.h>

#include "SpellCaster.h"
#include "Board.h"

GLuint blob_tex, fire_tex, stroke_tex;

GLuint create_texture(const char *filename, bool repeat) {
    static map<string, GLuint> cache;

    auto p = cache.find(filename);
    if (p != cache.end()) {
        return p->second;
    }

    cout << "loading: " << filename << endl;
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    SDL_Surface *texture_data = IMG_Load(filename);
    if (!texture_data) {
         printf("IMG_Load: %s\n", IMG_GetError());
         exit(1);
    }

    // Give the image to OpenGL
    SDL_LockSurface(texture_data);

    GLint nOfColors = texture_data->format->BytesPerPixel;
    GLenum texture_format;
    if (nOfColors == 4) {
        if (texture_data->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
    } else if (nOfColors == 3) {
        if (texture_data->format->Rmask == 0x000000ff)
            texture_format = GL_RGB;
    } else {
        cout << "warning: the image is not truecolor..  this will probably break" << endl;
        exit(1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_data->w, texture_data->h, 0, texture_format, GL_UNSIGNED_BYTE, texture_data->pixels);
    SDL_UnlockSurface(texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(texture_data);

    if (repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    cache[filename] = textureID;

    return textureID;
}

Letter::Letter(char c, FT_Face &face) : character(c) {
    FT_GlyphSlot g = face->glyph;

    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        cout << "FONT error" << endl;
        exit(1);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        g->bitmap.width,
        g->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        g->bitmap.buffer
    );

    width = g->bitmap.width;
    height = g->bitmap.rows;
    advance = g->advance.x;
    left = g->bitmap_left;
    top = g->bitmap_top;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    texture = textureID;
}

void Board::launch3(int source_card) {
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
        particles.back().setPosition(now(), x);
        float animation_length = 2.0;
        for (float t = 0; t < 1.5; t += dt) {
            particles.back().setPosition(start_time+t, x);
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

void addSegment(vector<Rectangle> &particles, Vector2f start, Vector2f end, double time) {
    particles.push_back(Rectangle());

    particles.back().visible = true;
    particles.back().setNoHighlight();
    particles.back().setTexture(stroke_tex);

    particles.back().setAngle(time, orientation(end-start));
    particles.back().setSize(time, 0.1, 0.5*(end-start).norm());
    particles.back().setPosition(time, 0.5*(start+end));
    particles.back().setZ(time, 0.96);
    particles.back().setBrightness(time, 1.0);
}

template<class Gen>
void genLightning(vector<Rectangle> &particles, Vector2f src, Vector2f dst, int depth, Gen &generator, double time) {
    if (depth == 0) {
        addSegment(particles, src, dst, time);
        return;
    }

    uniform_real_distribution<float> displacement(0.3, 0.7);
    float s = displacement(generator);

    Vector2f mid = (1-s)*src+s*dst;
    Vector2f tangent = dst-src;
    Vector2f normal = Vector2f(tangent[1], -tangent[0]);

    uniform_real_distribution<float> offset(-0.2, 0.2);
    float o = offset(generator);
    mid += o*normal;

    genLightning(particles, src, mid, depth-1, generator, time);
    genLightning(particles, mid, dst, depth-1, generator, time);
}

// Lightning
void Board::launch(int source_card, int target_card, double start_time, double end_time) {
    cout << "ACTUAL LAUNCH() = " << source_card << ' ' << target[source_card] << endl;

    Vector2f source(-0.5, 0.0);
    Vector2f target(0.5, 0.0);

    std::minstd_rand0 generator(std::chrono::system_clock::now().time_since_epoch().count());

    for (int t = 0; t < 20; ++t) {
        std::lock_guard<std::mutex> guard(board_mutex);
        particles.resize(0);

        for (int s = -2; s <= 2; ++s) {
        //for (int s = 0; s <= 0; ++s) {
            genLightning(particles, source+Vector2f(0.125, 0),
                                    target+Vector2f(-0.125, 0.05*s), 3, generator, start_time+0.05*t);
        }
        wait_until(start_time+0.05*(t+1));
    }
    particles.resize(0);

    //wait_until(end_time);
    //particles.resize(0);
}

// Fire
void Board::launch4(int source_card, int target_card, double start_time, double end_time) {
    std::lock_guard<std::mutex> guard(board_mutex);
    particles.resize(0);
    cout << "ACTUAL LAUNCH = " << source_card << ' ' << target[source_card] << endl;

    Vector2f source(-0.5, 0.0);
    Vector2f target(0.5, 0.0);

    Vector2f middle = 0.5*(source+target);
    Vector2f delta = target-source;
    float l = delta.norm();
    cout << "l = " << l << endl;

    particles.push_back(Rectangle());
    particles.back().setTexture(fire_tex);

    //double start_time = now();
    particles.back().setAngle(start_time);
    particles.back().setZ(start_time);
    particles.back().setPosition(start_time);
    particles.back().setSize(start_time);
    particles.back().setBrightness(start_time);

    particles.back().setAngle(start_time+0.01, orientation(delta));
    particles.back().setZ(start_time+0.01, 0.95);
    particles.back().setPosition(start_time+0.01, middle);
    particles.back().setSize(start_time+0.01, 0.1, 0.5*l);
    particles.back().setBrightness(start_time+0.01, 1.0);

    particles.back().visible = true;

    particles.back().setSize(end_time, 0.1, 0.5*l);
    particles.back().setSize(end_time, 0.0, 0.0);

    //wait_until(start_time+duration);
}

void Board::launch2(int source_card) {
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

void Board::setUpBoard(const SpellCaster *game, double time0, double time1) {
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
        cards[p].setPosition(time0);
        cards[p].setAngle(time0);
        setInPlayPosition(time1, p, i++);
    }
    for (auto p : graveyard) {
        cards[p].visible = true;
        cards[p].setPosition(time0);
        cards[p].setAngle(time0);
        setGraveyardPosition(time1, p);
    }
    cout.flush();
}

void Board::arena(int card1, int card2, double start_time, double end_time) {
    std::lock_guard<std::mutex> guard(board_mutex);
    float size = 0.125;

    cards[card1].visible = true;
    cards[card1].shadow = false;

    arenaVisible.addEvent(start_time, 1.0);
    cards[card1].setBrightness(start_time, 1.0);
    cards[card1].setPosition(start_time);

    cards[card1].setZ(start_time, 0.95);

    arenaVisible.addEvent(end_time, 1.0);
    cards[card1].setPosition(end_time, -0.5, 0);
    cards[card1].setZ(end_time, 0.9);
    cards[card1].setSize(end_time, size, 2*size);

    if (card2 == PLAYER0) {
        player.setZ(start_time, 0.95);
        player.setPosition(start_time);

        player.setPosition(end_time, 0.5, 0);
        player.setZ(end_time, 0.95);
        player.setSize(end_time, size, size);
        player.setBrightness(0.0, 1.0);
        player.visible = true;
        player.shadow = true;
        return;
    } else if (card2 == PLAYER1) {
        computer.setZ(start_time, 0.95);

        computer.setPosition(end_time, 0.5, 0);
        computer.setZ(end_time, 0.95);
        computer.setSize(end_time, size, size);
        computer.setBrightness(0.0, 1.0);
        computer.visible = true;
        computer.shadow = true;
        return;
    }

    cards[card2].setZ(start_time, 0.95);

    cards[card2].setPosition(end_time, 0.5, 0);
    cards[card2].setZ(end_time, 0.95);
    cards[card2].setSize(end_time, size, 2*size);
    cards[card2].setBrightness(0.0, 1.0);
    cards[card2].visible = true;
    cards[card2].shadow = true;
}

void Board::unArena(int card1, int card2, double time0, double time1) {
    std::lock_guard<std::mutex> guard(board_mutex);
    arenaVisible.addEvent(time0, 1.0);
    cards[card1].setPosition(time0);
    player.setPosition(time0);

    arenaVisible.addEvent(time1, 0.0);
    cout << "Returning player" << endl;
    setPlayerPosition(time1, 0.95);
    cout << "Returning computer" << endl;
    setComputerPosition(time1, 0.95);
}

void Board::unFocus(int player, int card, float delay) {
    std::lock_guard<std::mutex> guard(board_mutex);
    int focus = find(hand[0].begin(), hand[0].end(), card)-hand[0].begin();
    assert(game->hand[0][focus] == card);

    int n = hand[player].size();
    float left_edge = config.hand_left-0.5*0.125;
    float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

    float left_centre = left_edge+0.5*0.125;
    float right_centre = right_edge-0.5*0.125;

    vector<float> centres;
    pack(n, 2.0*0.125, left_edge, right_edge, centres);

    auto p = hand[player][focus];
    setHandPosition(now()+delay, p, player, focus, centres[focus], 0.125, 0.1+0.001*focus, 0.0, 0.0);
}

void Board::focus(int player, int card, float delay) {
    std::lock_guard<std::mutex> guard(board_mutex);
    int focus = find(hand[0].begin(), hand[0].end(), card)-hand[0].begin();
    assert(hand[0][focus] == card);

    int n = hand[player].size();
    float left_edge = config.hand_left-0.5*0.125;
    float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

    float left_centre = left_edge+0.5*0.125;
    float right_centre = right_edge-0.5*0.125;

    vector<float> centres;
    pack(n, 2.0*0.125, left_edge, right_edge, centres);

    auto p = hand[player][focus];
    setHandPosition(now(), p, player, focus, centres[focus], 0.125, 0.2, 0.0, 0.0);
    setHandPosition(now()+0.25*delay, p, player, focus, centres[focus], 1.1*0.125, 0.5, 0.0, 0.0);
    setHandPosition(now()+0.5*delay, p, player, focus, centres[focus], 1.2*0.125, 1.0, 0.0, 0.0);
    for (int k = 0; k < 100; ++k) {
        setHandPosition(now()+0.5*delay+0.3*k, p, player, focus, centres[focus], 1.2*0.125, 1.0, 0.004*cos(k), -0.004*sin(k));
    }
}
