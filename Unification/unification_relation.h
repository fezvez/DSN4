#ifndef UNIFICATION_PROBLEM_H
#define UNIFICATION_PROBLEM_H

#include "unification_term.h"

#include <QList>

#include "Logic/logic_rule.h"

/**
 * Unification_Relation
 *
 * Unification_Relation unifies 2 relations
 * It does so by matching terms with each other (Unification_Term)
 * At the end of the unification procedure, you end up with a list
 * of substitutions of the form "X->a" which means that variable X
 * should be substituted with object constant a
 * If every single unification suceeds, then isValid is true
 *
 * You can then take any term/relation/rule and apply the substitutions.
 * For example, if you have this code :
 *
 * Unification_Relation u = Unification_Relation(relation1, relation2)
 * u.solve();
 * if(u.isUnificationValid()){
 *   u.applySubstitutionInPlace(relation1);
 *   u.applySubstitutionInPlace(relation2);
 *   // Here, you are guaranteed that relation1 and relation2 are identical
 * }
 */

/**
 * Typical example of unification between two terms
 * f(a,g(X,Y)) and f(X,Z)
 * where f and g are functional constants
 * You end up with the substitution :
 * X->a
 * Z->g(a,Y)
 */

class Unification_Relation;
typedef QSharedPointer<Unification_Relation> URelation;

class Unification_Relation
{
public:
    Unification_Relation();
    Unification_Relation(LRelation relation1, LRelation relation2);
    Unification_Relation(QList<UTerm> eqs);

    void addEquation(UTerm eq);

    void solve();
    void printSolverResults();
    void clear();

    UTerm applySubstitution(UTerm e);
    LTerm applySubstitution(LTerm t);
    LRelation applySubstitution(LRelation r);
    LRule applySubstitution(LRule r);

    void applySubstitutionInPlace(UTerm e);
    void applySubstitutionInPlace(LTerm t);
    void applySubstitutionInPlace(LRelation r);
    void applySubstitutionInPlace(LRule r);

    // Should only be called after solve() is called
    bool isUnificationValid();
    bool isFirstRelationMoreSpecific();

    QList<UTerm> getSubstitutions();
    QString toString();
    QString getSolverMessage();


private:
    // An equation means analyzing whether two terms can be unified
    QList<UTerm> equationsOrigin;   // The original equations, nothing more
    QList<UTerm> equations;         // The current set of equations being analyzed
    QList<UTerm> equationsTemp;     // While we empty "equations" we fill equationsTemp. When "equations is empty" we do equations = equationsTemp
    QList<UTerm> substitutions;     // When we are done (because we have obtained a substitution Variable -> something) we put it here

    bool isValid;
    QString solverMessage;

};

#endif // UNIFICATION_PROBLEM_H
