#include "unification_equation.h"

#include <QStringBuilder>
#include <QDebug>






Unification_Equation::Unification_Equation(LTerm t1, LTerm t2)
{
    term1 = t1->clone();
    term2 = t2->clone();
}

UEquation Unification_Equation::clone(){
    return UEquation(new Unification_Equation(term1->clone(), term2->clone()));
}

LTerm Unification_Equation::getTerm1(){
    return term1;
}

LTerm Unification_Equation::getTerm2(){
    return term2;
}

void Unification_Equation::substitute(LTerm var, LTerm term){
    //qDebug() << "Substitute " << var->toString() << " with " << term->toString() << " in " << toString();
    term1->substitute(var, term);
    term2->substitute(var, term);
    //qDebug() << "It's now " << toString();
}



QString Unification_Equation::toString(){
    return term1->toString() + " == " + term2->toString();
}
