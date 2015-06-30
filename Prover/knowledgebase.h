#ifndef KnowledgeBase_H
#define KnowledgeBase_H


#include <QList>
#include <QSet>
#include <QMap>
#include <QVector>
#include <QSharedPointer>
#include <QRegExp>

#include "Logic/logic_term.h"
#include "Logic/logic_rule.h"

#include "parser.h"



/**
 * Objectives :
 *  - Check arity identical for each instance of a relation constant / function constant
 *  - Check that the GDL respects stratified negation
 *  - Create GDL specific structures (Set of base propositions, what is the initial state, etc...)
 */
class KnowledgeBase;
typedef QSharedPointer<KnowledgeBase> PKB2;

class Stratum;
typedef QSharedPointer<Stratum> LStratum;


class KnowledgeBase
{

    /**
      * Consructor
      */
public:
    KnowledgeBase();

protected:
    void createRegExp();

    /**
      * Methods related to setup
      */
public:
    void setup(QList<LRelation> relations, QList<LRule> rules);
    QMap<QString, LTerm> getConstantMap();

    QMap<LTerm, QList<LRule>> getConstantToRuleMap();
    QMap<LTerm, QList<LRelation>> getConstantToRelationMap();

protected:
    void storeConstants();
    void buildFullConstantMap();
    void buildEvaluationMap();
    void buildArity();
    void checkArity(LRelation relation);

    // Optional and debug
    void generateStratum();
    void printConstantsWithArity();
    void printFreeVariables();

    // Helper functions to guarantee that everyone uses the same objects
public:
    LRule manageRule(LRule r);
    LRelation manageRelation(LRelation r);
    LTerm manageTerm(LTerm t);
    LTerm manageConstant(LTerm c);
    LTerm manageFunctionConstant(LTerm c);
    LTerm manageRelationConstant(LTerm c);
    LTerm manageObjectConstant(LTerm c);

protected:
    QList<LRelation> relationList;
    QList<LRule> ruleList;

    QMap<QString, LTerm> constantMap;
    QSet<LTerm> objectConstantSet;
    QSet<LTerm> functionConstantSet;
    QSet<LTerm> relationConstantSet;

    QMap<LTerm, QList<LRule>> constantToRuleMap;
    QMap<LTerm, QList<LRelation>> constantToRelationMap;

    QMap<LTerm, int> arity;
    QMap<LTerm, LStratum> stratumMap;
    QList<QList<LTerm>> stratifiedConstants;

public:
    QMap<LTerm, LStratum> getStratumMap();
    QList<QList<LTerm>> getStratifiedConstants();

    /**
      * Methods and data structures related to Prolog-like evaluation
      */
public:

    QList<LRelation> evaluate(LRelation relation);
    void loadTempRelations(const QVector<LRelation> contents);
    void loadAdditionalTempRelations(const QVector<LRelation> contents);

protected:
    QList<LRule> ruleSubstitution(LRule rule);
    LRule buildSkolemRule(LRule originalRule, LRule ruleFromKB);
    bool isRelationValid(LRelation r);

//    QList<LRelation> evaluate2(LRelation r);
//    QList<LRule> ruleSubstitution2(LRule rule);



protected:
    QList<LRelation> evaluationRelations;
    QList<LRule> evaluationRules;
    QList<LRelation> evaluationTempRelations;

    // Utility member variables to access the two previous list members easily
    QMap<LTerm, QList<LRule>> constantToRuleEvaluationMap;
    QMap<LTerm, QList<LRelation>> constantToRelationEvaluationMap;
    QMap<LTerm, QList<LRelation>> constantToTempRelationEvaluationMap;


    static int skolemNumber;

    // Debug
public:
    void printRuleEvaluation();
    void printRelationEvaluation();
    void printTempRelationEvaluation();

    QList<LRelation> getEvaluationRelations();
    QList<LRule> getEvaluationRules();
    QList<LRelation> getEvaluationTempRelations();

    QMap<LTerm, QList<LRelation>> getConstantToTempRelationEvaluationMap();

    /**
      * Misc
      */

protected:


    // Parsing
public:
    LTerm getTermFromString(QString s);
    LRelation getRelationfromString(QString s);

protected:
    Parser parser;
    QStringList split(QString line);
    QRegExp ruleRegExp;
    QRegExp whitespaceRegExp;
    QRegExp leftPar;
    QRegExp rightPar;
    QRegExp inputRegExp;
    QRegExp nextRegExp;



};



class Stratum{
public:
    Stratum(LTerm n);

    void addDependency(LStratum s);
    void addDependencyNegative(LStratum s);

    LTerm getNode();
    int getStrata() const;
    bool updateStrata();
    bool updateStrataStrongly();

    QString toString();

public:
    static bool greaterThan(LStratum s1, LStratum s2){
        return s1->getStrata()<s2->getStrata();
    }

private:
    LTerm node;
    QSet<LStratum> dependency;
    QSet<LStratum> dependencyNegative;

    int strata;
};


#endif // KnowledgeBase_H
