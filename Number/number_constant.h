#ifndef NUMBER_CONSTANT_H
#define NUMBER_CONSTANT_H

#include "number_term.h"

class Number_Constant;
typedef QSharedPointer<Number_Constant> NConstant;

class Number_Constant : public Number_Term
{
public:
    Number_Constant(const QString & s);
    Number_Constant(const Number_Constant & constant);
    bool operator==(const Number_Term & t);

    NConstant clone() const;
    NTerm cloneTerm() const;

    bool isGround() const;
};

#endif // NUMBER_CONSTANT_H
