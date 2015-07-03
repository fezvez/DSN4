#include "logic_relation.h"

#include <QDebug>

Logic_Relation::Logic_Relation(LTerm h, QList<LTerm> b, Logic::LOGIC_QUALIFIER q, bool n):
    qualifier(q),
    negation(n)
{
    head = Logic_Term::clone(h);
    body = Logic_Term::cloneList(b);
    buildName();
    buildFreeVariables();
}

LRelation Logic_Relation::clone(LRelation relation){
    if(relation->isGround()){
        return relation;
    }
    return LRelation(new Logic_Relation(Logic_Term::clone(relation->getHead()), Logic_Term::cloneList(relation->getBody()), relation->getQualifier(), relation->isNegation()));
}



bool Logic_Relation::isGround() const{
    for(LTerm term : body)
    {
        if(!term->isGround()){
            return false;
        }
    }
    return true;
}

LTerm Logic_Relation::getRelationConstant(){
    return head;
}

LTerm Logic_Relation::getHead(){
    return head;
}

QList<LTerm> Logic_Relation::getBody(){
    return body;
}

Logic::LOGIC_QUALIFIER Logic_Relation::getQualifier(){
    return qualifier;
}

void Logic_Relation::setQualifier(Logic::LOGIC_QUALIFIER q){
    qualifier = q;
    buildName();
}

bool Logic_Relation::isNegation(){
    return negation;
}

void Logic_Relation::setNegation(bool b){
    negation = b;
    buildName();
}

QString Logic_Relation::toStringWithNoQualifier(){
    if(qualifier != Logic::LOGIC_QUALIFIER::NO_QUAL){
    QString answer = name.mid(6, name.size()-7);
    return answer;
    }
    return name;
}

QSet<QString> Logic_Relation::getFreeVariables(){
    return freeVariables;
}

// Inherited
void Logic_Relation::buildName(){
    name = getHead()->toString();
    switch(body.size()){
    case 0:
        break;
    default:
        name = QString('(') + name + " " + body[0]->toString();
        for(int i=1; i<body.size(); ++i){
            name = name + " " + body[i]->toString();
        }
        name = name + ")";
        break;
    }

    switch(qualifier){
    case BASE:
        name = QString("(base ") + name + ")";
        break;
    case TRUE:
        name = QString("(true ") + name + ")";
        break;
    case INIT:
        name = QString("(init ") + name + ")";
        break;
    default:
        break;
    }

    if(negation){
        name = QString("(not ") + name + ")";
    }
}

QString Logic_Relation::rebuildName(){
    name = getHead()->rebuildName();
    switch(body.size()){
    case 0:
        break;
    default:
        name = QString('(') + name + " " + body[0]->rebuildName();
        for(int i=1; i<body.size(); ++i){
            name = name + " " + body[i]->rebuildName();
        }
        name = name + ")";
        break;
    }

    switch(qualifier){
    case BASE:
        name = QString("(base ") + name + ")";
        break;
    case TRUE:
        name = QString("(true ") + name + ")";
        break;
    case INIT:
        name = QString("(init ") + name + ")";
        break;
    default:
        break;
    }

    if(negation){
        name = QString("(not ") + name + ")";
    }

    return name;
}



QSet<QString> Logic_Relation::buildFreeVariables(){
    freeVariables.clear();
    for(LTerm term: body){
        switch(term->getType()){
        case(VARIABLE):
            freeVariables << term->toString();
            break;
        case(FUNCTION):
            freeVariables += term->getFreeVariables();
            break;
        default:
            break;
        }

    }
    return freeVariables;
}

void Logic_Relation::substitute(LTerm v, LTerm t){
    Q_ASSERT(v->getType() == LOGIC_TERM_TYPE::VARIABLE);
    QString variableName = v->toString();
    if(freeVariables.contains(variableName)){
        //qDebug() << "Logic_Relation::substitutewith term " << t->toString() << " and var "  << variableName << " in relation " << toString();
        for(LTerm bodyTerm : body){
            bodyTerm->substitute(v, t);
        }

        // Update name
        buildName();

        // Update free variables
        freeVariables.remove(variableName);
        addFreeVariables(t);
    }
}

void Logic_Relation::addFreeVariables(LTerm term){
    switch(term->getType()){
    case(VARIABLE):
        freeVariables << term->toString();
        break;
    case(FUNCTION):
        freeVariables += term->getFreeVariables();
        break;
    default:
        break;
    }
}



