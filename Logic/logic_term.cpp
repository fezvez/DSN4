#include "logic_term.h"

#include <QDebug>

 int Logic_Term::nbTerm = 0;

// Note : ths constructor can only be called for Constant and Variable
Logic_Term::Logic_Term(const QString & s, LOGIC_TERM_TYPE t){
//    qDebug() << "Creation term " << nbTerm++ << "\t" << s;
    name = s;
    type = t;
    if(t == LOGIC_TERM_TYPE::CONSTANT){
        buildKeyword();
    }

//        qDebug() << "Logic_Term created " << nbTerm++;
}

Logic_Term::Logic_Term(const QString & s, LOGIC_TERM_TYPE t, Logic::LOGIC_KEYWORD k){
//    qDebug() << "Creation term " << nbTerm++ << "\t" << s;
    name = s;
    type = t;
    keyword = k;

//    qDebug() << "Logic_Term created " << nbTerm++;
}

Logic_Term::Logic_Term(LTerm h, QList<LTerm> b)
{
    type = LOGIC_TERM_TYPE::FUNCTION;
    head = Logic_Term::clone(h);    // Could be head = h because it must be a CONSTANT
    body = Logic_Term::cloneList(b);

    buildName();
    buildFreeVariables();
    keyword = LOGIC_KEYWORD::NO_KEYWORD;

//    qDebug() << "Logic_Term created " << nbTerm++;
}

// Should never happen
// Logic_Term should always be wrapped in a QSharedPointer (typedef LTerm)
Logic_Term::Logic_Term(const Logic_Term & l){
    (void)l;
    Q_ASSERT(false);
}

void Logic_Term::buildKeyword(){
    keyword = Logic::getGDLKeywordFromString(name);
}

LTerm Logic_Term::clone(LTerm term){
    switch(term->getType()){
    case(CONSTANT):
        return term;
    case(VARIABLE):
        return LTerm(new Logic_Term(term->getName(), LOGIC_TERM_TYPE::VARIABLE, LOGIC_KEYWORD::NO_KEYWORD));
    case(FUNCTION):{
        if(term->isGround()){
            return term;
        }
        // Rest assured, the head and body are cloned
        return LTerm(new Logic_Term(term->getHead(), term->getBody()));
        break;
    }
    }
}

QList<LTerm> Logic_Term::cloneList(QList<LTerm> list){
    QList<LTerm> clonedBody;
    for(LTerm term : list){
        clonedBody << Logic_Term::clone(term);
    }
    return clonedBody;
}


bool Logic_Term::operator==(Logic_Term & t){
    return (toString()==t.toString());
}

bool Logic_Term::operator!=(Logic_Term & t){
    return (toString()!=t.toString());
}


// Getters and shit
LOGIC_TERM_TYPE Logic_Term::getType(){
    return type;
}

LTerm Logic_Term::getHead(){
    return head;
}

QList<LTerm> Logic_Term::getBody(){
    return body;
}

QSet<QString> Logic_Term::getFreeVariables(){
    return freeVariables;
}

Logic::LOGIC_KEYWORD Logic_Term::getKeyword(){
    return keyword;
}

void Logic_Term::setKeyword(Logic::LOGIC_KEYWORD k){
    keyword = k;
}



QString Logic_Term::toString(){
    return name;
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

void Logic_Term::buildFreeVariables(){
    freeVariables.clear();
    for(LTerm term : body){
        addFreeVariables(term);
    }
}

void Logic_Term::addFreeVariables(LTerm term){
    switch(term->getType()){
    case(VARIABLE):
        freeVariables <<  term->toString();
        break;
    case(FUNCTION):
        freeVariables += term->getFreeVariables();
        break;
    default:    // case(CONSTANT)
        break;
    }
}







// Guaranteed that this is a function
// Assumes the name of the terms in the head/body are correct
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
}

// Assumes the name of the terms in the head/body are incorrect
QString Logic_Term::rebuildName(){
    //qDebug() << "rebuildName " << name;
    switch(type){
    case(VARIABLE):
    case(CONSTANT):
        return name;
    default:
        break;
    }

    //qDebug() << "function with head " << head->toString();
    //qDebug() << "body size " << body.size();
    name = head->rebuildName();
    if(body.isEmpty()){
        return name;
    }
    name = QString('(') + name + " " + body[0]->rebuildName();
    for(int i=1; i<body.size(); ++i){
        name = name + " " + body[i]->rebuildName();
    }
    name = name + ")";

    //qDebug() << "rebuildName finished" << name;

    return name;
}


// Replace all variables v with term t (which can contain several variables)
void Logic_Term::substitute(LTerm v, LTerm t){
    Q_ASSERT(v->getType() == VARIABLE);

    //qDebug() << "Beginning of Logic_Term::substitute(LTerm v, LTerm t) for term " << name;
    //qDebug() << "printDebug() before of Logic_Term::substitute(LTerm v, LTerm t)";
    //printDebug();

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
            //qDebug() << "This functional term indeed containts the variable " << variableName;
            for(LTerm term : body){
                term->substitute(v,t);
            }
            buildName();
            //buildFreeVariables(); // More efficient to do the operations below
            freeVariables.remove(variableName);
            addFreeVariables(t);

            //qDebug() << "new name is " << toString();
        }
        break;
    }

    //qDebug() << "printDebug() after of Logic_Term::substitute(LTerm v, LTerm t)";
    //printDebug();


}

void Logic_Term::substitute(LTerm term){
    Q_ASSERT(type == VARIABLE);
    //qDebug() << "Sub " << name << " with " << term->getName();
    name = term->getName();
    type = term->getType();
    switch(type){
    case(FUNCTION):
        head = Logic_Term::clone(term->getHead());
        body = Logic_Term::cloneList(term->getBody());
        //buildName();          // Useless, the name is guaranteed to be correct
        buildFreeVariables();
        break;
    default:
        break;
    }

    //qDebug() << "Sub finished";
    //printDebug();
}






void Logic_Term::printDebug(int nbTab){
    QString start = "";
    for(int i=0; i<nbTab; ++i){
        start += "\t";
    }
    qDebug() << start << "Print Debug of Term : " << toString();
    switch(type){
    case(CONSTANT):
        qDebug() << start << "Constant " << toString();
        break;
    case(VARIABLE):
        qDebug() << start << "Variable " << toString();
        break;
    case(FUNCTION):
        qDebug() << start << "Function with head " << head->toString();

        for(LTerm term : body){
            term->printDebug(nbTab+1);
        }

        for(QString var : freeVariables){
            qDebug() << start << "Free Variable " << var << " appears ";
        }
    default:
        break;
    }
}


