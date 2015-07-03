#include "unification_relation.h"

#include "parser.h"

#include <QDebug>

Unification_Relation::Unification_Relation(){

}

Unification_Relation::Unification_Relation(LRelation relation1, LRelation relation2){
    addEquation(UTerm(new Unification_Term(relation1->getHead(), relation2->getHead())));

    QList<LTerm> body1 = relation1->getBody();
    QList<LTerm> body2 = relation2->getBody();

    int size = body1.size();
    for(int i=0; i<size; ++i){
        addEquation(UTerm(new Unification_Term(body1[i], body2[i])));
    }
    solve();
}

Unification_Relation::Unification_Relation(QList<UTerm> eqs){
    equationsOrigin = QList<UTerm>(eqs);
    solve();
}



void Unification_Relation::addEquation(UTerm eq){
    equationsOrigin.append(eq);
}

void Unification_Relation::clear(){
    equationsOrigin.clear();
}

bool Unification_Relation::isUnificationValid(){
    return isValid;
}

void Unification_Relation::solve(){
//    qDebug() << "\n    Unification_Problem::Solve";

    equations.clear();
    equationsTemp.clear();
    substitutions.clear();

    equations = equationsOrigin;
    solverMessage = QString("Unification succesful");
    isValid = true;

    bool isFinished;
    UTerm swap;


    int cycleNb = 0;
    while(true){
        while(!equations.isEmpty()){

            UTerm equation = equations.back();
            equations.pop_back();
//            qDebug() << "        Equation " << equation->toString();

            LTerm term1 = equation->getTerm1();
            LTerm term2 = equation->getTerm2();

            LOGIC_TERM_TYPE type1 = term1->getType();
            LOGIC_TERM_TYPE type2 = term2->getType();

            switch(type1){
            case(CONSTANT):
                switch(type2){
                case(CONSTANT):
                    if(term1->toString() != term2->toString()){
                        solverMessage = QString("Error constant == constant in : ").append(equation->toString());
                        isValid = false;
                    }
                    break;
                case(VARIABLE):
                    swap = UTerm(new Unification_Term(term2, term1));
                    equationsTemp.append(swap);
                    break;
                case(FUNCTION):
                    solverMessage = QString("Error constant == function in : ").append(equation->toString());
                    isValid = false;
                    break;
                }
                break;

            case(VARIABLE):
//                bool isVariableSubstitutionValid;
//                isVariableSubstitutionValid = true;
                switch(type2){
                case(CONSTANT):
                    break;
                case(VARIABLE):
                    break;
                case(FUNCTION):
                    QSet<QString> freeVariables = term2->getFreeVariables();
                    if(freeVariables.contains(term1->toString())){
                        solverMessage = QString("Error variable is already in the function in : ").append(equation->toString());
                        isValid = false;
                    }
                    break;
                }

                if(!isValid){
                    break;
                }
                for(UTerm eq : equations){
                    eq->substitute(term1, term2);
                }
                for(UTerm eq : equationsTemp){
                    eq->substitute(term1, term2);
                }
                for(UTerm eq : substitutions){
                    eq->substitute(term1, term2);
                }
                substitutions.append(equation);
                break;

            case(FUNCTION):
                switch(type2){
                case(CONSTANT):
                    solverMessage = QString("Error function == constant in : ").append(equation->toString());
                    isValid = false;
                    break;
                case(VARIABLE):
                    swap = UTerm(new Unification_Term(term2, term1));
                    equationsTemp.append(swap);
                    break;
                case(FUNCTION):
                    if(term1->getHead()->toString() != term2->getHead()->toString()){
                        solverMessage = QString("Error function name disagree in : ").append(equation->toString());
                        isValid = false;
                        break;
                    }
                    int size = term1->getBody().size();
                    QList<LTerm> body1 = term1->getBody();
                    QList<LTerm> body2 = term2->getBody();
                    for(int i=0; i<size; ++i){
                        UTerm newEquation = UTerm(new Unification_Term(body1[i], body2[i]));
                        equationsTemp.append(newEquation);
                    }
                    break;
                }
                break;
            }
        }

        if(!isValid){
            break;
        }
        isFinished = equationsTemp.isEmpty();
        if(isFinished)
        {
            break;
        }

        equations = equationsTemp;
        equationsTemp.clear();
        cycleNb++;
    }
}


void Unification_Relation::printSolverResults(){
    qDebug() << "\n    Solver results";
    if(isValid){
        qDebug() << "        Unification exists";
    }
    else{
        qDebug() << "            Unification fails" << solverMessage;

    }
    qDebug() << "        Print " << equationsOrigin.size() << " original equations";
    for(UTerm e : equationsOrigin){
        qDebug() << "            " << e->toString();
    }
    if(!isValid){
        return;
    }
    qDebug() << "        These equations become ";
    for(UTerm e : equationsOrigin){
        qDebug() <<  "            " << applySubstitution(e)->toString();
    }

    qDebug() << "        Thanks to the " << substitutions.size() << " substitutions";
    for(UTerm e : substitutions){
        qDebug() <<  "            " << e->toString();
    }
}

void Unification_Relation::applySubstitutionInPlace(LTerm t){
    for(UTerm sub : substitutions){
        t->substitute(sub->getTerm1(), sub->getTerm2());
//        t->buildName();
    }
}

void Unification_Relation::applySubstitutionInPlace(UTerm e){
    applySubstitutionInPlace(e->getTerm1());
    applySubstitutionInPlace(e->getTerm2());
}

void Unification_Relation::applySubstitutionInPlace(LRelation r){
    for(LTerm term : r->getBody()){
        applySubstitutionInPlace(term);
    }
//        r->buildName();
}

void Unification_Relation::applySubstitutionInPlace(LRule r){
    applySubstitutionInPlace(r->getHead());
    for(LRelation relation : r->getBody()){
        applySubstitutionInPlace(relation);
    }
//    r->buildName();

}


LTerm Unification_Relation::applySubstitution(LTerm t){
    LTerm answer = Logic_Term::clone(t);
    applySubstitutionInPlace(answer);
    return answer;
}

UTerm Unification_Relation::applySubstitution(UTerm e){
    UTerm answer = e->clone();
    applySubstitutionInPlace(answer);
    return answer;
}

LRelation Unification_Relation::applySubstitution(LRelation r){
    LRelation answer = Logic_Relation::clone(r);
    applySubstitutionInPlace(answer);
    return answer;
}

LRule Unification_Relation::applySubstitution(LRule r){
    LRule answer = r->clone();
    applySubstitutionInPlace(answer);
    return answer;
}

QList<UTerm> Unification_Relation::getSubstitutions(){
    return substitutions;
}

QString Unification_Relation::toString(){
    QString answer;
    answer += QString("Unification_Relation validity is ");
    if(isValid){
        answer += QString("true\n");
        for(UTerm term : substitutions){
            answer += "\t" + term->toString() + "\n";
        }
    }
    else{
        answer += QString("false\n");
    }
    return answer;
}

QString Unification_Relation::getSolverMessage(){
    return solverMessage;
}
