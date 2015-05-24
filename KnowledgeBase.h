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





/**
 * Objectives :
 *  - Check arity identical for each instance of a relation constant / function constant
 *  - Check that the GDL respects stratified negation
 *  - Create GDL specific structures (Set of base propositions, what is the initial state, etc...)
 */
class KnowledgeBase;
typedef QSharedPointer<KnowledgeBase> PKB2;

class Stratum2;
typedef QSharedPointer<Stratum2> LStratum;

class Stratum2{
public:
    Stratum2(LTerm n);

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







class KnowledgeBase
{
public:
    KnowledgeBase();


public:
    void setup(QList<LRelation> relations, QList<LRule> rules);


private:
    void createRegExp();
    void storeConstants();
    void buildArity();
    void printConstants();
    void printConstantsWithArity();
    void buildRelationTypesAndQualifiers();
    void buildRulesTypesAndQualifiers();
    void linkBaseToNext();
    void generateStratum();
    void printFreeVariables();
    void buildBaseInitInputDoesPropositions();
    void buildRolePropositions();
    void buildStandardEvaluationStructures();
    void buildBaseInputEvaluationStructures();
    void buildEvaluationMap();


    /**
      * Methods related to Prolog-like evaluation
      */
public:
    void loadTempRelations(const QVector<LRelation> contents);
    QList<LRelation> evaluateRelation(LRelation relation);

protected:
    QList<LRelation> evaluate(LRelation r);
    QList<LRule> ruleSubstitution(LRule rule);
    LRule buildSkolemRule(LRule originalRule, LRule ruleFromKB);
    bool isRelationValid(LRelation r);

private:
    // The only really important thing for evaluation
    // Usually everything except base, init, input and does
    QList<LRelation> evaluationRelations;       // Relations that never changes
    QList<LRule> evaluationRules;               // Rules that never changes
    QList<LRelation> evaluationTempRelations;   // For the current state

    // Utility member variables to access the two previous list members easily
    QMap<LTerm, QList<LRule>> constantToRuleEvaluationMap;
    QMap<LTerm, QList<LRelation>> constantToRelationEvaluationMap;
    QMap<LTerm, QList<LRelation>> constantToTempRelationEvaluationMap;

    static int skolemNumber;



private:











    /**
  * All the knowledge base, indexed by the object constant at the very "start"
  */

public:
    QMap<QString, LTerm> getConstantMap();

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


/**
  * Keywords and shit
  */
public:
    const QMap<QString, LRelation>& getInitPropositions() const;
    const QMap<QString, LRelation>& getRoles() const;

protected:
    QMap<Logic::LOGIC_KEYWORD, QList<LRelation>> mapTypeToRelationContainer;
    QMap<Logic::LOGIC_KEYWORD, QList<LRule>> mapTypeToRuleContainer;

    QMap<Logic::LOGIC_QUALIFIER, QList<LRelation>> mapQualifierToRelationContainer;
    QMap<Logic::LOGIC_QUALIFIER, QList<LRule>> mapQualifierToRuleContainer;

    QList<LRelation> baseRelations;
    QList<LRelation> initRelations;
    QList<LRelation> standardQualRelations; // Everything else

    QList<LRelation> nextRelations;
    QList<LRelation> roleRelations;
    QList<LRelation> goalRelations;
    QList<LRelation> terminalRelations;
    QList<LRelation> legalRelations;
    QList<LRelation> inputRelations;
    QList<LRelation> doesRelations;
    QList<LRelation> standardTypeRelations;     // Everything else

    QList<LRule> baseRules;
    QList<LRule> initRules;
    QList<LRule> standardQualRules;     // Everything else

    QList<LRule> nextRules;
    QList<LRule> roleRules;
    QList<LRule> goalRules;
    QList<LRule> terminalRules;
    QList<LRule> legalRules;
    QList<LRule> inputRules;
    QList<LRule> doesRules;
    QList<LRule> standardTypeRules;     // Everything else


    //
    QMap<LTerm, LTerm> mapNextToBase;
    QMap<LTerm, LTerm> mapBaseToNext;

    QMap<QString, LRelation> basePropositions;
    QMap<QString, LRelation> initPropositions;
    QMap<QString, LRelation> inputPropositions; // either this line
    QMap<QString, LRelation> doesPropositions;  // or that one will disappear

    QMap<QString, LRelation> rolePropositions;

    QMap<QString, LTerm> baseConstants;



    /**
      * Misc
      */

public:
    void debug();

private:
    void checkArity(LRelation relation);
    QMap<LTerm, int> arity;

private:
    QMap<LTerm, LStratum> stratumMap;
    QList<QList<LTerm>> stratifiedConstants;


private:
    QStringList split(QString line);
    QRegExp ruleRegExp;
    QRegExp whitespaceRegExp;
    QRegExp leftPar;
    QRegExp rightPar;
    QRegExp inputRegExp;
    QRegExp nextRegExp;



};


#endif // KnowledgeBase_H
