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

    void buildComponents();
    QList<PComponent> getSubComponents(PComponent component);



    bool addProposition(LRelation relation);

    QList<LRule> getGrounding(LRule rule);
    bool addRuleToDatabase(LRule);


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

    void clearBasePropositions();
    void clearDoesPropositions();
    void clearViewPropositions();


protected:
    QMap<QString, PProposition> basePropositions;
    QMap<QString, PProposition> doesPropositions;
    QMap<QString, PProposition> viewPropositions;


    // To file
public:
    void toFile(QString filename);


protected:
    int getIndexDot(PComponent c);
    QMap<PComponent, int> indexDotMap;
    int indexDot;

    // Optimizations
public:
    void initializeSavedValuesMap();
    void clearAllSavedValues();
    void clearViewPropositionSavedValues();



//    QVector<int> depthChargeDebug(QVector<LRelation> baseProp, QMap<PProposition, PProposition> mapNextToBase);
//    int depthCharge(Role role);


protected:
    QSet<QString> basePropositionsAlreadyLoaded;
    QMap<PProposition, bool> hasCorrectSavedValue;


public:
    QVector<int> depthCharge(QVector<LRelation> baseProp, QMap<PProposition, PProposition> mapNextToBase);
    void buildDepthChargeMembers();


protected:
    QVector<int> depthChargeGoalVector;
    QMap<QString, int> roleIndex;
    QMap<QString, QVector<PProposition>> depthChargeLegalMoves;


    // Misc
public:
    void debug();
};

#endif // PROPNETPROVER_H
