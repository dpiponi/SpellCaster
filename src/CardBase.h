#ifndef CARD_BASE_H
#define CARD_BASE_H

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <map>

using std::ostream;
using std::istream;
using std::vector;
using std::pair;
using std::shared_ptr;
using std::map;
using std::cout;
using std::endl;

#include "CardClass.h"
#include "CardProperty.h"

class SpellCaster;
class Definition;
class Board;

class CardRegistry {
public:
    static map<string, const Definition *> *getRegistry() {
        cout << "Creating card registry" << endl;
        static map<string, const Definition *> *registry = new map<string, const Definition *>;
        return registry;
    }
    static void registerCard(string name, const Definition *definition) {
        auto registry = getRegistry();
        (*registry)[name] = definition;
        cout << "Registering card: " << name << endl;
    }
};

class Definition {
public:
    const char *name;
    const char *icon;
    int positivity;
    int world_cost;
    int astral_cost;
    int basehp;
    int attack;
    CardClass card_class;
    CardClass target_class;
    CardProperty properties;
    CardProperty requirements;
    CardProperty exclusions;
    Definition(
               const char *n,  // Short name
               const char *d,  // 2 char wide icon
               int p,          // positivity
               int wc,         // world_cost
               int ac,         // astral_cost
               int b,          // Base HP
               int c,          // Unused XXX
               int a,          // Attack
               CardClass k,    // Type of card this is: SPELL, MONSTER, ARTIFACT
               CardClass l,    // Type of target: subset of { SPELL, MONSTER, ARTIFACT }
               CardProperty properties0, // Properties: UNDEAD, resistances etc.
               CardProperty requirements0,  // Target mustn't have any of these properties. XXX
               CardProperty exclusions0  // Target mustn't have any of these properties. XXX
               ) :
        name(n), icon(d), positivity(p), world_cost(wc), astral_cost(ac), basehp(b), attack(a), card_class(k), target_class(l),
        properties(properties0),
        requirements(requirements0), exclusions(exclusions0) {
           CardRegistry::registerCard(name, this);
        };
    virtual int key () const { return 0; }
    virtual void describe(const SpellCaster *game, ostream &out, int c) const = 0;
    virtual void execute(SpellCaster *game, int c, bool verbose) const { };// = 0;
    virtual void executeInstant(SpellCaster *game, int c, bool verbose) const  { }
    virtual int computeAttack(SpellCaster *game, int card, int target, bool verbose) const;
    virtual void animate(SpellCaster *game, shared_ptr<Board> board, int card, int target, bool verbose) const;
};


class MonsterDefinition : public Definition {
public:
    template<class ... types>
        MonsterDefinition(types ... args) : Definition(args...) { }
    virtual void describe(const SpellCaster *game, ostream &out, int c) const;
    void execute(SpellCaster *game, int c, bool verbose) const;
    virtual void animate(SpellCaster *game, shared_ptr<Board> board, int card, int target, bool verbose) const;
};

class ImmobilisingMonsterDefinition : public MonsterDefinition {
public:
    template<class ... types>
        ImmobilisingMonsterDefinition(types ... args) : MonsterDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class GhoulBase : public MonsterDefinition {
public:
    template<class ... types>
        GhoulBase(types ... args) : MonsterDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class VampiricMonster: public MonsterDefinition {
public:
    template<class ... types>
        VampiricMonster(types ... args) : MonsterDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class SummonMonster : public MonsterDefinition {
public:
    template<class ... types>
        SummonMonster(types ... args) : MonsterDefinition(args...) { }
    virtual void describe(const SpellCaster *game, ostream &out, int c) const;
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class SpellDefinition : public Definition {
public:
    template<class ... types>
        SpellDefinition(types ... args) : Definition(args...) { }
    virtual void describe(const SpellCaster *game, ostream &out, int c) const;
};

class ArtifactDefinition : public Definition {
public:
    template<class ... types>
        ArtifactDefinition(types ... args) : Definition(args...) { }
    virtual void describe(const SpellCaster *game, ostream &out, int c) const;
};

class PoisonMonster : public MonsterDefinition {
public:
    template<class ... types>
        PoisonMonster(types ... args) : MonsterDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class BlastBase : public SpellDefinition {
public:
    template<class ... types>
        BlastBase(types ... args) : SpellDefinition(args...) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};

class FollowThroughBase : public SpellDefinition {
public:
    template<class ... types>
        FollowThroughBase(types ... args) : SpellDefinition(args...) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};

class DestroyBase : public SpellDefinition {
public:
    template<class ... types>
        DestroyBase(types ... args) : SpellDefinition(args...) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};

class ShieldBase : public SpellDefinition {
public:
    template<class ... types>
        ShieldBase(types ... args) : SpellDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class ReturnBase : public SpellDefinition {
public:
    template<class ... types>
        ReturnBase(types ... args) : SpellDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class TakeBase : public SpellDefinition {
public:
    template<class ... types>
        TakeBase(types ... args) : SpellDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class AntiAstralBase : public SpellDefinition {
public:
    template<class ... types>
        AntiAstralBase(types ... args) : SpellDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class AntiWorldlyBase : public SpellDefinition {
public:
    template<class ... types>
        AntiWorldlyBase(types ... args) : SpellDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class SleepBase : public SpellDefinition {
public:
    template<class ... types>
        SleepBase(types ... args) : SpellDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const override;
    void animate(SpellCaster *game, shared_ptr<Board> board, int card, int target, bool verbose) const override;
};

class MagicWeapon : public ArtifactDefinition {
public:
    template<class ... types>
        MagicWeapon(types ... args) : ArtifactDefinition(args...) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};

class StrengthBase : public SpellDefinition {
public:
    template<class ... types>
        StrengthBase(types ... args) : SpellDefinition(args...) { }
    void execute(SpellCaster *game, int c, bool verbose) const;
};

class DoubleBase : public SpellDefinition {
public:
    template<class ... types>
    DoubleBase(types ... args) : SpellDefinition(args...) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};

class ShardBase : public SpellDefinition {
public:
    template<class ... types>
    ShardBase(types ... args) : SpellDefinition(args...) { }
    virtual void execute(SpellCaster *game, int c, bool verbose) const;
};

class PerpetualMachineBase : public ArtifactDefinition {
public:
    template<class ... types>
    PerpetualMachineBase(types ... args) : ArtifactDefinition(args...) { }
    virtual void execute(SpellCaster *game, int card, bool verbose) const;
};

class ImminentDeathBase : public ArtifactDefinition {
public:
    template<class ... types>
    ImminentDeathBase(types ... args) : ArtifactDefinition(args...) { }
    virtual void execute(SpellCaster *game, int card, bool verbose) const;
};

#endif
