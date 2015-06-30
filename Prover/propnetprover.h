#ifndef PROPNETPROVER_H
#define PROPNETPROVER_H

#include "gdlprover.h"

#include "../PropNet/propositiondatabase.h"

class PropnetProver : public GDLProver
{
public:
    PropnetProver();
    ~PropnetProver();

    void setup(QList<LRelation> relations, QList<LRule> rules);
    void loadKifFile(QString filename);

    void generatePropnet();
    void cleanPropnet();
    void debug();

    bool addProposition(LRelation relation);

    QList<LRule> getGrounding(LRule rule);
    bool addRuleToDatabase(LRule);

    void toFile(QString filename);

protected:
    PDatabase propositionDatabase;
    QSet<QString> alreadyInDatabase;
};

#endif // PROPNETPROVER_H
