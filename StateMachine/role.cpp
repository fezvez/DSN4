#include "role.h"

#include <QDebug>

Role::Role(){

}

Role::Role(LTerm r)
{
    role = r;
}

Role::Role(const Role& r){
    role = r.getTerm();
}

const LTerm Role::getTerm() const{
    return role;
}

QString Role::toString() const{
    QString answer;
    answer += "Role : ";
    answer += role->toString();
    return answer;
}
