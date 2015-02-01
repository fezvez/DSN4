#include "unification_problem.h"

#include "parser.h"

#include <QDebug>

Unification_Problem::Unification_Problem(){

}

Unification_Problem::Unification_Problem(LRelation relation1, LRelation relation2){
    QList<LTerm> body1 = relation1->getBody();
    QList<LTerm> body2 = relation2->getBody();

    int size = body1.size();
    for(int i=0; i<size; ++i){
        addEquation(UEquation(new Unification_Equation(body1[i], body2[i])));
    }
    solve();
}



Unification_Problem::Unification_Problem(QList<UEquation> eqs){
    equationsOrigin = QList<UEquation>(eqs);
}

void Unification_Problem::addEquation(UEquation eq){
    equationsOrigin.append(eq);
}

void Unification_Problem::clear(){
    equationsOrigin.clear();
}

bool Unification_Problem::isUnificationValid(){
    return isValid;
}

void Unification_Problem::solve(){
//    qDebug() << "\n    Unification_Problem::Solve";

    equations.clear();
    equationsTemp.clear();
    equationsChecked.clear();

    equations = equationsOrigin;
    solverMessage = QString("Unification succesful");
    isValid = true;

    bool isFinished;
    UEquation swap;



    int cycleNb = 0;
    while(true){

        while(!equations.isEmpty()){

            UEquation equation = equations.back();
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
                    swap = UEquation(new Unification_Equation(term2, term1));
                    equationsTemp.append(swap);
                    break;
                case(FUNCTION):
                    solverMessage = QString("Error constant == function in : ").append(equation->toString());
                    isValid = false;
                    break;
                }
                break;

            case(VARIABLE):
                bool isVariableSubstitutionValid;
                isVariableSubstitutionValid = true;
                switch(type2){
                case(CONSTANT):
                    break;
                case(VARIABLE):
                    break;
                case(FUNCTION):
                    QMap<QString, QList<LTerm> > freeVariables = term2->getFreeVariables();
                    if(freeVariables.contains(term1->toString())){
                        solverMessage = QString("Error variable is already in the function in : ").append(equation->toString());
                        isValid = false;
                    }
                    break;
                }

                if(!isValid){
                    break;
                }
                for(UEquation eq : equations){
                    eq->substitute(term1, term2);
                }
                for(UEquation eq : equationsTemp){
                    eq->substitute(term1, term2);
                }
                for(UEquation eq : equationsChecked){
                    eq->substitute(term1, term2);
                }
                equationsChecked.append(equation);
                break;

            case(FUNCTION):
                switch(type2){
                case(CONSTANT):
                    solverMessage = QString("Error function == constant in : ").append(equation->toString());
                    isValid = false;
                    break;
                case(VARIABLE):
                    swap = UEquation(new Unification_Equation(term2, term1));
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
                        UEquation newEquation = UEquation(new Unification_Equation(body1[i], body2[i]));
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

void Unification_Problem::debug(){
    Parser parser;



    if(false){
        qDebug() << "\n\nCreating terms";

        LTerm a = parser.parseTerm("a");
        LTerm b = parser.parseTerm("b");
        LTerm c = parser.parseTerm("c");
        LTerm x = parser.parseTerm("?x");
        LTerm y = parser.parseTerm("?y");
        LTerm z = parser.parseTerm("?z");
        LTerm fx = parser.parseTerm("(f ?x)");
        LTerm fy = parser.parseTerm("(f ?y)");
        LTerm fz = parser.parseTerm("(f ?z)");
        LTerm gxx = parser.parseTerm("(g ?x ?x)");
        LTerm gab = parser.parseTerm("(g a b)");
        LTerm gxy = parser.parseTerm("(g ?x ?y)");
        LTerm gyz = parser.parseTerm("(g ?y ?z)");
        LTerm hxfx = parser.parseTerm("(h ?x (f ?x))");
        LTerm hxgxy = parser.parseTerm("(h ?x (g ?x ?y))");


        // Substitution check :

        qDebug() << "\n\nSubstitution check";

        qDebug() << "\nSubstituting " << x->toString()
                 << " with " << y->toString()
                 << " in " << fx->toString();
        fx->printDebug();
        fx->substitute(x, y);
        fx->printDebug();

        qDebug() << "\nSubstituting " << x->toString()
                 << " with " << y->toString()
                 << " in " << gxx->toString();
        gxx->printDebug();
        gxx->substitute(x, y);
        gxx->printDebug();

        qDebug() << "\nSubstituting " << x->toString()
                 << " with " << y->toString()
                 << " in " << gxy->toString();
        gxy->printDebug();
        gxy->substitute(x, y);
        gxy->printDebug();

        qDebug() << "\nSubstituting " << x->toString()
                 << " with " << y->toString()
                 << " in " << hxgxy->toString();
        hxgxy->printDebug();
        hxgxy->substitute(x, y);
        hxgxy->printDebug();

        qDebug() << "\nSubstituting " << x->toString()
                 << " with " << z->toString()
                 << " in " << hxgxy->toString();
        hxgxy->printDebug();
        hxgxy->substitute(x, z);
        hxgxy->printDebug();

        qDebug() << "\nSubstituting " << y->toString()
                 << " with " << fz->toString()
                 << " in " << hxgxy->toString();
        hxgxy->printDebug();
        hxgxy->substitute(y, fz);
        hxgxy->printDebug();
    }

    if(true){
        qDebug() << "\n\nCreating terms";

        LTerm a = parser.parseTerm("a");
        LTerm b = parser.parseTerm("b");
        LTerm c = parser.parseTerm("c");
        LTerm x = parser.parseTerm("?x");
        LTerm y = parser.parseTerm("?y");
        LTerm z = parser.parseTerm("?z");
        LTerm fx = parser.parseTerm("(f ?x)");
        LTerm fy = parser.parseTerm("(f ?y)");
        LTerm fz = parser.parseTerm("(f ?z)");
        LTerm gxx = parser.parseTerm("(g ?x ?x)");
        LTerm gab = parser.parseTerm("(g a b)");
        LTerm gxy = parser.parseTerm("(g ?x ?y)");
        LTerm gyz = parser.parseTerm("(g ?y ?z)");
        LTerm hxfx = parser.parseTerm("(h ?x (f ?x))");
        LTerm hxgxy = parser.parseTerm("(h ?x (g ?x ?y))");
        LTerm hfzw = parser.parseTerm("(h (f ?z) ?w)");

        qDebug() << "\n\nRunning solver";
        UEquation eq1;
        UEquation eq2;
        UEquation eq3;

        clear();
        eq1 = UEquation(new Unification_Equation(a, x));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(x, a));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(a, b));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(!isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(a, fx));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(!isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(b, hxgxy));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(!isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(x, fy));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(x, gxy));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(!isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(y, hxgxy));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(!isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(gxx, gab));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(!isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(a, a));
        eq2 = UEquation(new Unification_Equation(a, x));
        addEquation(eq1);
        addEquation(eq2);
        solve();
        printSolverResults();
        Q_ASSERT(isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(x, fy));
        eq2 = UEquation(new Unification_Equation(y, gxx));
        addEquation(eq1);
        addEquation(eq2);
        solve();
        printSolverResults();
        Q_ASSERT(!isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(gxy, gyz));
        addEquation(eq1);
        solve();
        printSolverResults();
        Q_ASSERT(isValid);

        clear();
        eq1 = UEquation(new Unification_Equation(hxgxy, hfzw));
        addEquation(eq1);
        addEquation(eq2);
        solve();
        printSolverResults();
        Q_ASSERT(isValid);
    }
}

void Unification_Problem::printSolverResults(){
    qDebug() << "\n    Solver results";
    if(isValid){
        qDebug() << "        Unification exists";
    }
    else{
        qDebug() << "            Unification fails" << solverMessage;

    }
    qDebug() << "        Print " << equationsOrigin.size() << " original equations";
    for(UEquation e : equationsOrigin){
        qDebug() << "            " << e->toString();
    }
    if(!isValid){
        return;
    }
    qDebug() << "        These equations become ";
    for(UEquation e : equationsOrigin){
        qDebug() <<  "            " << applySubstitution(e)->toString();
    }

    qDebug() << "        Thanks to the " << equationsChecked.size() << " substitutions";
    for(UEquation e : equationsChecked){
        qDebug() <<  "            " << e->toString();
    }
}

void Unification_Problem::applySubstitutionInPlace(LTerm t){
    for(UEquation sub : equationsChecked){
        t->substitute(sub->getTerm1(), sub->getTerm2());
    }
}

void Unification_Problem::applySubstitutionInPlace(UEquation e){
    applySubstitutionInPlace(e->getTerm1());
    applySubstitutionInPlace(e->getTerm2());
}

void Unification_Problem::applySubstitutionInPlace(LRelation r){
    for(LTerm term : r->getBody()){
        applySubstitutionInPlace(term);
    }
    r->setIsNameCorrect(false);
}

void Unification_Problem::applySubstitutionInPlace(LRule r){
    applySubstitutionInPlace(r->getHead());
    for(LRelation relation : r->getBody()){
        applySubstitutionInPlace(relation);
    }
    r->setIsNameCorrect(false);

}

LTerm Unification_Problem::applySubstitution(LTerm t){
    LTerm answer = t->clone();
    applySubstitutionInPlace(answer);
    return answer;
}

UEquation Unification_Problem::applySubstitution(UEquation e){
    UEquation answer = e->clone();
    applySubstitutionInPlace(answer);
    return answer;
}



LRelation Unification_Problem::applySubstitution(LRelation r){
    LRelation answer = r->clone();
    applySubstitutionInPlace(answer);
    return answer;
}

LRule Unification_Problem::applySubstitution(LRule r){
    LRule answer = r->clone();
    applySubstitutionInPlace(answer);
    return answer;
}




