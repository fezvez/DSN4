#ifndef UNIFICATION_PROBLEM_H
#define UNIFICATION_PROBLEM_H

#include "unification_equation.h"

#include <QList>

#include "Logic/logic_rule.h"

/**
 * Unification_Problem unifies 2 relations
 * It does so by matching terms with each other (Unification_Equation)
 */

class Unification_Problem;
typedef QSharedPointer<Unification_Problem> UProblem;

class Unification_Problem
{
public:
    Unification_Problem();
    Unification_Problem(LRelation relation1, LRelation relation2);
    Unification_Problem(QList<UEquation> eqs);

    void addEquation(UEquation eq);

    void solve();

    void debug();
    void printSolverResults();
    void clear();

    UEquation applySubstitution(UEquation e);
    LTerm applySubstitution(LTerm t);
    LRelation applySubstitution(LRelation r);
    LRule applySubstitution(LRule r);

    void applySubstitutionInPlace(UEquation e);
    void applySubstitutionInPlace(LTerm t);
    void applySubstitutionInPlace(LRelation r);
    void applySubstitutionInPlace(LRule r);

    // Should only be called after solve() is called
    bool isUnificationValid();


private:
    QList<UEquation> equationsOrigin;   // The original equations, nothing more
    QList<UEquation> equations;         // The current set of equations being analyzed
    QList<UEquation> equationsTemp;     // While we empty "equations" we fill equationsTemp. When "equations is empty" we do equations = equationsTemp
    QList<UEquation> equationsChecked;  // When we are done (because we have obtained a substitution Variable -> something) we put it here

    bool isValid;
    QString solverMessage;

};

#endif // UNIFICATION_PROBLEM_H
