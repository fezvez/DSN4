#include "logic.h"

QString Logic::toString(){
    if(isNameCorrect){
        return name;
    }
    buildName();
    return name;
}

Logic::~Logic(){

}

void Logic::buildName(){

}

void Logic::setIsNameCorrect(bool b){
    isNameCorrect = b;
}

QMap<QString, Logic::LOGIC_KEYWORD> Logic::mapString2GDLType = QMap<QString, Logic::LOGIC_KEYWORD>();
QMap<Logic::LOGIC_KEYWORD, QString> Logic::mapGDLType2String = QMap<Logic::LOGIC_KEYWORD, QString>();

QMap<QString, Logic::LOGIC_QUALIFIER> Logic::mapString2GDLQualifier = QMap<QString, Logic::LOGIC_QUALIFIER>();
QMap<Logic::LOGIC_QUALIFIER, QString> Logic::mapGDLQualifier2String = QMap<Logic::LOGIC_QUALIFIER, QString>();
