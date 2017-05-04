#ifndef BOARD_BASE_H
#define BOARD_BASE_H

#include <Eigen/Core>

using Eigen::Vector2f;
using Eigen::Vector3f;

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
#endif
