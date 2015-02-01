#include "number.h"

QString Number::toString() const{
    return name;
}

Number::~Number(){

}

QMap<QString, Number::NUMBER_TYPE> Number::mapString2GDLType = QMap<QString, Number::NUMBER_TYPE>();
QMap<Number::NUMBER_TYPE, QString> Number::mapGDLType2String = QMap<Number::NUMBER_TYPE, QString>();

QMap<QString, Number::NUMBER_QUALIFIER> Number::mapString2GDLQualifier = QMap<QString, Number::NUMBER_QUALIFIER>();
QMap<Number::NUMBER_QUALIFIER, QString> Number::mapGDLQualifier2String = QMap<Number::NUMBER_QUALIFIER, QString>();
