#include <map>
#include <string>
#include <algorithm>

using std::map;
using std::max;
using std::string;
using std::dynamic_pointer_cast;

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL_image.h>

#include "Math.h"
#include "SpellCaster.h"
#include "Board.h"

GLuint blob_tex, fire_tex, stroke_tex;

GLuint create_texture(const char *filename, bool repeat, bool mipmap) {
    static map<string, GLuint> cache;

    auto p = cache.find(filename);
    if (p != cache.end()) {
        return p->second;
    }

//    cout << "loading: " << filename << endl;
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
    if (mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
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

#if 0
void Board::launch3(int source_card) {
    std::lock_guard<std::mutex> guard(board_mutex);
    particles.resize(0);
    cout << "ACTUAL LAUNCH = " << source_card << ' ' << target[source_card] << endl;

//        static GLuint blob_tex = create_texture("assets/blob.png");

    float sx = cards[source_card]->getX();
    float sy = cards[source_card]->getY();
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
        tx = cards[target_card]->getX();
        ty = cards[target_card]->getY();
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
        particles.back().setAlpha(0.0, 1.0);
        particles.back().visible = true;
        particles.back().setNoHighlight();
    }
}
#endif

void addSegment(shared_ptr<Group> group, Vector2f start, Vector2f end, double time) {
    Rectangle segment;
    segment.visible = true;
    segment.setNoHighlight();
    segment.setTexture(stroke_tex);

    segment.setAngle(time, orientation(end-start));
    segment.setSize(time, 0.1, 0.5*(end-start).norm());
    segment.setPosition(time, 0.5*(start+end));
    segment.setZ(time, 0.96);
    segment.setAlpha(time, 1.0);

    group->addElement(make_shared<Rectangle>(segment));
}

template<class Gen>
void genLightning(shared_ptr<Group> particles, Vector2f src, Vector2f dst, int depth, Gen &generator, double time) {
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

// rocks
void Board::launch(int source_card, int target_card, double start_time, double end_time) {
    cout << "ACTUAL LAUNCH() = " << source_card << ' ' << target[source_card] << endl;

    Vector2f source(-0.5, 0.0);
    Vector2f target(0.5, 0.0);

    //GLuint tex = create_texture("assets/flaming rock.png");
    GLuint tex = create_texture("assets/miracle.png");

    std::minstd_rand0 generator(std::chrono::system_clock::now().time_since_epoch().count());
    uniform_real_distribution<float> offset(-1.0, 1.0);

    int nrocks = 30;
    Vector2f pos[nrocks], v[nrocks];
    for (int i = 0; i < nrocks; ++i) {
        pos[i] = target+Vector2f(-1.0, 1.0)+offset(generator)*Vector2f(0.5, -0.5)+offset(generator)*Vector2f(0.1, 0.1);
        v[i] = Vector2f(0.75, -0.75)+0.25*offset(generator)*Vector2f(1.0, -1.0);
    }

    int id = 0;
    auto particles = make_shared<Group>();

    double dt = (end_time-start_time)/10;
    for (int i = 0; i < nrocks; ++i) {
        shared_ptr<Rectangle> segment = make_shared<Rectangle>();
        for (int t = 0; t < 10; ++t) {
            double time = start_time+dt*t;

            segment->visible = true;
            segment->setNoHighlight();
            segment->setTexture(tex);
            segment->shadow = false;

            segment->setAngle(time, 0.0);
            segment->setSize(time, 0.075, 0.075);
            segment->setPosition(time, pos[i]);
            segment->setZ(time, 0.96);
            float alpha;
            float z = pos[i][1];
            if (z < 0) {
                alpha = 1+10.0*z;
            } else {
                alpha = 1-z;
            }
            alpha = max(0.5f+0.5f*alpha, 0.0f);
            alpha = min(alpha, 2.0f-0.2f*t);
            segment->setAlpha(time, alpha);

            pos[i] += dt*v[i];

        }
        particles->addElement(segment);
    }

    {
        std::lock_guard<std::mutex> guard(board_mutex);
        id = drawables.addElement(particles);
    }

    wait_until(end_time);

    {
        std::lock_guard<std::mutex> guard(board_mutex);
        drawables.removeElement(id);
    }
}

void Board::flame(Vector3f colour, int source_card, int target_card, double start_time, double end_time) {
    cout << "ACTUAL LAUNCH() = " << source_card << ' ' << target[source_card] << endl;

    Vector2f source(-0.5, 0.0);
    Vector2f target(0.5, 0.0);

    GLuint tex = create_texture("assets/glow.png");

    int id = 0;
    double dt = (end_time-start_time)/20;
    shared_ptr<Rectangle> segment = make_shared<Rectangle>();
    static GlowProgram *flame_program = ProgramRegistry::getProgram<GlowProgram>("flame");
    segment->setShader(flame_program);
    //glow_program.use();
    flame_program->setColor(colour);
    for (int t = 0; t < 20; ++t) {
        double time = start_time+dt*t;

        segment->visible = true;
        segment->setNoHighlight();
        segment->setTexture(tex);
        segment->shadow = false;

        segment->setAngle(time, 0.0);
        float size = 0.5;
        segment->setSize(time, size, size);
        segment->setPosition(time, target);
        segment->setZ(time, 0.99);
        float s = 0.05*t;
        float alpha = 2*linstep(0.0, 0.05, s)*linstep(1.0, 0.25, s);
        cout << "s=" <<s << endl;
        cout << "alpha=" <<alpha << endl;
        segment->setAlpha(time, alpha);
    }

    {
        std::lock_guard<std::mutex> guard(board_mutex);
        cout << "Adding segment element" << endl;
        id = drawables.addElement(segment);
    }

    cout << "Waiting" << endl;
    wait_until(end_time);
    cout << "Done" << endl;

    {
        std::lock_guard<std::mutex> guard(board_mutex);
        drawables.removeElement(id);
    }
}

void Board::claw(Vector3f colour, int source_card, int target_card, double start_time, double end_time, int n_claws) {
    cout << "ACTUAL LAUNCH() = " << source_card << ' ' << target[source_card] << endl;

    Vector2f source(-0.5, 0.0);
    Vector2f target(0.5, 0.0);

    GLuint tex1 = create_texture("assets/claw1.png");
    GLuint tex2 = create_texture("assets/claw2.png");
    GLuint tex3 = create_texture("assets/claw3.png");
    vector<GLuint> tex { tex1, tex2, tex3 };

    vector<int> id;
    double dt = (end_time-start_time)/20;

    for (int k = 0; k < n_claws; ++k) {
        shared_ptr<Rectangle> segment = make_shared<Rectangle>();
        for (int t = 0; t < 20; ++t) {

            double time = start_time+dt*t;
            int tt = 0.1*min(10, t-k);

            segment->visible = true;
            segment->setNoHighlight();
            segment->setTexture(tex[k % 3]);
            segment->shadow = false;

            segment->setAngle(time, 0.0);
            float size = tt*0.1;
            segment->setSize(time, size, size);
            float amplitude = 0.2*(0.5+0.5*size);
            Vector2f offset(-0.1+0.03*(-0.5*(n_claws-1)+k), 0.1-0.1*tt);
            segment->setPosition(time, target+offset);
            segment->setZ(time, 0.99);
            segment->setAlpha(time, 1.0);

            {
                std::lock_guard<std::mutex> guard(board_mutex);
                cout << "Adding segment element" << endl;
                id.push_back(drawables.addElement(segment));
            }
        }
    }

    cout << "Waiting" << endl;
    wait_until(end_time);
    cout << "Done" << endl;

    {
        std::lock_guard<std::mutex> guard(board_mutex);
        for (auto p : id) {
            drawables.removeElement(p);
        }
    }
}

void Board::zzz(Vector3f colour, int source_card, int target_card, double start_time, double end_time) {
    cout << "ACTUAL LAUNCH() = " << source_card << ' ' << target[source_card] << endl;

    Vector2f source(-0.5, 0.0);
    Vector2f target(0.5, 0.0);

    GLuint tex = create_texture("assets/zzz.png");

    vector<int> id;
    double dt = (end_time-start_time)/20;
    //static GlowProgram *flame_program = ProgramRegistry::getProgram<GlowProgram>("flame");
    //segment->setShader(flame_program);
    //glow_program.use();
    //flame_program->setColor(colour);
    for (int k = 0; k < 3; ++k) {
        shared_ptr<Rectangle> segment = make_shared<Rectangle>();
        for (int t = 0; t < 20; ++t) {
            double time = start_time+dt*t;

            segment->visible = true;
            segment->setNoHighlight();
            segment->setTexture(tex);
            segment->shadow = false;

            segment->setAngle(time, 0.0);
            double tt = t-2*k;
            float size = max(0.0, 0.125*0.05*tt);
            segment->setSize(time, size, size);
            float amplitude = 0.2*(0.5+0.5*size);
            Vector2f pos(amplitude*cos(8.0*0.05*tt), amplitude*sin(8.0*0.05*tt)+0.4*0.05*tt);
            segment->setPosition(time, target+pos+Vector2f(0.0, 0.1));
            segment->setZ(time, 0.99);
            float s = 0.05*t;
            float alpha = 2*linstep(0.0, 0.05, s)*linstep(1.0, 0.25, s);
            cout << "s=" <<s << endl;
            cout << "alpha=" <<alpha << endl;
            segment->setAlpha(time, alpha);
        }

        {
            std::lock_guard<std::mutex> guard(board_mutex);
            cout << "Adding segment element" << endl;
            id.push_back(drawables.addElement(segment));
        }
    }

    cout << "Waiting" << endl;
    wait_until(end_time);
    cout << "Done" << endl;

    {
        std::lock_guard<std::mutex> guard(board_mutex);
        for (auto p : id) {
            drawables.removeElement(p);
        }
    }
}

void Board::glow(Vector3f colour, int source_card, int target_card, double start_time, double end_time, Vector2f target) {
    cout << "ACTUAL LAUNCH() = " << source_card << ' ' << target[source_card] << endl;

    Vector2f source(-0.5, 0.0);
    //Vector2f target(0.5, 0.0);

    GLuint tex = create_texture("assets/glow.png");

    int id = 0;
    double dt = (end_time-start_time)/10;
    shared_ptr<Rectangle> segment = make_shared<Rectangle>();
    static GlowProgram *glow_program = ProgramRegistry::getProgram<GlowProgram>("glow");
    segment->setShader(glow_program);
    //glow_program.use();
    glow_program->setColor(colour);
    for (int t = 0; t < 10; ++t) {
        double time = start_time+dt*t;

        segment->visible = true;
        segment->setNoHighlight();
        segment->setTexture(tex);
        segment->shadow = false;

        segment->setAngle(time, 0.0);
        float size = 0.5*(1.0*0.1*t);//sqrt(triangle(2*(0.1*t)-0.5));
        segment->setSize(time, size, size);
        segment->setPosition(time, target);
        segment->setZ(time, 0.99);
        float alpha = sqrt(triangle(2*(0.1*t)-0.5));
        segment->setAlpha(time, alpha);
    }

    {
        std::lock_guard<std::mutex> guard(board_mutex);
        id = drawables.addElement(segment);
    }

    wait_until(end_time);

    {
        std::lock_guard<std::mutex> guard(board_mutex);
        drawables.removeElement(id);
    }
}

// Lightning
void Board::launch5(int source_card, int target_card, double start_time, double end_time) {
    cout << "ACTUAL LAUNCH() = " << source_card << ' ' << target[source_card] << endl;

    Vector2f source(-0.5, 0.0);
    Vector2f target(0.5, 0.0);

    std::minstd_rand0 generator(std::chrono::system_clock::now().time_since_epoch().count());

    int id = 0;
    for (int t = 0; t < 20; ++t) {
        auto particles = make_shared<Group>();

        for (int s = -2; s <= 2; ++s) {
        //for (int s = 0; s <= 0; ++s) {
            genLightning(particles, source+Vector2f(0.125, 0),
                                    target+Vector2f(-0.125, 0.05*s), 3, generator, start_time+0.05*t);
        }
        {
            std::lock_guard<std::mutex> guard(board_mutex);
            if (id) {
                drawables.removeElement(id);
            }
            id = drawables.addElement(particles);
        }
        wait_until(start_time+0.05*(t+1)); // Here? XXX
    }
    {
        std::lock_guard<std::mutex> guard(board_mutex);
        drawables.removeElement(id);
    }

//    particles.resize(0);

    //wait_until(end_time);
    //particles.resize(0);
}

#if 0
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
    particles.back().setAlpha(start_time);

    particles.back().setAngle(start_time+0.01, orientation(delta));
    particles.back().setZ(start_time+0.01, 0.95);
    particles.back().setPosition(start_time+0.01, middle);
    particles.back().setSize(start_time+0.01, 0.1, 0.5*l);
    particles.back().setAlpha(start_time+0.01, 1.0);

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

    float sx = cards[source_card]->getX();
    float sy = cards[source_card]->getY();
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
        tx = cards[target_card]->getX();
        ty = cards[target_card]->getY();
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
        particles.back().setAlpha(0.0, 1.0);
        particles.back().visible = true;
        particles.back().setNoHighlight();
    }
}
#endif

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
        cards[c]->setTexture(exposed[c] ? tex[c] : back_texture);
    }

    renderPlayerStats();
    renderComputerStats();

    setUpHand(0);
    setUpHand(1);

    int i = 0;
    for (auto p : in_play) {
        cards[p]->visible = true;
        cards[p]->setPosition(time0);
        cards[p]->setAngle(time0);
        setInPlayPosition(time1, p, i++);
    }
    for (auto p : graveyard) {
        cards[p]->visible = true;
        cards[p]->setPosition(time0);
        cards[p]->setAngle(time0);
        setGraveyardPosition(time1, p);
    }
    cout.flush();
}

int Board::arena(int card1, int card2, double start_time, double end_time) {
    std::lock_guard<std::mutex> guard(board_mutex);
    auto arena_rectangle = make_shared<Rectangle>();
    arena_rectangle->visible = true;
    arena_rectangle->shadow = false;
    arena_rectangle->setAlpha(start_time, 0.0);
    arena_rectangle->setAlpha(end_time, 1.0);
    arena_rectangle->setZ(start_time, 0.8);
    arena_rectangle->setPosition(start_time, 0.0, 0.0);
    arena_rectangle->setSize(start_time, 0.75, 0.35);
    arena_rectangle->setAngle(start_time, 0.0);
    arena_rectangle->setTexture(create_texture("assets/tiles.jpg"));

    int arena_id = drawables.addElement(arena_rectangle);

    float size = 0.125;

    cards[card1]->visible = true;
    cards[card1]->shadow = false;

    cards[card1]->setAlpha(start_time, 1.0);
    cards[card1]->setPosition(start_time);

    cards[card1]->setZ(start_time, 0.95);

    cards[card1]->setPosition(end_time, -0.5, 0);
    cards[card1]->setZ(end_time, 0.9);
    cards[card1]->setSize(end_time, size, 2*size);

    if (card2 == PLAYER0) {
        player.setZ(start_time, 0.95);
        //player.setPosition(start_time);

        player.setPosition(end_time, 0.5, 0);
        player.setZ(end_time, 0.95);
        player.setSize(end_time, size, 2*size);
        player.setAlpha(0.0, 1.0);
        player.visible = true;
        player.shadow = true;
        return arena_id;
    } else if (card2 == PLAYER1) {
        computer.setZ(start_time, 0.95);

        computer.setPosition(end_time, 0.5, 0);
        computer.setZ(end_time, 0.95);
        computer.setSize(end_time, size, 2*size);
        computer.setAlpha(0.0, 1.0);
        computer.visible = true;
        computer.shadow = true;
        return arena_id;
    }

    cards[card2]->setZ(start_time, 0.95);

    cards[card2]->setPosition(end_time, 0.5, 0);
    cards[card2]->setZ(end_time, 0.95);
    cards[card2]->setSize(end_time, size, 2*size);
    cards[card2]->setAlpha(0.0, 1.0);
    cards[card2]->visible = true;
    cards[card2]->shadow = true;

    return arena_id;
}

void Board::unArena(int arena_id, int card1, int card2, double time0, double time1) {
    {
        std::lock_guard<std::mutex> guard(board_mutex);
        //cards[card1]->setPosition(time0);
        player.setPosition(time0);

        cout << "Returning player" << endl;
        setPlayerPosition(time1, 0.95);
        cout << "Returning computer" << endl;
        setComputerPosition(time1, 0.95);

        auto arena_rectangle = dynamic_pointer_cast<Rectangle>(drawables.getElement(arena_id));
        arena_rectangle->setAlpha(time0, 1.0);
        arena_rectangle->setAlpha(time1, 0.0);
    }
    wait_until(time1);
    {
        std::lock_guard<std::mutex> guard(board_mutex);

        drawables.removeElement(arena_id);
    }
}

void Board::unFocus(int player, int card, float delay) {
    std::lock_guard<std::mutex> guard(board_mutex);
    int focus = find(hand[0].begin(), hand[0].end(), card)-hand[0].begin();
    //assert(game->hand[0][focus] == card);

    int n = hand[player].size();
    float left_edge = config.hand_left-0.5*0.125;
    float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

    float left_centre = left_edge+0.5*0.125;
    float right_centre = right_edge-0.5*0.125;

    auto p = hand[player][focus];
    setHandPosition(now()+delay, p, player, focus, 0.125, 0.1+0.001*focus, 0.0, 0.0);
}

void Board::focus(int player, int card, float delay, bool wobble) {
    std::lock_guard<std::mutex> guard(board_mutex);
    int focus = find(hand[player].begin(), hand[player].end(), card)-hand[player].begin();
    cout << "FOCUS " << player << ' ' << card << ' ' << focus << endl;
    assert(hand[player][focus] == card);

    int n = hand[player].size();
    float left_edge = config.hand_left-0.5*0.125;
    float right_edge = config.hand_left+config.hand_spacing*(n-1)+0.5*0.125;

    float left_centre = left_edge+0.5*0.125;
    float right_centre = right_edge-0.5*0.125;

    auto p = hand[player][focus];
    setHandPosition(now(), p, player, focus, 0.125, 0.2, 0.0, 0.0);
    setHandPosition(now()+0.25*delay, p, player, focus, 1.1*0.125, 0.5, 0.0, 0.0);
    setHandPosition(now()+0.5*delay, p, player, focus, 1.2*0.125, 0.999, 0.0, 0.0);
    if (wobble) {
        for (int k = 0; k < 100; ++k) {
            setHandPosition(now()+0.5*delay+0.3*k, p, player, focus, 1.2*0.125, 0.999, 0.004*cos(k), -0.004*sin(k));
        }
    }
}

void Board::initTextures(const vector<const Definition *> &player_deck,
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
        cards[i]->setTexture(back_texture);
    }

    blob_tex = create_texture("assets/blob.png");
    fire_tex = create_texture("assets/fire.png", true);
    stroke_tex = create_texture("assets/stroke.png", false, true);
    //create_texture("assets/colosseum.jpg");
    create_texture("assets/tiles.jpg");
    create_texture("assets/flaming rock.png");
    create_texture("assets/miracle.png");
    create_texture("assets/glow.png");
    create_texture("assets/zzz.png");
    create_texture("assets/claw1.png");
    create_texture("assets/claw2.png");
    create_texture("assets/claw3.png");

    cout << "...done" << endl;
}

void Board::setText(shared_ptr<LinearGroup> word, const char *msg, float x, float y) {
    float startx = x;
    int msg_len = strlen(msg);
    word->reset();
    float letter_size = 0.001;
    for (int i = 0; i < msg_len; ++i) {
        int c = msg[i];
        if (c >= 32 && c <= 127) {
            auto glyph = make_shared<TextRectangle>();
            glyph->setTexture(letters[c].texture);
            glyph->setPosition(0.0, Vector2f(
                                x+0.5*letter_size*letters[c].width+letter_size*letters[c].left,
                                y-0.5*letter_size*letters[c].height+letter_size*letters[c].top));
            glyph->setZ(0.0, 0.9);
            glyph->setSize(0.0,
                            0.5*letter_size*letters[c].width,
                            0.5*letter_size*letters[c].height);
            x = x+letter_size*letters[c].advance/64.0;
            glyph->setAlpha(0.0, 1.0);
            glyph->visible = true;
            glyph->setNoHighlight();
            word->appendElement(glyph);
        } else if (c == 10) {
            x = startx;
            y -= config.vertical_text_space;
        }
    }
}

void Board::setGraveyardPosition(double time, int c) {
    cards[c]->setPosition(time, Vector2f(1.3, 0.0));
    cards[c]->setZ(time, 0.9);
    cards[c]->setSize(time, 0.0625*1.5, 0.125*1.5);
    cards[c]->setAlpha(0.0, 1.0);
    cards[c]->setAngle(time, -2*M_PI);
    cards[c]->visible = true;
    cards[c]->shadow = true;
}

void Board::setInPlayPosition(double time, int c, int i) {
    //cards[c].setPosition(time, -1.1+0.27*i, owner[c] ==0 ? -0.025 : 0.025);
    cards[c]->setPosition(time, Vector2f(config.in_play_left+config.in_play_spacing*i,
                                        owner[c] == 0 ? -config.offset_for_player : config.offset_for_player));
    cards[c]->setZ(time, 0.0);
    cards[c]->setSize(0.0, 0.125, 0.25);
    cards[c]->setAlpha(0.0, 1.0);
    cards[c]->visible = true;
    cards[c]->shadow = true;
}

// For now connections are at a=0.5
void Board::drawConnection(float ratio, RGB rgb, int i, int j, int k) {
    float widthi = cards[i]->getXSize();
    float widthj = cards[j]->getXSize();
    float horizontal_height = 0.3+0.02*k;
    float ix = cards[i]->getX()-0.5*widthi;
    float jx = cards[j]->getX()+0.5*widthj;
    drawLine(ratio, config.border_line_width, rgb, ix, cards[i]->getY()+cards[i]->getYSize(),
                    ix, horizontal_height);
    drawLine(ratio, config.border_line_width, rgb, ix, horizontal_height,
                    jx, horizontal_height);
    drawLine(ratio, config.border_line_width, rgb, jx, horizontal_height,
                    jx, cards[j]->getY()+cards[j]->getYSize());
}

void Board::setPlayerPosition(double time, double z) {
//    cout << config.player_column << ' ' << config.player_row << endl;
    cout << "Computer position: " << config.player_column << ' ' << config.player_row << endl;
    player.setPosition(time, config.player_column, config.player_row);
    player.setZ(time, z);
    player.setSize(time, 0.125, 0.25);
    player.setAlpha(time, 1.0);
    player.visible = true;
    player.shadow = true;
}

void Board::setComputerPosition(double time, double z) {
    cout << "Player position: " << config.player_column << ' ' << config.computer_row << endl;
    computer.setPosition(time, config.player_column, config.computer_row);
    computer.setZ(time, z);
    computer.setSize(time, 0.125, 0.25);
    computer.setAlpha(time, 1.0);
    computer.visible = true;
    computer.shadow = true;
}

void Board::setAnnotation(int card) {
    std::lock_guard<std::mutex> guard(board_mutex);
    annotation.setTexture(tex[card]);;
    annotation.setPosition(now(), Vector2f(config.annotation_x, config.annotation_y));
    annotation.setZ(now(), 0.90);
    annotation.setSize(0.0, 0.5*config.annotation_height, config.annotation_height);
    annotation.setAlpha(0.0, 1.0);
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
    setText(word_annotation, summary.str().c_str(),
            config.annotation_x-0.5*config.annotation_height+0.01,
            config.annotation_y-config.annotation_height-config.vertical_text_space);
}

void Board::initBackground() {
    background.setTexture(create_texture(config.background.c_str()));
    background.setPosition(0.0, Vector2f(0.0, 0.0));
    background.setZ(0.0, 0.0);
    background.setSize(0.0, 1600.0/1172.0, 1.0);
    background.setAlpha(0.0, 1.0);
    background.visible = true;
}

void Board::initPlayers() {
    std::lock_guard<std::mutex> guard(board_mutex);
    initBackground();

    player.setTexture(create_texture(config.player_icon.c_str()));
    setPlayerPosition(now());

    computer.setTexture(create_texture(config.computer_icon.c_str()));
    setComputerPosition(now());

    passbutton.setTexture(create_texture("assets/passbutton.png"));
    passbutton.setPosition(0.0, 0.95, 0.1);
    passbutton.setZ(0.0, 0.0);
    passbutton.setSize(0.0, 0.2, 0.2/3.0);
    passbutton.setAlpha(0.0, 1.0);
    passbutton.visible = true;
    passbutton.shadow = true;

    discardbutton.setTexture(create_texture("assets/discardbutton.png"));
    discardbutton.setPosition(0.0, 0.95, -0.1);
    discardbutton.setZ(0.0, 0.0);
    discardbutton.setSize(0.0, 0.2, 0.2/3.0);
    discardbutton.setAlpha(0.0, 1.0);
    discardbutton.visible = true;
    discardbutton.shadow = true;
}

void Board::drawz(float zlo, float zhi, float ratio) {
    //connect_shader();

    background.drawz(zlo, zhi, ratio, config.border_line_width);

    if (zlo <= 0.0 && 0.0 < zhi) {
        ::drawShadow(ratio, -0.15, 0.95, 0.0, 0.0, 0.75, 0.1, 0.35);
    }

    player.drawz(zlo, zhi, ratio, config.border_line_width);
    computer.drawz(zlo, zhi, ratio, config.border_line_width);
    passbutton.drawz(zlo, zhi, ratio, config.border_line_width);
    discardbutton.drawz(zlo, zhi, ratio, config.border_line_width);
    word->drawz(zlo, zhi, ratio, 0.0);
    word_stats0->drawz(zlo, zhi, ratio, 0.0);
    word_stats1->drawz(zlo, zhi, ratio, 0.0);
    for (int i = 0; i < 2; ++i) {
        for (auto p : hand[i]) {
            cards[p]->drawz(zlo, zhi, ratio, config.border_line_width);
        }
    }
    for (auto p : in_play) {
        cards[p]->drawz(zlo, zhi, ratio, config.border_line_width);
    }
    for (auto p : graveyard) {
        cards[p]->drawz(zlo, zhi, ratio, config.border_line_width);
    }
    int k = 0;
    if (zlo <= 0.5 && 0.5 < zhi) {
        for (auto p : in_play) {
            int i = p;
            if (cards[i]->visible) {
                if (target[i] != -1) {
                    int j = target[i];
                    RGB rgb = owner[i] ? config.computer_highlight : config.player_highlight;
                    if (j < 1000) {
                        if (location[j] == Location::IN_PLAY) {
                            drawConnection(ratio, rgb, i, j, k);
                            ++k;
                        }
                    } else if (j==1001) {
                        float widthi = cards[i]->getXSize();
                        float horizontal_height = cards[i]->getY()+cards[i]->getYSize()+0.05;
                        drawLine(ratio, config.border_line_width, rgb, cards[i]->getX()-0.5*widthi, cards[i]->getY()+cards[i]->getYSize(),
                                        cards[i]->getX()-0.5*widthi, horizontal_height);
                    } else if (j==1000) {
                        float widthi = cards[i]->getXSize();
                        float horizontal_height = cards[i]->getY()-cards[i]->getYSize()-0.05;
                        drawLine(ratio, config.border_line_width, rgb, cards[i]->getX()-0.5*widthi, cards[i]->getY()-cards[i]->getYSize(),
                                        cards[i]->getX()-0.5*widthi, horizontal_height);
                    }
                }
            }
        }
    }
    if (zlo <= 0.9 && 0.9 < zhi) {
        if (annotation.visible) {
            ::drawShadow(ratio, -1.1, 0.0, 0.0, 0.0, 0.2, 0.8, 0.35);
            annotation.drawz(zlo, zhi, ratio, config.border_line_width);
            //draw_text(ratio, word_annotation);
            word_annotation->drawz(zlo, zhi, ratio, 0.0);
        }
    }

    drawables.drawz(zlo, zhi, ratio);
}

void Board::setHandPosition(double time, int c, int h, int i, float size, float z, float offsetx, float offsety) {
    cards[c]->setPosition(time, handPosition(h, i)+Vector2f(offsetx, offsety));
    cards[c]->setZ(time, z);
    cards[c]->setSize(time, size, 2*size);
    cards[c]->setAlpha(0.0, 1.0);
    cards[c]->visible = true;
    cards[c]->shadow = true;
}

void Board::shakePlayer(int p) {
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

void Board::unHighlightAll() {
    std::lock_guard<std::mutex> guard(board_mutex);
    computer.setNoHighlight();
    player.setNoHighlight();
    passbutton.setNoHighlight();
    discardbutton.setNoHighlight();
    for (auto p : in_play) {
        cards[p]->setNoHighlight();
    }
    for (auto p : hand[0]) {
        cards[p]->setNoHighlight();
    }
    for (auto p : hand[1]) {
        cards[p]->setNoHighlight();
    }
}

void Board::shake_card(int c) {
    std::lock_guard<std::mutex> guard(board_mutex);
    cout << "!!!!!!!!!!!!!!!!!!!!!!!1 SHAKING " << c << endl;
    for (int i = 0; i < 16; ++i) {
        cards[c]->setAngle(now()+0.05*i, -0.1+0.2*(i % 2));
    };
    cards[c]->setAngle(now()+0.06*16, 0.0);
}

int Board::contains(Point point) const {
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
    // We want last matching card
    // Or could traverse in reverse order...
    // Need depth order
    int last_card = -1;
    for (int i = 0; i < 2; ++i) {
        for (auto p : hand[i]) {
            if (cards[p]->contains(point)) {
                last_card = p;
            }
        }
    }
    for (auto p : in_play) {
        if (cards[p]->contains(point)) {
            last_card = p;
        }
    }
    for (auto p : graveyard) {
        if (cards[p]->contains(point)) {
            last_card = p;
        }
    }
    return last_card;
}

void Board::dump() {
    cout << "hand0: ";
    for (int i = 0; i < hand[0].size(); ++i) cout << hand[0][i] << ' ';
    cout << endl;
    for (int i = 0; i < in_play.size(); ++i) cout << in_play[i] << ' ';
    cout << endl;
    cout << "hand1: ";
    for (int i = 0; i < hand[1].size(); ++i) cout << hand[1][i] << ' ';
    cout << endl;
}

BoardConfig::BoardConfig(Json json) {
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
    player_row = json["player_row"].number_value();
    computer_row = json["computer_row"].number_value();
    player_column = json["player_column"].number_value();
    player_stats_column = json["player_stats_column"].number_value();
    player_stats_row = json["player_stats_row"].number_value();
    computer_stats_row = json["computer_stats_row"].number_value();
}
