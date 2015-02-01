#include "gdl_variable.h"

GDL_Variable::GDL_Variable(const QString & s){
    name = s;
    recursiveName = name;
}

PVariable GDL_Variable::clone(){
    return PVariable(new GDL_Variable(QString(name)));
}

PTerm GDL_Variable::cloneTerm(){
    PVariable variable = clone();
    return variable.staticCast<GDL_Term>();
}

bool GDL_Variable::isGround() const{
    return false;
}

QString GDL_Variable::buildNameRecursively(){
    return recursiveName;
}

bool GDL_Variable::operator==(const GDL_Term & t){
    return false;
}

void GDL_Variable::assignRecursiveName(QString s){
    recursiveName = s;
}
