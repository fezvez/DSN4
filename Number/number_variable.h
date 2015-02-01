#ifndef NUMBER_VARIABLE_H
#define NUMBER_VARIABLE_H

#include "number_term.h"

class Number_Variable;
typedef QSharedPointer<Number_Variable> NVariable;

class Number_Variable : public Number_Term
{
public:
    Number_Variable(const QString & s);
    bool operator==(const Number_Term & t);

    NVariable clone() const;
    NTerm cloneTerm() const;

    bool isGround() const;
};

#endif // NUMBER_VARIABLE_H
