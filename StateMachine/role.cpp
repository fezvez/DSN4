#include "role.h"

Role::Role(){

}

Role::Role(LTerm r)
{
    role = r;
}

Role::Role(const Role& r){
    role = r.getRole();
}

const LTerm Role::getRole() const{
    return role;
}

QString Role::toString() const{
    QString answer;
    answer += "Role : ";
    answer += role->toString();
    return answer;
}
