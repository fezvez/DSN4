#include "logic_rule.h"

#include <QDebug>

Logic_Rule::Logic_Rule(LRelation h, QList<LRelation> b):
    head(h),
    body(b)
{
    buildName();
    buildFreeVariables();
}

LRule Logic_Rule::clone(){
    QList<LRelation> clonedBody;
    for(LRelation relation : body){
        clonedBody.append(Logic_Relation::clone(relation));
    }
    return LRule(new Logic_Rule(Logic_Relation::clone(head), clonedBody));
}

// TODO
// Check if it simpler to do return freeVariables.isEmpty();
bool Logic_Rule::isGround() const{
    for(LRelation relation : body)
    {
        if(!relation->isGround())
            return false;
    }
    return true;
}

bool Logic_Rule::isBodyEmpty(){
    return body.isEmpty();
}

void Logic_Rule::buildName(){

    name = QString("(<= ") + head->toString();
    switch(body.size()){
    case 0:
        name = name + ")";
        return;
    default:
        break;
    }
    name = name + " " + body[0]->toString();
    for(int i=1; i<body.size(); ++i){
        name = name + " " + body[i]->toString();
    }
    name = name + ")";
}

QString Logic_Rule::rebuildName(){

    name = QString("(<= ") + head->rebuildName();
    switch(body.size()){
    case 0:
        name = name + ")";
        return name;
    default:
        break;
    }
    name = name + " " + body[0]->rebuildName();
    for(int i=1; i<body.size(); ++i){
        name = name + " " + body[i]->rebuildName();
    }
    name = name + ")";

    return name;
}


QSet<QString> Logic_Rule::buildFreeVariables(){
    freeVariables.clear();

    QSet<QString> fVHead = head->getFreeVariables();
    freeVariables += fVHead;

    for(LRelation relation: body){
        QSet<QString> fV = relation->getFreeVariables();
        freeVariables += fV;
    }
    return freeVariables;
}

void Logic_Rule::substitute(LTerm v, LTerm t){
    QString variableName = v->toString();
    if(freeVariables.contains(variableName)){
        //qDebug() << "Logic_Rule::substitute with term " << t->toString() << " and var " << variableName << " in rule " << toString();
        head->substitute(v, t);
        for(LRelation relation : body){
            relation->substitute(v, t);
        }

        buildName();

        // Update freeVariables
        freeVariables.remove(variableName);
        addFreeVariables(t);
    }
}

void Logic_Rule::addFreeVariables(LTerm term){
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

LRelation Logic_Rule::getHead(){
    return head;
}

QList<LRelation> Logic_Rule::getBody(){
    return body;
}

QSet<QString> Logic_Rule::getFreeVariables(){
    return freeVariables;
}




//LRelation Logic_Rule::getFirstUngroundedRelation(){
//    for(LRelation relation : body){
//        bool isGround = relation->isGround();
//        if(isGround){
//            continue;
//        }
//        return relation;
//    }
//    Q_ASSERT(false);
//}




