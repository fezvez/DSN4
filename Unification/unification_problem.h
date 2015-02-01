#ifndef UNIFICATION_PROBLEM_H
#define UNIFICATION_PROBLEM_H

#include "unification_equation.h"

#include <QList>

#include "Logic/logic_rule.h"

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

    bool isUnificationValid();


private:
    QList<UEquation> equationsOrigin;
    QList<UEquation> equations;
    QList<UEquation> equationsTemp;
    QList<UEquation> equationsChecked;

    bool isValid;
    QString solverMessage;

};

#endif // UNIFICATION_PROBLEM_H
