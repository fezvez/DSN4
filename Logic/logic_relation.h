#ifndef LOGIC_RELATION_H
#define LOGIC_RELATION_H

/**
 * Logic_Relation
 *
 * This class represents a Herbrand Logic relation. Such relation is made of a relation constant
 * (head) and a vector of terms (body). Note the syntactic similarity with a functional term
 *
 * This class just holds two extra bit of information
 *  - Is the relation a negation or not?
 *  - Does the relation have a LOGIC_QUALIFIER "base", "init" or "true"
 *
 * Negation : "not" can either be considered a relation constant of arity 1 like any other
 * or a very special keyword. The former is more elegant, but the latter is more performant
 * Why more performant? The first reason is that every time you want to evaluate a relation,
 * you need to check whether it is a negation because negations and non-negations are evaluated
 * differently. Lookup is slower than just doing a "if". The second reason is that identifying
 * and simplifying double negations becomes trivial
 *

 *
 * This class is (nearly) never used as is, it should always be embedded in a shared pointer
 * This is why we use the typedef LRelation throughout the class except for vey fundamental things
 * like the copy constructor or cloning
 *
 *
 */


/**
 *
 *Ramblings about LOGIC_QUALIFIER
 *
 * LOGIC_QUALIFIER : GDL uses three keywords "base", "init" and "true" which are beyond
 * Hebrand Logic. Think about it, in Tic-Tac-Toe, you can read these things
 *  - (init (control xplayer))
 *  - (<= (next (control oplayer)) (true (control xplayer)) )
 *
 * "(control xplayer)" is a boolean, which means that "control" is a relation constant (and
 * not a function constant). Which means that init is something that takes in parameter a
 * boolean. There is no such thing is GDL. "base", "init" and "true" are what I (Bertrand Decoster) call a qualifier.
 * They are useful (and necessary) to play an actual game, but they fall beyond Hebrand Logic.
 *
 * If you are curious, all qualifier are only about base propositions, which means that if you see
 * (base X) of (true Y), it means that X and Y are base propositions. Base propositions are crucial
 * to GDL because they are the components of the game state. To identify a game state uniquely,
 * you need the value of every single base proposition, and nothing else.
 *
 * You can know which is the set of base propositions by computing all the
 * rules/relations with the keyword "base" in it. This is typically done at the very beginning,
 * and in the remainder of the gameplay, you turn off these rules/relations (you remove them
 * from your knowledge base)
 *
 * "init" means that the base proposition is true in the initial state of the game. This is
 * typically computed once at the very beginning, and then you do the same thing, you remove
 * the init rules/relations from your knowledge base
 *
 * "true" is somewhat useless. The fundamental reason it is here (I guess) is because the GDL
 * people wanted the base propositions to always always have a qualifier. If you are alert enough,
 * you may have noted that I forgot one qualifier : "next". Next is so special, annoying and
 * performance-averse that I decided to get rid of it alltogether and change the rule
 * (<= (next (control oplayer)) (true (control xplayer)) )
 * into the rule
 * (<= (next_control oplayer) (true (control xplayer)) )
 * So, I generate a relation constant from thin air because every time I want to evaluate something
 * with my Herbrand Prover, I don't have to check whether it's a next or not
 *
 * So, a base proposition always has a qualifier attached to it, that's the important thing to
 * remember
 *
 */

#include "logic_term.h"

class Logic_Relation;
typedef QSharedPointer<Logic_Relation> LRelation;

class Logic_Relation : public Logic
{
public:
    Logic_Relation(LTerm h, QList<LTerm> b, Logic::LOGIC_QUALIFIER q = NO_QUAL, bool n = false);

    static LRelation clone(LRelation relation);

    bool isGround() const;

    LTerm getRelationConstant();
    LTerm getHead();
    QList<LTerm> getBody();
    Logic::LOGIC_QUALIFIER getQualifier();
    void setQualifier(Logic::LOGIC_QUALIFIER q); // Exclusively used to remove the init qualifier
    bool isNegation();
    void setNegation(bool b);
    QSet<QString> getFreeVariables();



    void substitute(LTerm v, LTerm t);

    QString toStringWithNoQualifier();
    QString rebuildName();


private:
    void buildName();
    void buildNameWithNoQualifier();
    QSet<QString> buildFreeVariables();
    void addFreeVariables(LTerm term);



protected:
    LTerm head;
    QList<LTerm> body;
    Logic::LOGIC_QUALIFIER qualifier;   // base, init, true or no_qualifier
    bool negation;
    QString nameWithNoQualifier;

    QSet<QString> freeVariables;
};

#endif // LOGIC_RELATION_H
