#ifndef LOGIC_EQUATION_H
#define LOGIC_EQUATION_H

#include "Logic/logic_term.h"


#include <QSharedPointer>


/**
 * Typical example of unification equation
 * f(a,g(X,Y)) = f(X,Z)
 * where f and g are functional constants
 * You end up with the substitution :
 * X->a
 * Z->g(a,Y)
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


//    QList<LTerm> variable1;
//    QList<LTerm> variable2;
};

#endif // LOGIC_EQUATION_H
