#ifndef UCTRAVE_H
#define UCTRAVE_H

#include <set>
#include <map>
#include <vector>
#include <cassert>
#include <iostream>
#include <ctime>
#include <thread>
#include <future>
#include <chrono>
#include <cmath>

using std::set;
using std::map;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::cout;
using std::endl;
using std::exp;

enum McRaveSchedule {
    HAND_SELECTED,
    MINIMUM_MSE
};

class UCTParameters {
public:
    double allowed_time;
    double c;

    McRaveSchedule schedule;
    int k;
    double bias;

    float heuristic_amaf_multiplier;
    float heuristic_mc_multiplier;
    
    bool use_heuristic;
};

const double NON_TERMINAL = 9999.0;

double score(const UCTParameters &params,
             bool optimistic,
             int nparent, int nchild, double qchild, int nidchild, int qidchild);

template<class Game>
class Node {
public:
    map<typename Game::Move, shared_ptr<Node>> children;
    Node *parent;
private:
public:
    int ntot;
    // q_mc's are values for nextPlayer
    // vector<typename Game::Move> moves;
    map<typename Game::Move, int> n_avail;
    map<typename Game::Move, int> n_mc;
    map<typename Game::Move, float> q_mc;
    map<typename Game::Move, int> n_amaf;
    map<typename Game::Move, float> q_amaf;

    ~Node() { }

    typename Game::Move bestChild(const UCTParameters &params, const vector<typename Game::Move> &moves, bool optimistic) {
        float best_score = -1e8;
        typename Game::Move best_id(-1, 0, 0);

        for (auto p : moves) {
            auto id = p;
            float s = score(params, optimistic, n_avail[id],
                             n_mc[id], q_mc[id], n_amaf[id], q_amaf[id]);
            if (s > best_score) {
                best_score = s;
                best_id = id;
            }
        }

        assert(best_score > -1e8);

        return best_id;
    }
    Node(const UCTParameters &params, Node *p)
            : parent(p), ntot(0) { }

    // XXX inefficient
    bool isThereUntried(const vector<typename Game::Move> &moves) {
        for (auto p : moves) {
            if (children.find(p) == children.end()) {
                return true;
            }
        }
        return false;
    }

    // Assumes an untried exists
    typename Game::Move findFirstUntried(
            const vector<typename Game::Move> &moves) {
        for (auto p : moves) {
            if (children.find(p) == children.end()) {
                return p;
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    shared_ptr<Node> expand(const UCTParameters &params,
                              typename Game::Move untriedMove,
                              shared_ptr<Game> position,
                              const vector<typename Game::Move> &moves,
                              vector<typename Game::Move> &ids) {
        ids.push_back(untriedMove);

        position->checkConsistency();
        position->doMove(untriedMove);
        auto child = make_shared<Node>(params, this);

        children[untriedMove] = child;

        return child;
    }

    static shared_ptr<Node>
    treePolicy(const UCTParameters &params,
                              shared_ptr<Game> position,
                              shared_ptr<Node> v,
                              vector<typename Game::Move> &ids) {
        position->checkConsistency();

        float value = position->evaluate();
        auto moves = position->legalMoves();

        if (value == NON_TERMINAL) {
            if (v->isThereUntried(moves)) {
                auto untriedMove = v->findFirstUntried(moves);
                ++v->n_avail[untriedMove]; // ??

                if (params.use_heuristic) {
                    assert(v->n_mc[untriedMove] == 0);
                    auto h = position->heuristicValue(untriedMove);
                    v->n_mc[untriedMove] = params.heuristic_mc_multiplier*h.second;
                    v->q_mc[untriedMove] = params.heuristic_mc_multiplier*h.first;
                }

                position->checkConsistency();
                return v->expand(params, untriedMove, position, moves, ids);
            } else {
                for (auto p : moves) {
                    ++v->n_avail[p];
                }
                auto b = v->bestChild(params, moves, true);
                ids.push_back(b);
                v = v->children[b];
                position->doMove(b);
                position->checkConsistency();
                return treePolicy(params, position, v, ids);
            }
        }
        return v;
    }

    // ids is moves from root of current search
    // When first called, the vertex should have no children.
    // Have to go up one to get to a vertex with children.
    void backupNegamax(float leaf_value,
                       vector<typename Game::Move> &ids, int firstMove) {
        int leaf_player = ids[firstMove-1].getPlayer();
        Node *v = this;
        //typename Game::Move thisChild;
        ++v->ntot;
        // value is value for person moving to v
        // So we store +value in parent
        while (v->parent) {
            --firstMove;
            //thisChild = v->moveThatGotUsHere;
            //typename Game::Move idThatGotUsHere = v->parent->moves[thisChild];
            typename Game::Move idThatGotUsHere = ids[firstMove];
            v = v->parent;

            float value2 = idThatGotUsHere.getPlayer() == leaf_player ? leaf_value : -leaf_value;

            ++v->ntot;
            typename Game::Move thisMove = idThatGotUsHere;
            ++v->n_mc[thisMove];
            v->q_mc[thisMove] += value2;

            set<typename Game::Move> s;
            // XXX assumes simple alternation YYY
            for (int k = firstMove; k < ids.size(); k += 2) {
                s.insert(ids[k]);
            }

            for (auto q : s) {
                ++v->n_amaf[q];
                v->q_amaf[q] += value2;
                assert(q.getPlayer() == thisMove.getPlayer());
            }
        }
        assert(firstMove == 0);
    }
    
#if 1
    void diagnostics(shared_ptr<Game> game, const UCTParameters &params) {
        cout << "Diagnostics:" << endl;
        for (auto p : n_mc) {
            typename Game::Move id = p.first;
            float s = score(params, false, n_avail[id],
                       n_mc[id], q_mc[id], n_amaf[id], q_amaf[id]);
            game->describeId(id);
            cout << " n = " << ntot << ": " << s
                 << " mc: " << q_mc[id] << '/' << n_mc[id]
                 << " amaf: " << q_amaf[id] << '/' << n_amaf[id]
                 << " avail: " << n_avail[id] << endl;
        }
        cout << "End of diagnostics" << endl;
        cout.flush();
    }
#endif
};
//
// position->evaluate() is value for player that got to position
// Not for position->nextPlayer.

// Value of defaultPolicy(positin, ...) is like position->evaluate()
template<class Game>
void defaultPolicy(shared_ptr<Node<Game>> v, int treeMoves,
                   shared_ptr<Game> game, vector<typename Game::Move> &ids,
                   float sign = 1.0, bool gameIsOriginal = true) {

    while (true) {
        float value = game->evaluate();

        // Return if terminal...
        if (value != NON_TERMINAL) {
            v->backupNegamax(sign*value, ids, treeMoves);
            return;
        }

        vector<typename Game::Move> moves = game->legalMoves();
        int n_mc = moves.size();
        if (n_mc == 0) {
            game->show();
            cout << "No legal moves" << endl;
            exit(1);
        }
#if HEURISTIC_ROLLOUT
        vector<double> probs;
        double t = 0.0;
        double lambda = 1.0;
        for (int k = 0; k < n_mc; ++k) {
            double value = game->heuristicValue(moves[k]).first;
            double p = exp(lambda*(value >= 0));
            probs.push_back(p);
            t += p;
        }
        double r = t*(rand()/double(RAND_MAX));
        int i = 0;
        for (i = 0; i < n_mc-1; ++i) {
            r -= probs[i];
            if (r <= 0) {
                break;
            }
        }
#else
        int i = rand() % n_mc;
#endif
        typename Game::Move chosen = moves[i];
        ids.push_back(chosen);
        game->doMove(chosen);
        gameIsOriginal = false;
        sign = -sign;
    }

    //defaultPolicy(v, treeMoves, game, ids, sign, gameIsOriginal);
}

template<class Game>
class Best {
public:
    shared_ptr<Node<Game>> root;
    typename Game::Move move;
};

template<class Game>
Best<Game> uctSearch(const UCTParameters &params,
                     shared_ptr<Game> start, bool verbose = true) {
    using std::chrono::steady_clock;

    assert(start->evaluate() == NON_TERMINAL);

    auto root = make_shared<Node<Game>>(params, (Node<Game> *)NULL);

    auto c_start = steady_clock::now();
    
    cout << "Start thinking..." << endl;
    int count = 0;
    bool did_some_work = false;
    while ((steady_clock::now()-c_start).count()*
           steady_clock::period::num/static_cast<float>(steady_clock::period::den)
           < params.allowed_time) {
        vector<typename Game::Move> ids;

        auto determinised = make_shared<Game>(*start);
        determinised->determinise();
        determinised->checkConsistency();

        auto treePolicyTree = Node<Game>::treePolicy(params, determinised, root, ids);
        int treeMoves = ids.size();
        defaultPolicy(treePolicyTree, treeMoves, determinised, ids);

        // ids[0..treeMoves-1] came from tree policy
        // ids[treeMoves ..] came from default policy
        ++count;
        did_some_work = true;
    }
    cout << "!!!!!!!!!!!!!! Considered " << count << " plays" << endl;
    assert(did_some_work);
    if (verbose) {
        root->diagnostics(start, params);
    }

#if 0 // Check consistency
    for (auto p = root->n_avail.begin(); p != root->n_avail.end(); ++p) {
        assert(p->first.getPlayer() == start->nextPlayer);
    }
    for (auto p = root->q_mc.begin(); p != root->q_mc.end(); ++p) {
        assert(p->first.getPlayer() == start->nextPlayer);
    }
    for (auto p = root->n_mc.begin(); p != root->n_mc.end(); ++p) {
        assert(p->first.getPlayer() == start->nextPlayer);
    }
    for (auto p = root->q_amaf.begin(); p != root->q_amaf.end(); ++p) {
        assert(p->first.getPlayer() == start->nextPlayer);
    }
    for (auto p = root->n_amaf.begin(); p != root->n_amaf.end(); ++p) {
        assert(p->first.getPlayer() == start->nextPlayer);
    }
#endif

    auto moves = start->legalMoves();
    auto bestMove = root->bestChild(params, moves, false);

    return Best<Game>{root, bestMove};
}

template<class Game>
shared_ptr<Node<Game>>
uctSearch0(const UCTParameters &params,
           shared_ptr<Game> start, bool verbose = true) {
    using std::chrono::steady_clock;
    auto root = make_shared<Node<Game>>(params, (Node<Game> *)0, start);

    auto c_start = steady_clock::now();
    
    int count = 0;
    while ((steady_clock::now()-c_start).count()*
           steady_clock::period::num/static_cast<float>(steady_clock::period::den)
           < params.allowed_time) {
        vector<typename Game::Move> ids;
        shared_ptr<Node<Game>> v = Node<Game>::treePolicy(params, root, ids);
        int treeMoves = ids.size();
        defaultPolicy(v, treeMoves, v->position, ids);
        // ids[0..treeMoves-1] came from tree policy
        // is[treeMoves ..] came from default policy
        // v->backupNegamax(value, ids, treeMoves);
        ++count;
    }
    cout << "Considered " << count << " plays" << endl;
    if (false && verbose) {
        root->diagnostics(params);
    }
    return root;
}

template<class X> void sum_vector(vector<X> &a, const vector<X> &b) {
    for (int i = 0; i < a.size(); ++i) {
        a[i] += b[i];
    }
}

template<class X, class Y> void sum_map(map<X, Y> &a, const map<X, Y> &b) {
    for (auto p : b) {
        a[p.first] += p->second;
    }
}

template<class Game>
Best<Game> uctSearch1(const UCTParameters &params, shared_ptr<Game> start, bool verbose = true) {
    auto froot0 = std::async(std::launch::async, [&params, start, verbose]() {
        return uctSearch0(params, start, verbose);
    });
    auto froot1 = std::async(std::launch::async, [&params, start, verbose]() {
        return uctSearch0(params, start, verbose);
    });
    auto froot2 = std::async(std::launch::async, [&params, start, verbose]() {
        return uctSearch0(params, start, verbose);
    });
    auto froot3 = std::async(std::launch::async, [&params, start, verbose]() {
        return uctSearch0(params, start, verbose);
    });
    froot0.wait();
    froot1.wait();
    froot1.wait();
    froot3.wait();

    shared_ptr<Node<Game>> root0 = froot0.get();
    shared_ptr<Node<Game>> root1 = froot1.get();
    shared_ptr<Node<Game>> root2 = froot2.get();
    shared_ptr<Node<Game>> root3 = froot3.get();

    root0->ntot += root1->ntot;
    root0->ntot += root2->ntot;
    root0->ntot += root3->ntot;

    sum_vector(root0->n_mc, root1->n_mc);
    sum_vector(root0->n_mc, root2->n_mc);
    sum_vector(root0->n_mc, root3->n_mc);

    sum_vector(root0->q_mc, root1->q_mc);
    sum_vector(root0->q_mc, root2->q_mc);
    sum_vector(root0->q_mc, root3->q_mc);

    sum_map(root0->n_amaf, root1->n_amaf);
    sum_map(root0->n_amaf, root2->n_amaf);
    sum_map(root0->n_amaf, root3->n_amaf);

    sum_map(root0->q_amaf, root1->q_amaf);
    sum_map(root0->q_amaf, root2->q_amaf);
    sum_map(root0->q_amaf, root3->q_amaf);

    //delete root1;
    int i = root0->bestChild(params, false);
    typename Game::Move bestMove = root0->moves[i];
    return Best<Game>{root0, bestMove};
}

#if 0
template<class Game>
void examineTree(const UCTParameters &params, shared_ptr<Node<Game>> tree0) {
    Node<Game> *root = tree0.get();
    Node<Game> *tree = root;
    string cmd;
    while (true) {
        tree->position->show();
//        tree->diagnostics(params);
        cout << "cmd: ";
        getline(cin, cmd);
#if 0
        if (cmd[0] >= '0' && cmd[0] <= '9') {
            istrstream cmd_line(cmd.c_str());
            int n_mc;
            cmd_line >> n_mc;
            tree = tree->children[n_mc].get();
        } else if (cmd[0] == 'u') {
            if (tree->parent) {
                tree = tree->parent;
            } else {
                cout << "At root" << endl;
            }
        } else
#endif
        if (cmd[0] == 'r') {
            tree = root;
        } else return;
    }
}
#endif

#endif
