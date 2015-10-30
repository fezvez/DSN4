#ifndef PROPNETPROVER_H
#define PROPNETPROVER_H

#include "gdlprover.h"

#include "PropNet/propositiondatabase.h"
#include "PropNet/componentand.h"
#include "PropNet/componentor.h"
#include "PropNet/componentnot.h"
#include "flags.h"


#include <vector>
#include "PropNet/propositionoptimized.h"

class PropnetProver : public GDLProver
{
public:
    PropnetProver();
    ~PropnetProver();

    void setup(QList<LRelation> relations, QList<LRule> rules);
    void setup(QString filename);

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
    QVector<int> depthCharge(QVector<LRelation> baseProp);
    void buildDepthChargeMembers();
    QMap<PProposition, PProposition> getMapNextPropositionToBaseProposition();
    void buildMapNextPropositionToBaseProposition();

protected:
    QVector<int> depthChargeGoalVector;
    QMap<QString, int> roleIndex;
    QMap<QString, QVector<PProposition>> depthChargeLegalMoves;
    QMap<PProposition, PProposition> mapNextPropositionToBaseProposition;


    // Optimized version
public:
    void buildOptimizedPropnet();
    void buildOptimizedMembers();
    void buildPropositionsOptimized();
    void fillComponentAndOptimized(PCAnd p, std::vector<uint32_t> & prop, std::vector<uint32_t> & negProp);

    QVector<int> depthChargeOptimized(QVector<LRelation> baseProp);


    void loadPropnetBasePropositionsOptimized(QVector<LRelation> & baseProps);
    void clearBasePropositionsOptimized();
    void clearAllSavedValuesOptimized();
    void randomizeLegalMoveOptimized();

    bool propnetEvaluateOptimized(QString s);

protected:
    std::vector<PropositionOptimized> propnetOptimized;
    QVector<PProposition> propnetOptimizedIndexReversed;
    QVector<int> depthChargeGoalVectorOptimized;
    QMap<PProposition, uint32_t> propositionIndexOptimized;
    QVector<int> roleIndexOptimized;
    QVector<int> basePropositionsOptimized;
    QVector<int> nextPropositionsOptimized;
    int terminalPropositionIndexOptimized;


    QVector<QVector<int> > legalMovesOptimized;
    QVector<QVector<int> > doesMovesOptimized;
    QVector<int> currentDoesMovesOptimized;
    QVector<QVector<int> >randomRoleOrderOptimized;
    QVector<QVector<int> > goalOptimized;           // For role R, gives you a list of propositions
    QVector<QVector<int> > goalValueOptimized;      // For role R, gives you the associated reward (of each proposition)

    // Misc
public:
    void debugPropnet();
};

#endif // PROPNETPROVER_H
