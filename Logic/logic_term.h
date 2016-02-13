#ifndef LOGIC_TERM_H
#define LOGIC_TERM_H

#include "logic.h"

#include <QSet>

/**
 * Logic_Term
 *
 * This class represents a Herbrand Logic term
 * Terms are of three kind
 *  - Object constants
 *  - Variables
 *  - Functional terms which are made of a function constant (head) and a vector of terms (body)
 * The type of term is embedded in the LOGIC_TERM_TYPE enum. It was previously done as separate
 * classes which inherited Logic_Term, but for performance reasons, it was siply put as an enum
 * (Virtual table calls can be expensive, especially if accurate branch prediction can be done
 * on the LOGIC_TERM_TYPE enum)
 *
 * This class just holds the data, except for making a single nontrivial computation of building
 * the free variables.
 *
 * This class is (nearly) never used as is, it should always be embedded in a shared pointer
 * This is why we use the typedef LTerm throughout the class except for vey fundamental things
 * like the copy constructor or cloning
 *
 *
 */

class Logic_Term;
typedef QSharedPointer<Logic_Term> LTerm;


enum LOGIC_TERM_TYPE{
    CONSTANT,
    VARIABLE,
    FUNCTION
};

class Logic_Term : public Logic
{
public:
    Logic_Term(const QString & s, LOGIC_TERM_TYPE t);
    Logic_Term(const QString & s, LOGIC_TERM_TYPE t, Logic::LOGIC_KEYWORD k);
    Logic_Term(LTerm head, QList<LTerm> body);
    Logic_Term(const Logic_Term & l);

    static LTerm clone(LTerm term);
    static QList<LTerm> cloneList(QList<LTerm> list);

    bool operator==(Logic_Term & t);
    bool operator!=(Logic_Term & t);

    LOGIC_TERM_TYPE getType();
    LTerm getHead();
    QList<LTerm> getBody();
    Logic::LOGIC_KEYWORD getKeyword();
    void setKeyword(Logic::LOGIC_KEYWORD k);
    QSet<QString> getFreeVariables();

    QString toString();
    bool isGround() const;

    void substitute(LTerm v, LTerm t);

private:
    void buildFreeVariables();
    void addFreeVariables(LTerm term);
    void buildKeyword();
    void buildName();

public:

    QString rebuildName();

    void substitute(LTerm term);            // Only if type == VARIABLE

private:
    LOGIC_TERM_TYPE type;
    Logic::LOGIC_KEYWORD keyword;


    // Only used for functions
    LTerm head;
    QList<LTerm> body;
    QSet<QString> freeVariables;


public:
    void printDebug(int nbTab = 0);

    static int nbTerm;
};

#endif // LOGIC_TERM_H
