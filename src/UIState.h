#ifndef UISTATE_H
#define UISTATE_H

#include <memory>

#include <glad/glad.h>

#include "Rectangle.h"
#include "SpellCaster.h"
#include "Application.h"

using std::shared_ptr;

class UIState {
public:
    virtual void mouse(Application *app, int button, int action, int mode) { }
    virtual void motion(Application *app) { }
    virtual void idle(Application *app) { }
};

class WaitingForPlayerMoveToFinish : public UIState {
public:
    WaitingForPlayerMoveToFinish() { }
    void mouse(Application *app, int button, int action, int mode) override { }
    void idle(Application *app) override;
    void motion(Application *app) override;
};

class WaitingForComputerEvaluationToFinish : public UIState {
public:
    void mouse(Application *app, int button, int action, int mode) override { }
    void idle(Application *) override;
    void motion(Application *app) override;
};

class WaitingForComputerMoveToFinish : public UIState {
public:
    void mouse(Application *app, int button, int action, int mode) override { }
    void idle(Application *app) override;
    void motion(Application *app) override;
};

class WaitingForFirstCard : public UIState {
public:
    void mouse(Application *app, int button, int action, int mode) override;
    void idle(Application *) override { }
    void motion(Application *) override;
};

class WaitingForSecondCard : public UIState {
    int first_card;
public:
    WaitingForSecondCard(int f) : first_card(f) { }
    void mouse(Application *app, int button, int action, int mode) override;
    void idle(Application *app) override { }
    void motion(Application *) override;
};

void highlightLegalFirstCard(shared_ptr<SpellCaster> game);
void highlightLegalSecondCard(int first_card, shared_ptr<SpellCaster> game);

#endif
