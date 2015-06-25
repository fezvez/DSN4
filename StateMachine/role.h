#ifndef ROLE_H
#define ROLE_H

#include <QSharedPointer>

#include "../Logic/logic_term.h"

class Role;
typedef QSharedPointer<Role> PRole;
class Role
{
public:
    // Dangerous, don't use this one, the LTerm will be a dangling pointer
    // Only here because QVector requires a default constructor
    Role();


    Role(LTerm r);
    Role(const Role& r);

    const LTerm getTerm() const;
    QString toString() const;

private:
    LTerm role;
};

#endif // ROLE_H
