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
        clonedBody.append(relation->clone());
    }
    return LRule(new Logic_Rule(head->clone(), clonedBody));
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

    isNameCorrect = true;
}


QSet<QString> Logic_Rule::buildFreeVariables(){
    freeVariables.clear();

    QSet<QString> fVHead = head->buildFreeVariables();
    freeVariables += fVHead;

    for(LRelation relation: body){
        QSet<QString> fV = relation->buildFreeVariables();
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

        // Update freeVariables
        freeVariables.remove(variableName);

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




