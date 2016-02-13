#ifndef LOGIC_EQUATION_H
#define LOGIC_EQUATION_H

#include "Logic/logic_term.h"


#include <QSharedPointer>


/**
 * Unification_Term
 *
 * Just hold 2 terms
 */

class Unification_Term;
typedef QSharedPointer<Unification_Term> UTerm;

class Unification_Term
{
public:
    Unification_Term(LTerm t1, LTerm t2);

    LTerm getTerm1();
    LTerm getTerm2();

    void rebuildName();

    QString toString();

    UTerm clone();
    void substitute(LTerm var, LTerm term);

private:
    LTerm term1;
    LTerm term2;
};

#endif // LOGIC_EQUATION_H
