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
    bool cleanPropnetIteration();

    bool addProposition(LRelation relation);

    QList<LRule> getGrounding(LRule rule);
    bool addRuleToDatabase(LRule);

    void buildComponents();
    QList<PComponent> getSubComponents(PComponent component);

    PDatabase getDatabase();

protected:
    PDatabase propositionDatabase;
    QSet<QString> alreadyInDatabase;
    QList<PComponent> components;

    // Query solver
public:
    bool propnetEvaluate(QString s);
    bool propnetEvaluate(PProposition proposition);
    PProposition getPropositionFromString(QString s);
    void loadPropnetBasePropositions(QVector<LRelation> baseProp);
    void loadPropnetDoesPropositions(QVector<LRelation> doesProp);

protected:
    void buildBaseDoesPropositions();
    void buildNextRelationToBaseRelationMap();

    void clearBasePropositions();
    void clearInputPropositions();


protected:
    QMap<QString, PProposition> basePropositions;
    QMap<QString, PProposition> doesPropositions;



    // To file
public:
    void toFile(QString filename);


protected:
    int getIndexDot(PComponent c);
    QMap<PComponent, int> indexDotMap;
    int indexDot;

    // Misc
public:
    void debug();
};

#endif // PROPNETPROVER_H
