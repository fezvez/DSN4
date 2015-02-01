#include "number_variable.h"


Number_Variable::Number_Variable(const QString & s){
    name = s;
}

bool Number_Variable::operator==(const Number_Term & t){
    return false;
}

NVariable Number_Variable::clone() const{
    return NVariable(new Number_Variable(QString(name)));
}

NTerm Number_Variable::cloneTerm() const{
    NVariable variable = clone();
    return variable.staticCast<Number_Term>();
}

bool Number_Variable::isGround() const{
    return false;
}
