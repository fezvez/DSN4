#ifndef GDL_TERM_H
#define GDL_TERM_H

#include "gdl.h"

class GDL_Term;
typedef QSharedPointer<GDL_Term> PTerm;

class GDL_Term : public GDL
{
public:
    virtual PTerm cloneTerm() = 0;

    bool isGround() const = 0;
    QString buildNameRecursively() = 0;
    virtual bool operator==(const GDL_Term & t) = 0;
};

#endif // GDL_TERM_H
