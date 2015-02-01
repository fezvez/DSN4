#ifndef NUMBER_FUNCTION_H
#define NUMBER_FUNCTION_H

#include "number_term.h"
#include "number_constant.h"

class Number_Function;
typedef QSharedPointer<Number_Function> NFunction;

class Number_Function : public Number_Term
{
public:
    Number_Function(NConstant h, QList<NTerm> b);
    bool operator==(const Number_Term & t);

    NFunction clone() const;
    NTerm cloneTerm() const;

public:
    NConstant getHead();
    QList<NTerm> getBody();
    bool isGround() const;

private:
    void buildName();

private:
    NConstant head;
    QList<NTerm> body;
};

#endif // NUMBER_FUNCTION_H
