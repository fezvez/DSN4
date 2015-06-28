#ifndef PROPNETPROVER_H
#define PROPNETPROVER_H

#include "gdlprover.h"

class PropnetProver : public GDLProver
{
public:
    PropnetProver();
    ~PropnetProver();

    void setup(QList<LRelation> relations, QList<LRule> rules);
    void loadKifFile(QString filename);

    void generatePropnet();
};

#endif // PROPNETPROVER_H
