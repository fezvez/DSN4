#include "logic_term.h"

#include <QDebug>

Logic_Term::Logic_Term(const QString & s, LOGIC_TERM_TYPE t){
    name = s;
    type = t;
    isNameCorrect = true;
}

Logic_Term::Logic_Term(LTerm h, QList<LTerm> b):
    head(h),
    body(b)
{
    type = LOGIC_TERM_TYPE::FUNCTION;
    buildName();
    setup();

}

void Logic_Term::setup(){
    freeVariables.clear();

    switch(type){
    case(CONSTANT):
        return;
    case(VARIABLE):
        return;
    default:
        break;

    }

    for(LTerm term : body){
        switch(term->getType()){
        case(CONSTANT):
            break;
        case(VARIABLE):
            freeVariables[term->toString()].append(term);
            break;
        case(FUNCTION):
            for(LTerm term : term->getBody()){
                addVariables(term);
            }
        }
    }
}

void Logic_Term::addVariables(LTerm t){
    switch(t->getType()){
    case(VARIABLE):
        freeVariables[t->toString()].append(t);
        break;
    case(FUNCTION):
        for(QString s : t->getFreeVariables().keys()){
            freeVariables[s] += t->getFreeVariables()[s];
        }
        break;
    default:
        break;
    }
}

void Logic_Term::addSingleVariable(LTerm t){
    freeVariables[t->toString()].append(t);
}



bool Logic_Term::operator==(Logic_Term & t){
    return (toString()==t.toString());
}

bool Logic_Term::operator!=(Logic_Term & t){
    return (toString()!=t.toString());
}



LTerm Logic_Term::clone() const{
    QList<LTerm> clonedBody;
    switch(type){
    case(FUNCTION):
        for(LTerm term : body){
            clonedBody.append(term->clone());
        }
        return LTerm(new Logic_Term(head->clone(), clonedBody));
        break;
    default:
        return LTerm(new Logic_Term(name, type));
    }
}

bool Logic_Term::isGround() const{

    switch(type){
    case(LOGIC_TERM_TYPE::CONSTANT):
        return true;
    case(LOGIC_TERM_TYPE::VARIABLE):
        return false;
    case(LOGIC_TERM_TYPE::FUNCTION):
        for(LTerm term : body)
        {
            if(!term->isGround())
                return false;
        }
    }
    return true;
}



LOGIC_TERM_TYPE Logic_Term::getType(){
    return type;
}

LTerm Logic_Term::getHead(){
    return head;
}

QList<LTerm> Logic_Term::getBody(){
    return body;
}


QString Logic_Term::toString(){
    switch(type){
    case(CONSTANT):
    case(VARIABLE):
        return name;
    default:
        break;
    }

    if(!isNameCorrect){
        buildName();
    }
    return name;

}


// Guaranteed that this is a function
void Logic_Term::buildName(){


    name = head->toString();
    if(body.isEmpty()){
        return;
    }
    name = QString('(') + name + " " + body[0]->toString();
    for(int i=1; i<body.size(); ++i){
        name = name + " " + body[i]->toString();
    }
    name = name + ")";

    isNameCorrect = true;
}



void Logic_Term::substitute(LTerm v, LTerm t){
    Q_ASSERT(v->getType() == VARIABLE);

    QString variableName = v->toString();
    //qDebug() << "Logic_Term::substitute with term " << t->toString() << " and var " << variableName << " in term " << toString();

    switch(type){

    case(CONSTANT):
        break;
    case(VARIABLE):
        if(toString() == variableName){
            substitute(t);
        }
        break;
    case(FUNCTION):
        if(freeVariables.contains(variableName)){
            for(LTerm term : freeVariables[variableName]){
                term->substitute(t);
                addVariables(term);
            }
            freeVariables.remove(variableName);

            isNameCorrect = false;
        }
        break;
    }

}

void Logic_Term::substitute(LTerm term){
    Q_ASSERT(type == VARIABLE);
    LTerm  t = term->clone();

    name = t->toString();
    type = t->getType();
    switch(type){
    case(FUNCTION):
        head = t->getHead();
        body = t->getBody();
        freeVariables = t->getFreeVariables();
        break;
    default:
        break;
    }

    isNameCorrect = false;
}


QMap<QString, QList<LTerm> > Logic_Term::getFreeVariables(){
    return freeVariables;
}



void Logic_Term::printDebug(){
    qDebug() << "\nPrint Debug of Term : " << toString();
    switch(type){
    case(FUNCTION):
        qDebug() << "It's a function";
        for(QString var : freeVariables.keys()){
            qDebug() << "\tVariable " << var << " appears " << freeVariables[var].size() << " times";
            for(LTerm t : freeVariables[var]){
                qDebug() << "\t\t" << t->toString() << "\t" << t.data();
            }
        }
    default:
        break;
    }
}

Logic::LOGIC_KEYWORD Logic_Term::getKeyword(){
    return keyword;
}

void Logic_Term::setKeyword(Logic::LOGIC_KEYWORD k){
    keyword = k;
}
