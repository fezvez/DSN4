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
  * Knowledge Base
  * Goal is to do pure Herbrand semantics
  * Nothing GDL related
  * Relies on the fact that base, init and true are abstracted away in the Parser
  * (i.e : they are representend by a LOGIC_QUALIFIER in a Logic_Relation)
  */

/**
 * Objectives :
 *  - Check arity identical for each instance of a relation constant / function constant
 *  - Check that the GDL respects stratified negation
 *  - Create GDL specific structures (Set of base propositions, what is the initial state, etc...)
 */

/**
 * Forward declaration
 */
class Stratum;
typedef QSharedPointer<Stratum> LStratum;


class KnowledgeBase
{
    /**
      * Consructor
      */
public:
    KnowledgeBase();

    /**
      * Methods related to setup
      */
public:
    void setup(QString filename);
    void setup(QList<LRelation> relations, QList<LRule> rules);

protected:
    void storeConstants();
    void buildFullConstantMap();
    void buildEvaluationMap();



    /**
      * Accessing all the raw data
      */
public:
    QMap<QString, LTerm> getConstantMap();
    QMap<LTerm, QList<LRule>> getConstantToRuleMap();
    QMap<LTerm, QList<LRelation>> getConstantToRelationMap();


protected:
    QList<LRelation> relationList;
    QList<LRule> ruleList;

    QMap<QString, LTerm> constantMap;
    QSet<LTerm> objectConstantSet;
    QSet<LTerm> functionConstantSet;
    QSet<LTerm> relationConstantSet;

    QMap<LTerm, QList<LRule>> constantToRuleMap;
    QMap<LTerm, QList<LRelation>> constantToRelationMap;


    /**
      * Static analysis
      * Entirely optional
      * Note that Stratum cannot be computed if the arity are not computed beforehand (I think)
      */
public:
    QMap<LTerm, int> getArity();
    QMap<LTerm, LStratum> getStratumMap();
    QList<QList<LTerm>> getStratifiedConstants();

protected:
    void buildArity();
    void checkArity(LRelation relation);
    void printConstantsWithArity();
    void generateStratum();
    void printFreeVariables();

protected:
    QMap<LTerm, int> arity;
    QMap<LTerm, LStratum> stratumMap;
    QList<QList<LTerm>> stratifiedConstants;




    /**
      * Methods and data structures related to Prolog-like evaluation
      */

    // Helper functions to guarantee that everyone uses the same objects
    // Necessary to call it before calling evaluate()
public:
    LRule manageRule(LRule r);
    LRelation manageRelation(LRelation r);
    LTerm manageTerm(LTerm t);
    LTerm manageConstant(LTerm c);
    LTerm manageFunctionConstant(LTerm c);
    LTerm manageRelationConstant(LTerm c);
    LTerm manageObjectConstant(LTerm c);

public:
    QList<LRelation> evaluate(LRelation relation);                          // The single most important function of the whole class


protected:
    QList<LRule> ruleSubstitution(LRule rule);
    LRule buildSkolemRule(LRule originalRule, LRule ruleFromKB);
    bool isRelationValid(LRelation r);

    static int skolemNumber;

public:
    QList<LRelation> getEvaluationRelations();
    QList<LRule> getEvaluationRules();
    QList<LRelation> getEvaluationTempRelations();

    QMap<LTerm, QList<LRelation>> getConstantToTempRelationEvaluationMap();

    void loadTempRelations(const QVector<LRelation> contents);              // Ease of use function to load the current state (base propositions)
    void loadAdditionalTempRelations(const QVector<LRelation> contents);    // Ease of use function to load the moves (does propositions)
    // Note that in general Prolog-like evaluation, you can do whatever you want with those
    // loadTempRelations clears evaluationTempRelations before loading new relations
    // loadAdditionalTempRelations just add stuff to evaluationTempRelations

protected:
    QList<LRelation> evaluationRelations;
    QList<LRule> evaluationRules;
    QList<LRelation> evaluationTempRelations;   // If you want to add relations on the fly (and delete them easily) Typically for base and does

    // Utility member variables to access the two previous list members easily
    QMap<LTerm, QList<LRule>> constantToRuleEvaluationMap;
    QMap<LTerm, QList<LRelation>> constantToRelationEvaluationMap;
    QMap<LTerm, QList<LRelation>> constantToTempRelationEvaluationMap;



    // Debug
public:
    void printRuleEvaluation();
    void printRelationEvaluation();
    void printTempRelationEvaluation();





    /**
      * Parsing
      */
public:
    LTerm getTermFromString(QString s);         // Ease of use function
    LRelation getRelationfromString(QString s);

protected:
    QStringList split(QString line);



public:
    static QRegExp ruleRegExp;
    static QRegExp whitespaceRegExp;
    static QRegExp wordRegExp;
    static QRegExp leftPar;
    static QRegExp rightPar;
    static QRegExp inputRegExp;
    static QRegExp nextRegExp;
    static QRegExp newlineRegExp;

    Parser parser;
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
