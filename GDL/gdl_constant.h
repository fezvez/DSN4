#ifndef GDL_CONSTANT_H
#define GDL_CONSTANT_H

#include "gdl_term.h"

class GDL_Constant;
typedef QSharedPointer<GDL_Constant> PConstant;

class GDL_Constant : public GDL_Term
{
public:
    GDL_Constant(const QString & s);
    GDL_Constant(const GDL_Constant & constant);

    PConstant clone();
    PTerm cloneTerm();

    bool isGround() const;
    QString buildNameRecursively();
    bool operator==(const GDL_Term & t);
};

#endif // GDL_CONSTANT_H
