#include "unification_term.h"

#include <QStringBuilder>
#include <QDebug>






Unification_Term::Unification_Term(LTerm t1, LTerm t2)
{
    term1 = Logic_Term::clone(t1);
    term2 = Logic_Term::clone(t2);
}

UTerm Unification_Term::clone(){
    return UTerm(new Unification_Term(Logic_Term::clone(term1), Logic_Term::clone(term2)));
}

LTerm Unification_Term::getTerm1(){
    return term1;
}

LTerm Unification_Term::getTerm2(){
    return term2;
}

void Unification_Term::substitute(LTerm var, LTerm term){
    //qDebug() << "Substitute " << var->toString() << " with " << term->toString() << " in " << toString();
    term1->substitute(var, term);
    term2->substitute(var, term);
    //qDebug() << "It's now " << toString();
}



QString Unification_Term::toString(){
    return term1->getName() + " == " + term2->getName();
}
