#ifndef GDLPROVER_H
#define GDLPROVER_H

#include "knowledgebase.h"


class GDLProver : public KnowledgeBase
{
public:
    GDLProver();
    ~GDLProver();

public:
    void setup(QString filename);
    void setup(QList<LRelation> relations, QList<LRule> rules);


protected:
    void linkBaseToNext();
    void buildRelationTypesAndQualifiers();
    void buildRulesTypesAndQualifiers();
    void buildBaseInitInputDoesPropositions();
    void buildRolePropositions(QList<LRelation> relations);
    void buildStandardEvaluationStructures();
    void buildBaseInputEvaluationStructures();


public:
    //void loadTempRelations(const QVector<LRelation> contents);

    /**
      * Keywords and shit
      */
    public:
        const QVector<LRelation>& getInitState() const;
        const QMap<QString, LRelation>& getInitPropositions() const;
        const QMap<QString, LRelation>& getRolePropositions() const;
        const QVector<LTerm>& getRoles() const;
        const QMap<LTerm, LRelation>& getNextQueries() const;
        const QMap<LTerm, LTerm>& getMapNextToBase() const;


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
        QMap<LTerm, LRelation> mapNextToQuery;

        // All of these can be computed from the rules (legal can not)
        QMap<QString, LRelation> basePropositions;
        QMap<QString, LRelation> initPropositions;
        QMap<QString, LRelation> inputPropositions; // either this line
        QMap<QString, LRelation> doesPropositions;  // or that one will disappear

        QMap<QString, LRelation> rolePropositions;
        QVector<LTerm> roles;

        QMap<QString, LTerm> baseConstants;

        QVector<LRelation> initState; // Convenience
};

#endif // GDLPROVER_H
