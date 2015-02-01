#ifndef ROLE_H
#define ROLE_H

#include <QSharedPointer>

#include "../Logic/logic_term.h"

class Role;
typedef QSharedPointer<Role> PRole;
class Role
{
public:
    Role(LTerm r);

    LTerm getRole();

private:
    LTerm role;
};

#endif // ROLE_H
