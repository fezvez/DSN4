#include "gdlprover.h"

#include <QDebug>

#include "../parser.h"
#include "../flags.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTORS / DESTRUCTORS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GDLProver::GDLProver() : KnowledgeBase()
{

}

GDLProver::~GDLProver()
{

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// GETTERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const QMap<QString, LRelation>& GDLProver::getInitPropositions() const{
    return initPropositions;
}

const QVector<LRelation>& GDLProver::getInitState() const{
    return initState;
}

const QMap<QString, LRelation>& GDLProver::getRolePropositions() const{
    return rolePropositions;
}

const QVector<LTerm>& GDLProver::getRoles() const{
    return roles;
}

const QMap<LTerm, LRelation>& GDLProver::getNextQueries() const{
    return mapNextToQuery;
}

const QMap<LTerm, LTerm>& GDLProver::getMapNextToBase() const{
    return mapNextToBase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// SETUP
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GDLProver::setup(QString filename){
    Parser parser;
    parser.generateHerbrandFromFile(filename);
    setup(parser.getRelations(), parser.getRules());
}

void GDLProver::setup(QList<LRelation> relations, QList<LRule> rules){
    KnowledgeBase::setup(relations, rules);

    criticalDebug("GDLProver setup start");

    linkBaseToNext();

    buildRelationTypesAndQualifiers();
    buildRulesTypesAndQualifiers();
    buildBaseInitInputDoesPropositions();
    buildRolePropositions(relations);
    buildStandardEvaluationStructures();

    criticalDebug("GDLProver setup finished");
}



/**
 * @brief GDLProver::linkBaseToNext
 * next(cell(1,1,x)) is obviously linked to base(cell(1,1,x))
 * We just need to make this relation explicit
 * So what computing the next state will just result in looking at the next propositions
 */
void GDLProver::linkBaseToNext(){
    mapNextToBase.clear();
    mapBaseToNext.clear();

    for(LTerm constant : relationConstantSet){
        QString constantString = constant->toString();
        if(constantString.contains(nextRegExp)){
            QString matchString = constantString.remove(0,5);
            Q_ASSERT(constantMap.contains(matchString));
            LTerm match = constantMap[matchString];
            mapNextToBase.insert(constant, match);
            mapBaseToNext.insert(match, constant);
        }
    }

    for(LTerm term : mapNextToBase.keys()){
        QString stringConstant = term->toString();
        if(!Logic::mapString2GDLType.contains(stringConstant)){
            Logic::mapString2GDLType[stringConstant] = Logic::LOGIC_KEYWORD::NEXT;
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() << "\n\nLink Base Propositions to Next Propositions";

    int padSpaceSize = 16;
    for(LTerm base : mapBaseToNext.keys()){
        qDebug() << "Base proposition " << padSpace(base->toString(), padSpaceSize) << " matched with : " << mapBaseToNext[base]->toString();
    }
    for(LTerm base : mapNextToBase.keys()){
        qDebug() << "Next proposition " << padSpace(base->toString(), padSpaceSize) << "matched with : " << mapNextToBase[base]->toString();
    }
#endif

    mapNextToQuery.clear();
    for(LTerm head : mapNextToBase.keys()){
        QList<LTerm> body;
        for(int i = 0; i<arity[head]; ++i){
            body.append(LTerm(new Logic_Term(QString("?x").append(QString::number(i)), LOGIC_TERM_TYPE::VARIABLE)));
        }
        mapNextToQuery[head] = LRelation(new Logic_Relation(head, body));
    }


#ifndef QT_NO_DEBUG
    qDebug() << "\n\nAll next queries";
    for(LTerm head : mapNextToQuery.keys()){
        qDebug() << "Next query : " << mapNextToQuery[head]->toString();
    }

#endif


}


/**
 * @brief GDLProver::buildRelationTypesAndQualifiers
 * A Qualifier is an "invisible" keyword such as base, init or true
 * They are present to explicitely tell what is the "minimal description" of the game state
 * A Type is a GDL keyword (role, terminal...)
 */
void GDLProver::buildRelationTypesAndQualifiers(){
    baseRelations.clear();
    initRelations.clear();
    standardTypeRelations.clear();

    nextRelations.clear();
    roleRelations.clear();
    goalRelations.clear();
    legalRelations.clear();
    terminalRelations.clear();
    inputRelations.clear();
    doesRelations.clear();
    standardTypeRelations.clear();

    mapQualifierToRelationContainer.clear();
    mapQualifierToRelationContainer.insert(Logic::LOGIC_QUALIFIER::BASE, baseRelations);
    mapQualifierToRelationContainer.insert(Logic::LOGIC_QUALIFIER::INIT, initRelations);
    mapQualifierToRelationContainer.insert(Logic::LOGIC_QUALIFIER::NO_QUAL, standardQualRelations);

    mapTypeToRelationContainer.clear();
    mapTypeToRelationContainer.insert(Logic::LOGIC_KEYWORD::NEXT, nextRelations);
    mapTypeToRelationContainer.insert(Logic::LOGIC_KEYWORD::ROLE, roleRelations);
    mapTypeToRelationContainer.insert(Logic::LOGIC_KEYWORD::GOAL, goalRelations);
    mapTypeToRelationContainer.insert(Logic::LOGIC_KEYWORD::TERMINAL, terminalRelations);
    mapTypeToRelationContainer.insert(Logic::LOGIC_KEYWORD::LEGAL, legalRelations);
    mapTypeToRelationContainer.insert(Logic::LOGIC_KEYWORD::INPUT, inputRelations);
    mapTypeToRelationContainer.insert(Logic::LOGIC_KEYWORD::DOES, doesRelations);
    mapTypeToRelationContainer.insert(Logic::LOGIC_KEYWORD::NO_KEYWORD, standardTypeRelations);



    for(LRelation relation:relationList){
        Logic::LOGIC_QUALIFIER qualifier = relation->getQualifier();
        if(mapQualifierToRelationContainer.contains(qualifier)){
            mapQualifierToRelationContainer[qualifier].append(relation);
            continue;
        }
        mapQualifierToRelationContainer[Logic::NO_QUAL].append(relation);
    }


    for(LRelation relation:relationList){
        Logic::LOGIC_KEYWORD type = Logic::getGDLKeywordFromString(relation->getHead()->toString());
        if(mapTypeToRelationContainer.contains(type)){
            mapTypeToRelationContainer[type].append(relation);
            continue;
        }


        //        if(mapNextToBase.contains(relation->getHead())){
        //            mapTypeToRelationContainer[Logic::NEXT].append(relation);
        //            continue;
        //        }

        mapTypeToRelationContainer[Logic::NO_KEYWORD].append(relation);
    }

    baseRelations = mapQualifierToRelationContainer[Logic::BASE];
    initRelations = mapQualifierToRelationContainer[Logic::INIT];
    standardQualRelations = mapQualifierToRelationContainer[Logic::NO_QUAL];
    Q_ASSERT(!mapQualifierToRelationContainer.contains(Logic::TRUE));

    nextRelations = mapTypeToRelationContainer[Logic::NEXT];
    roleRelations = mapTypeToRelationContainer[Logic::ROLE];
    goalRelations = mapTypeToRelationContainer[Logic::GOAL];
    terminalRelations = mapTypeToRelationContainer[Logic::TERMINAL];
    legalRelations = mapTypeToRelationContainer[Logic::LEGAL];
    inputRelations = mapTypeToRelationContainer[Logic::INPUT];
    doesRelations = mapTypeToRelationContainer[Logic::DOES];
    standardTypeRelations = mapTypeToRelationContainer[Logic::NO_KEYWORD];
    Q_ASSERT(!mapTypeToRelationContainer.contains(Logic::DISTINCT));

#ifndef QT_NO_DEBUG
    qDebug() << "\n\nOrdered Relations";
    qDebug() << "There are " << relationList.size() << " relations";
    qDebug() << "Printing relation qualifiers : ";
    for(Logic::LOGIC_QUALIFIER qualifier : mapQualifierToRelationContainer.keys()){
        QList<LRelation> container = mapQualifierToRelationContainer[qualifier];
        for(LRelation relation : container){
            qDebug() << "\tRelation of qualifier " << Logic::getStringFromGDLQualifier(qualifier) << "\t: " << relation->toString();
        }
    }

    qDebug() << "Printing relation types : ";
    for(Logic::LOGIC_KEYWORD type : mapTypeToRelationContainer.keys()){
        QList<LRelation> container = mapTypeToRelationContainer[type];
        for(LRelation relation : container){
            qDebug() << "\tRelation of type " << Logic::getStringFromGDLKeyword(type) << "\t: " << relation->toString();
        }
    }
    qDebug() << "\n";
#endif
}


/**
 * @brief GDLProver::buildRulesTypesAndQualifiers
 * A Qualifier is an "invisible" keyword such as base, init or true
 * They are present to explicitely tell what is the "minimal description" of the game state
 * A Type is a GDL keyword (role, terminal...)
 */
void GDLProver::buildRulesTypesAndQualifiers(){

    baseRules.clear();
    initRules.clear();
    standardQualRules.clear();

    nextRules.clear();
    roleRules.clear();
    goalRules.clear();
    terminalRules.clear();
    legalRules.clear();
    inputRules.clear();
    doesRules.clear();
    standardTypeRules.clear();



    mapQualifierToRuleContainer.clear();
    mapQualifierToRuleContainer.insert(Logic::LOGIC_QUALIFIER::BASE, baseRules);
    mapQualifierToRuleContainer.insert(Logic::LOGIC_QUALIFIER::INIT, initRules);
    mapQualifierToRuleContainer.insert(Logic::LOGIC_QUALIFIER::NO_QUAL, standardQualRules);

    mapTypeToRuleContainer.clear();
    mapTypeToRuleContainer.insert(Logic::LOGIC_KEYWORD::NEXT, nextRules);
    mapTypeToRuleContainer.insert(Logic::LOGIC_KEYWORD::ROLE, roleRules);
    mapTypeToRuleContainer.insert(Logic::LOGIC_KEYWORD::GOAL, goalRules);
    mapTypeToRuleContainer.insert(Logic::LOGIC_KEYWORD::TERMINAL, terminalRules);
    mapTypeToRuleContainer.insert(Logic::LOGIC_KEYWORD::LEGAL, legalRules);
    mapTypeToRuleContainer.insert(Logic::LOGIC_KEYWORD::INPUT, inputRules);
    mapTypeToRuleContainer.insert(Logic::LOGIC_KEYWORD::DOES, doesRules);
    mapTypeToRuleContainer.insert(Logic::LOGIC_KEYWORD::NO_KEYWORD, standardTypeRules);


    for(LRule rule : ruleList){
        Logic::LOGIC_QUALIFIER qualifier = rule->getHead()->getQualifier();
        if(mapQualifierToRuleContainer.contains(qualifier)){
            mapQualifierToRuleContainer[qualifier].append(rule);
            continue;
        }
        mapQualifierToRuleContainer[Logic::NO_QUAL].append(rule);
    }

    for(LRule rule : ruleList){
        Logic::LOGIC_KEYWORD type = Logic::getGDLKeywordFromString(rule->getHead()->getHead()->toString());
        if(mapTypeToRuleContainer.contains(type)){
            mapTypeToRuleContainer[type].append(rule);
            continue;
        }

        if(mapNextToBase.contains(rule->getHead()->getHead())){
            mapTypeToRuleContainer[Logic::NEXT].append(rule);
            continue;
        }

        mapTypeToRuleContainer[Logic::NO_KEYWORD].append(rule);
    }

    baseRules = mapQualifierToRuleContainer[Logic::BASE];
    initRules = mapQualifierToRuleContainer[Logic::INIT];
    standardQualRules = mapQualifierToRuleContainer[Logic::NO_QUAL];
    Q_ASSERT(!mapQualifierToRuleContainer.contains(Logic::TRUE));

    nextRules = mapTypeToRuleContainer[Logic::NEXT];
    roleRules = mapTypeToRuleContainer[Logic::ROLE];
    goalRules = mapTypeToRuleContainer[Logic::GOAL];
    terminalRules = mapTypeToRuleContainer[Logic::TERMINAL];
    legalRules = mapTypeToRuleContainer[Logic::LEGAL];
    inputRules = mapTypeToRuleContainer[Logic::INPUT];
    doesRules = mapTypeToRuleContainer[Logic::DOES];
    standardTypeRules = mapTypeToRuleContainer[Logic::NO_KEYWORD];
    Q_ASSERT(!mapTypeToRuleContainer.contains(Logic::DISTINCT));

#ifndef QT_NO_DEBUG
    qDebug() << "\n\nOrdered Rules";
    qDebug() << "There are " << ruleList.size() << " rules";
    qDebug() << "Printing rules qualifiers";
    for(Logic::LOGIC_QUALIFIER qualifier : mapQualifierToRuleContainer.keys()){
        QList<LRule> container = mapQualifierToRuleContainer[qualifier];
        for(LRule rule : container){
            qDebug() << "\tRule of qualifier \t" << Logic::getStringFromGDLQualifier(qualifier) << "\t: " << rule->toString();
        }
    }
    qDebug() << "Printing rule types";
    for(Logic::LOGIC_KEYWORD type : mapTypeToRuleContainer.keys()){
        QList<LRule> container = mapTypeToRuleContainer[type];
        for(LRule rule : container){
            qDebug() << "\tRule of type \t" << Logic::getStringFromGDLKeyword(type) << "\t: " << rule->toString();
        }
    }
#endif


}

/**
 * Reminder about the Logic Type and Qualifier
    enum LOGIC_QUALIFIER{
        NO_QUAL,
        BASE,
        INIT,
        TRUE
    };

    enum LOGIC_TYPE{
        NO_TYPE,
        ROLE,
        NEXT,
        GOAL,
        LEGAL,
        TERMINAL,
        DOES,
        INPUT,
        DISTINCT
    };
 */

/**
 * @brief GDLProver::buildStandardEvaluationStructures
 * This is what you need to compute the next state
 * Remove :
 *  - The "base" relations/rules
 *  - The "init" relations/rules
 *  - The "input" relations/rules
 *  - The "does" relations/rules
 */
void GDLProver::buildStandardEvaluationStructures(){
    evaluationRules.clear();
    for(LRule rule : ruleList){
        LRelation ruleHead = rule->getHead();
        Logic::LOGIC_QUALIFIER qual = ruleHead->getQualifier();
        switch(qual){
        case(Logic::BASE):
        case(Logic::INIT):
            continue;
        default:
            break;
        }

        LTerm constant = ruleHead->getHead();
        Logic::LOGIC_KEYWORD type = constant->getKeyword();
        switch(type){
        case(Logic::INPUT):
        case(Logic::DOES):
            continue;
        default:
            break;
        }

        evaluationRules.append(rule);
        //        qDebug() << rule->toString();
        //        qDebug() << Logic::getStringFromGDLKeyword(rule->getHead()->getHead()->getKeyword());

    }


    evaluationRelations.clear();
    for(LRelation relation : relationList){
        Logic::LOGIC_QUALIFIER qual = relation->getQualifier();
        switch(qual){
        case(Logic::BASE):
        case(Logic::INIT):
            continue;
        default:
            break;
        }

        LTerm constant = relation->getHead();
        Logic::LOGIC_KEYWORD type = constant->getKeyword();
        switch(type){
        case(Logic::INPUT):
        case(Logic::DOES):
            continue;
        default:
            break;
        }

        evaluationRelations.append(relation);
    }

    buildEvaluationMap();
}

/**
 * @brief GDLProver::buildBaseInputEvaluationStructures
 * This is what you need to compute the base and input propositions
 * This literally is used only once, at the beginning of the game
 */
void GDLProver::buildBaseInputEvaluationStructures(){
    evaluationRules.clear();
    evaluationRules = ruleList;

    evaluationRelations.clear();
    evaluationRelations = relationList;

    buildEvaluationMap();
}



/**
 * @brief GDLProver::buildBaseInitInputDoesPropositions
 */
void GDLProver::buildBaseInitInputDoesPropositions(){

    debug("\n\nBase, Init, Input and Does Propositions");

    buildBaseInputEvaluationStructures();

    // BASE
    basePropositions.clear();
    for(LRelation relation : baseRelations){
        basePropositions[relation->toString()] = relation;
    }

    for(LRule rule : baseRules){
        //qDebug () << "Base rule " << rule->toString();
        LRelation head = rule->getHead();
        //qDebug () << "Head  " << head->toString();
        QList<LRelation> evaluatedRelations = evaluate(head);
        for(LRelation relation : evaluatedRelations){
            QString stringRelation = relation->toString();
            if(!basePropositions.contains(stringRelation)){
                basePropositions[stringRelation] = relation;
            }
        }
    }
#ifndef QT_NO_DEBUG
    qDebug() << "There are " << basePropositions.size() << " base propositions";
    for(QString s : basePropositions.keys()){
        qDebug() << "\tBase proposition : " << s;
    }
#endif

    baseConstants.clear();
    for(LRelation relation : basePropositions.values()){
        LTerm head = relation->getHead();
        baseConstants.insert(head->toString(), head);
    }
#ifndef QT_NO_DEBUG
    qDebug() << "Nb of base constant relations : " << baseConstants.size();
    for(LTerm term : baseConstants.values()){
        qDebug() << "\tBase constant : " << term->toString();
    }
#endif

    // INIT
    initPropositions.clear();
    for(LRelation relation : initRelations){
//        LRelation relationWithoutInit = Logic_Relation::clone(relation);
//        relationWithoutInit->setQualifier(Logic::LOGIC_QUALIFIER::NO_QUAL);
//        initPropositions[relationWithoutInit->toString()] = relationWithoutInit;

        LRelation relationSwappedToBase = Logic_Relation::clone(relation);
        relationSwappedToBase->setQualifier(Logic::LOGIC_QUALIFIER::BASE);
        initPropositions[relationSwappedToBase->toString()] = relationSwappedToBase;
    }

    for(LRule rule : initRules){
        //qDebug () << "init rule " << rule->toString();
        LRelation head = rule->getHead();
        //qDebug () << "Head  " << head->toString();
        QList<LRelation> evaluatedRelations = evaluate(head);
        for(LRelation relation : evaluatedRelations){
            QString stringRelation = relation->toString();
            if(!initPropositions.contains(stringRelation)){
                initPropositions[stringRelation] = basePropositions[stringRelation];
            }
        }
    }
#ifndef QT_NO_DEBUG
    qDebug() << "There are " << initPropositions.size() << " init propositions";
    for(QString s : initPropositions.keys()){
        qDebug() << "\tInit proposition : " << s;
    }
#endif


    // INIT for convenience (can call loadTempRelations(initState))
    initState.clear();
    for(LRelation relation : initPropositions.values()){
        initState.append(relation);
    }

    inputPropositions.clear();
    for(LRelation relation : inputRelations){
        inputPropositions[relation->toString()] = relation;
    }

    for(LRule rule : inputRules){
        //qDebug () << "Input rule " << rule->toString();
        LRelation head = rule->getHead();
        //qDebug () << "Head  " << head->toString();
        QList<LRelation> evaluatedRelations = evaluate(head);
        for(LRelation relation : evaluatedRelations){
            QString stringRelation = relation->toString();
            if(!inputPropositions.contains(stringRelation)){
                inputPropositions[stringRelation] = relation;
            }
        }
    }
#ifndef QT_NO_DEBUG
    qDebug() << "There are " << inputPropositions.size() << " input propositions";
    for(QString s : inputPropositions.keys()){
        qDebug() << "\tInput proposition : " << s;
    }
#endif

    doesPropositions.clear();
    for(LRelation relation : doesRelations){
        doesPropositions[relation->toString()] = relation;
    }

    for(LRule rule : doesRules){
        //qDebug () << "Does rule " << rule->toString();
        LRelation head = rule->getHead();
        //qDebug () << "Head  " << head->toString();
        QList<LRelation> evaluatedRelations = evaluate(head);
        for(LRelation relation : evaluatedRelations){
            QString stringRelation = relation->toString();
            if(!doesPropositions.contains(stringRelation)){
                doesPropositions[stringRelation] = relation;
            }
        }
    }
#ifndef QT_NO_DEBUG
    qDebug() << "There are " << doesPropositions.size() << " does propositions";
    for(QString s : doesPropositions.keys()){
        qDebug() << "\tDoes proposition : " << s;
    }
#endif
}

void GDLProver::buildRolePropositions(QList<LRelation> relations){

    rolePropositions.clear();
    for(LRelation relation : roleRelations){
        rolePropositions.insert(relation->toString(), relation);
    }

    for(LRule rule : roleRules){
        qDebug () << "Role rule " << rule->toString();  // Should not exist, see below
        LRelation head = rule->getHead();
        QList<LRelation> evaluatedRelations = evaluate(head);
        for(LRelation relation : evaluatedRelations){
            rolePropositions.insert(relation->toString(), relation);
        }
    }

    roles.clear();
    // Exception, the roles are in the order given by the rules, we can't do that :
    /**
    for(LRelation roleRelation : rolePropositions.values()){
        roles.append(roleRelation->getBody()[0]);
    }
    */

    Q_ASSERT(constantMap.contains("role"));
    //    qDebug() << "List of Roles";
//    LTerm roleConstant = constantMap["role"];
    for(LRelation relation : relations){
        //        qDebug() << "Relation " << relation->toString();
        if(relation->getHead()->toString() == "role"){ // Can't use LTerm equality here

            roles.append(manageTerm(relation->getBody()[0]));
            //            qDebug() << "Role " << relation->getBody()[0]->toString();
        }
    }

//    for(LTerm roleTerm : roles){
//        qDebug() << "Role " << roleTerm->toString() << " with address " << roleTerm.data();
//    }
}

