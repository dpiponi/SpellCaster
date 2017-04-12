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

void WaitingForSecondCard::mouse(Application *app, int button, int action, int mode) {
    Point point = app->getMousePixels();
    int second_card = board.mouse_press(point);

    if (second_card >= 0) {
        board.unHighlightAll();
        SpellCaster::Move id = SpellCaster::Move(0, first_card, second_card == 3000 ? DISCARD : second_card);
        if (!game->isLegalId(id, true)) {
            cout << "ILLEGAL!!!!!!!!!!!!!!" << endl;
            cout << "state = State::WAITING_FOR_FIRST_CARD" << endl;
            highlightLegalFirstCard(game);
            ui_state = make_shared<WaitingForFirstCard>();
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
                //UCTParameters params{5.0, 1.0, MINIMUM_MSE, 0, 0.5, 10, 10, true};
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
//    cout << "Motion above: " << first_card << endl;
    if (first_card >= 0 && first_card < 1000 && game->exposedTo[0][first_card]) {
        board.setAnnotation(first_card);
    } else {
        board.setNoAnnotation();
    }
}

void WaitingForSecondCard::motion(Application *app) {
    Point point = app->getMousePixels();
    int first_card = board.mouse_press(point);
//    cout << "Motion above: " << first_card << endl;
    if (first_card >= 0 && first_card < 1000 && game->exposedTo[0][first_card]) {
        board.setAnnotation(first_card);
    } else {
        board.setNoAnnotation();
    }
}

void WaitingForFirstCard::mouse(Application *app, int button, int action, int mode) {
    board << "Select card or pass";
    board.setNewMessage();

    cout << "Selecting first card" << endl;
    Point point = app->getMousePixels();
    int first_card = board.mouse_press(point);
    cout << "first_card = " << first_card << endl;

    if (first_card == 2000) { // PASS
        playerPasses();
    } else if (first_card >= 0) {
        if (first_card < 1000) {
            board.highlightCard(first_card);
            cout << "Highlighting card " << first_card << endl;
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
