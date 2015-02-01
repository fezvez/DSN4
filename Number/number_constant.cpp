#include "number_constant.h"

Number_Constant::Number_Constant(const QString & s){
    name = s;
}

Number_Constant::Number_Constant(const Number_Constant &constant){
    name = QString(constant.toString());
}

bool Number_Constant::operator==(const Number_Term & t){
    return (name==t.toString());
}

NConstant Number_Constant::clone() const{
    return NConstant(new Number_Constant(QString(name)));
}

NTerm Number_Constant::cloneTerm() const{
    NConstant constant = clone();
    return constant.staticCast<Number_Term>();
}

bool Number_Constant::isGround() const{
    return true;
}


