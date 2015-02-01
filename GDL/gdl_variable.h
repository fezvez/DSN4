#ifndef GDL_VARIABLE_H
#define GDL_VARIABLE_H

#include "gdl_term.h"

class GDL_Variable;
typedef QSharedPointer<GDL_Variable> PVariable;

class GDL_Variable : public GDL_Term
{
public:
    GDL_Variable(const QString & s);
    PVariable clone();
    PTerm cloneTerm();

    bool isGround() const;
    QString buildNameRecursively();

    bool operator==(const GDL_Term & t);

    void assignRecursiveName(QString s);

protected:
    QString recursiveName;

};

#endif // GDL_VARIABLE_H
