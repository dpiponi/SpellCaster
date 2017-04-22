#include "UIState.h"
#include "Rectangle.h"
#include "Board.h"
#include "SpellCaster.h"
#include "Application.h"

extern Board<SpellCaster> board;
extern shared_ptr<SpellCaster> game;
extern shared_ptr<UIState> ui_state;
extern std::future<Best<SpellCaster>> future_best;
extern std::future<void> future_player_move;
extern std::future<void> future_computer_move;
extern UCTParameters params;

void highlightLegalFirstCard(shared_ptr<SpellCaster> game) {
    auto moves = game->legalMoves();
    for (auto p = moves.begin(); p != moves.end(); ++p) {
        if (p->card >= 0 && p->target != 1002) {
            board.highlightCard(p->card);
        } else if (p->card == -1) {
            board.highlightPass();
        }
    }
    cout << endl;
}

void highlightLegalSecondCard(int first_card, shared_ptr<SpellCaster> game) {
    auto moves = game->legalMoves();
    for (auto p = moves.begin(); p != moves.end(); ++p) {
        if (p->card == first_card) {
            if (p->target >= 0 && p->target < 1000) {
                board.highlightCard(p->target);
            } else if (p->target == DISCARD) {
                board.highlightDiscard();
            } else if (p->target == PLAYER0) {
                board.highlightPlayer(0);
            } else if (p->target == PLAYER1) {
                board.highlightPlayer(1);
            }
        }
    }
}

static void WaitingForFirstCardReceived(int first_card);

void WaitingForSecondCard::mouse(Application *app, int button, int action, int mode) {
    Point point = app->getMousePixels();
    int second_card = board.mouse_press(point);

    if (second_card >= 0) {
        if (second_card == first_card) {
            // Unselection
            board.unHighlightAll();

//            int pos = find(game->hand[0].begin(), game->hand[0].end(), first_card)-game->hand[0].begin();
//            assert(game->hand[0][pos] == first_card);
            board.unFocus(0, first_card, 0.1);

            ui_state = make_shared<WaitingForFirstCard>();
            return;
        }
        // XXX Bug! second_card might point to player, not actual card
        if (second_card < 1000 && game->location[second_card] == Location::HAND0) {
            board.unHighlightAll();

            board.unFocus(0, first_card, 0.1);

            WaitingForFirstCardReceived(second_card);
            return;
        }
        SpellCaster::Move id = SpellCaster::Move(0, first_card, second_card == 3000 ? DISCARD : second_card);
        if (!game->isLegalId(id, true)) {
            cout << "ILLEGAL!!!!!!!!!!!!!!" << endl;
            highlightLegalFirstCard(game);
            cout << "Carry on waiting for second card???" << endl;
            return;
        }

        cout << first_card << " THEN " << second_card << endl;

        auto analysis_game = make_shared<SpellCaster>(*game);
        assert(analysis_game->nextPlayer == 0);
        analysis_game->doMove(id, false);
        assert(analysis_game->nextPlayer == 1);

        future_player_move = std::async(std::launch::async, [id]() {
            game->doMove(id, true);
        });

        if (analysis_game->evaluate() == NON_TERMINAL) {
            future_best = std::async(std::launch::async, [analysis_game]() {
                cout << "!!!!! Starting AI" << endl;
                auto result = uctSearch(params, analysis_game, true);
                cout << "!!!!! AI done" << endl;
                return result;
            });
        }

        cout << "state = State::WAITING_FOR_PLAYER_MOVE_TO_FINISH" << endl;
        ui_state = make_shared<WaitingForPlayerMoveToFinish>();
    }
}

void playerPasses() {
    board.unHighlightAll();

    auto analysis_game = make_shared<SpellCaster>(*game);
    analysis_game->doMove(SpellCaster::Move(0, -1, -1), false);
    assert(analysis_game->nextPlayer == 1);

    future_player_move = std::async(std::launch::async, []() {
        game->doMove(SpellCaster::Move(0, -1, -1), true);
        game->show();
    });

    if (analysis_game->evaluate() == NON_TERMINAL) {
        future_best = std::async(std::launch::async, [analysis_game]() {
            cout << "!!!!! Starting AI" << endl;
            auto result = uctSearch(params, analysis_game, true);
            cout << "!!!!! AI done" << endl;
            return result;
        });
    }

    ui_state = make_shared<WaitingForPlayerMoveToFinish>();
    cout << "state = State::WAITING_FOR_PLAYER_MOVE_TO_FINISH" << endl;
}

void WaitingForFirstCard::motion(Application *app) {
    Point point = app->getMousePixels();
    int first_card = board.mouse_press(point);
    if (first_card >= 0 && first_card < 1000 && game->exposedTo[0][first_card]) {
        board.setAnnotation(first_card);
    } else {
        board.setNoAnnotation();
    }
}

void WaitingForSecondCard::motion(Application *app) {
    Point point = app->getMousePixels();
    int first_card = board.mouse_press(point);
    if (first_card >= 0 && first_card < 1000 && game->exposedTo[0][first_card]) {
        board.setAnnotation(first_card);
    } else {
        board.setNoAnnotation();
    }
}


static void WaitingForFirstCardReceived(int first_card) {
#if 0
    // Launch some particles XXX!!!
    int source_card = board.hand[0][0];
    int target_card = board.hand[1][0];
//    board.launch(source_card, target_card);
#endif

    cout << "first_card = " << first_card << endl;

    if (first_card == 3000) {
        // Can't discard when no card is selected.
        // Ignore this mouse event.

        board << "Can't discard when no card is selected";
        board.setNewMessage();

        return;
    } if (first_card == 2000) { // PASS
        playerPasses();
    } else if (first_card >= 0) {
        if (first_card < 1000) {
            if (game->location[first_card] != Location::HAND0) {
                board << "You can only select cards in your hand";
                board.setNewMessage();

                return;
            }
            board.highlightCard(first_card);
            cout << "Highlighting card " << first_card << endl;

            board.focus(0, first_card, 0.5);
        }
        cout << "state = State::WAITING_FOR_SECOND_CARD" << endl;
        board << "Select target";
        board.unHighlightAll();
        highlightLegalSecondCard(first_card, game);
        board.setNewMessage();
        ui_state = make_shared<WaitingForSecondCard>(first_card);
    } else {
        cout << "state = State::WAITING_FOR_FIRST_CARD" << endl;
        board << "Select card or pass";
        board.setNewMessage();
        board.unHighlightAll();
        highlightLegalFirstCard(game);
        ui_state = make_shared<WaitingForFirstCard>();
    }
}

void WaitingForFirstCard::mouse(Application *app, int button, int action, int mode) {
    board << "Select card or pass";
    board.setNewMessage();

    cout << "Selecting first card" << endl;
    Point point = app->getMousePixels();
    int first_card = board.mouse_press(point);
    WaitingForFirstCardReceived(first_card);
}

void WaitingForComputerMoveToFinish::idle(Application *) {
    if (future_computer_move.valid()) {
        std::future_status status = future_computer_move.wait_for(std::chrono::milliseconds(10));
        if (status == std::future_status::ready) {
            float value = game->evaluate();
            if (value== -1.0) {
                cout << "You win!" << endl;
                exit(0);
            } else if (value == 1.0) {
                cout << "You lose!" << endl;
                exit(0);
            } else if (value == 0.0) {
                cout << "Draw" << endl;
                exit(0);
            }
            future_computer_move = std::future<void>();
            game->show();

            assert(game->nextPlayer == 0);
            board.unHighlightAll();
            highlightLegalFirstCard(game);
            cout << "state = State::WAITING_FOR_FIRST_CARD" << endl;
            highlightLegalFirstCard(game);
            board << "Select card or pass";
            ui_state = make_shared<WaitingForFirstCard>();
        }
    }
}

void WaitingForComputerEvaluationToFinish::idle(Application *) {
    if (future_best.valid()) {
        std::future_status status = future_best.wait_for(std::chrono::milliseconds(10));
        if (status == std::future_status::ready) {
            Best<SpellCaster> best = future_best.get();
            SpellCaster::Move move = best.move;
            future_best = std::future<Best<SpellCaster>>();

            future_computer_move = std::async(std::launch::async, [move]() {
                game->doMove(move, true);
            });
            cout << "state = State::WAITING_FOR_COMPUTER_MOVE_TO_FINISH" << endl;
            ui_state = make_shared<WaitingForComputerMoveToFinish>();
        }
    }
}

void WaitingForPlayerMoveToFinish::idle(Application *) {
    if (future_player_move.valid()) {
        std::future_status status = future_player_move.wait_for(std::chrono::milliseconds(10));
        if (status == std::future_status::ready) {
            float value = game->evaluate();
            if (value == 1.0) {
                cout << "You win!" << endl;
                exit(0);
            } else if (value == -1.0) {
                cout << "You lose!" << endl;
                exit(0);
            } else if (value == 0.0) {
                cout << "Draw" << endl;
                exit(0);
            }
                
            future_player_move = std::future<void>();
            if (game->nextPlayer == 1) {
                // Computer's turn next
                assert(game->nextPlayer == 1);

                ui_state = make_shared<WaitingForComputerEvaluationToFinish>();
                cout << "state = State::WAITING_FOR_COMPUTER_EVALUATION_TO_FINISH" << endl;
            } else {
                // Player gets next turn too
                board.unHighlightAll();
                highlightLegalFirstCard(game);
                cout << "state = State::WAITING_FOR_FIRST_CARD" << endl;
                highlightLegalFirstCard(game);
                board << "Select card or pass";
                ui_state = make_shared<WaitingForFirstCard>();
            }
        }
    }
}

void WaitingForPlayerMoveToFinish::motion(Application *app) {
    Point point = app->getMousePixels();
    int first_card = board.mouse_press(point);
    if (first_card >= 0 && first_card < 1000 && game->exposedTo[0][first_card]) {
        board.setAnnotation(first_card);
    } else {
        board.setNoAnnotation();
    }
}

void WaitingForComputerMoveToFinish::motion(Application *app) {
    Point point = app->getMousePixels();
    int first_card = board.mouse_press(point);
    if (first_card >= 0 && first_card < 1000 && game->exposedTo[0][first_card]) {
        board.setAnnotation(first_card);
    } else {
        board.setNoAnnotation();
    }
}

void WaitingForComputerEvaluationToFinish::motion(Application *app) {
    Point point = app->getMousePixels();
    int first_card = board.mouse_press(point);
    if (first_card >= 0 && first_card < 1000 && game->exposedTo[0][first_card]) {
        board.setAnnotation(first_card);
    } else {
        board.setNoAnnotation();
    }
}
