#ifndef NUMBER_TERM_H
#define NUMBER_TERM_H

#include "number.h"

class Number_Term;
typedef QSharedPointer<Number_Term> NTerm;

class Number_Variable;

class Number_Term : public Number
{
public:
    virtual bool operator==(const Number_Term & t) = 0;

    virtual NTerm cloneTerm() const = 0;
};

#endif // NUMBER_TERM_H
