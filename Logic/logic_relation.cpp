#include "logic_relation.h"

#include <QDebug>

Logic_Relation::Logic_Relation(LTerm h, QList<LTerm> b, Logic::LOGIC_QUALIFIER q, bool n):
    head(h),
    body(b),
    qualifier(q),
    negation(n)
{
    buildName();
    buildFreeVariables();
}

LRelation Logic_Relation::clone(){
    QList<LTerm> clonedBody;
    for(LTerm term : body){
        clonedBody.append(term->clone());
    }
    return LRelation(new Logic_Relation(head->clone(), clonedBody, qualifier, negation));
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

    isNameCorrect = true;
}



QSet<QString> Logic_Relation::buildFreeVariables(){
    freeVariables.clear();
    for(LTerm term: body){
        switch(term->getType()){
        case(VARIABLE):
            freeVariables << term->toString();
            break;
        case(FUNCTION):
            for(QString key : term->getFreeVariables().keys()){
                freeVariables << key;
            }
            break;
        default:
            break;
        }

    }
    return freeVariables;
}

void Logic_Relation::substitute(LTerm v, LTerm t){
    QString variableName = v->toString();
    if(freeVariables.contains(variableName)){
        //qDebug() << "Logic_Relation::substitutewith term " << t->toString() << " and var "  << variableName << " in relation " << toString();
        for(LTerm bodyTerm : body){
            bodyTerm->substitute(v, t);
        }

        // Update free variables
        switch(t->getType()){
        case(VARIABLE):
            freeVariables << t->toString();
            break;
        case(FUNCTION):
            for(QString s : t->getFreeVariables().keys()){
                freeVariables << s; // Insert s if it isn't there already
            }
            break;
        default:
            break;
        }

        isNameCorrect = false;
    }
}


LTerm Logic_Relation::getRelationConstant(){
    return head;
}

LTerm Logic_Relation::getHead(){
    return getRelationConstant();
}

QList<LTerm> Logic_Relation::getBody(){
    return body;
}

Logic::LOGIC_QUALIFIER Logic_Relation::getQualifier(){
    return qualifier;
}

bool Logic_Relation::isNegation(){
    return negation;
}
