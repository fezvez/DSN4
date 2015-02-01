#include "gdl_constant.h"

#include <QDebug>

GDL_Constant::GDL_Constant(const QString & s){
    name = s;
}

GDL_Constant::GDL_Constant(const GDL_Constant &constant){
    name = QString(constant.toString());
}

PConstant GDL_Constant::clone(){
    return PConstant(new GDL_Constant(QString(name)));
}

PTerm GDL_Constant::cloneTerm(){
    PConstant constant = clone();
    return constant.staticCast<GDL_Term>();
}

bool GDL_Constant::isGround() const{
    return true;
}

QString GDL_Constant::buildNameRecursively(){
    return name;
}

bool GDL_Constant::operator==(const GDL_Term & t){
    return name==t.toString();
}
