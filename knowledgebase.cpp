#include "KnowledgeBase.h"

#include <QDebug>
#include <QLinkedList>

#include "Unification/unification_relation.h"

int KnowledgeBase::skolemNumber = 1000;

KnowledgeBase::KnowledgeBase()
{

}


void KnowledgeBase::setup(QList<LRelation> relations, QList<LRule> rules){
    qDebug() <<"\n\nKNOWLEDGE BASE SETUP";

    relationList.clear();
    ruleList.clear();
    for(LRelation relation : relations){
        relationList.append(relation->clone());
    }
    for(LRule rule : rules){
        ruleList.append(rule->clone());
    }

    createRegExp();
    storeConstants();
    printConstants();   // Debug
    linkBaseToNext();
    buildRelationTypesAndQualifiers();
    buildRulesTypesAndQualifiers();
    buildBaseInitInputDoesPropositions();
    buildRolePropositions();
    buildStandardEvaluationStructures();

    //    buildArity();
    //    printConstantsWithArity();
    //    printFreeVariables();
    //    generateStratum();

    //debug();
}


void KnowledgeBase::debug(){

}


QMap<QString, LTerm> KnowledgeBase::getConstantMap(){
    return constantMap;
}


/**
 * @brief KnowledgeBase::evaluateRelation
 * @param relation
 * @return
 * Takes a relation (optionnaly with variables) and returns all the grounded
 * relations that are true (given the current )
 */
QList<LRelation> KnowledgeBase::evaluateRelation(LRelation relation){
    //Q_ASSERT(!relation->isNegation());
    //    qDebug() << "KnowledgeBase::evaluateRelation";
    LRelation updatedRelation = manageRelation(relation);   // Every constant is now a copy from the knowledge base


    //    LTerm head = updatedRelation->getHead();
    //    qDebug() << "Head is " << head->toString();

    return evaluate(updatedRelation);
}

QList<LRelation> KnowledgeBase::evaluate(LRelation r){
    //    qDebug() << "\nKnowledgeBase::evaluate(). Relation is : " << r->toString();
    LRelation head = manageRelation(r->clone());
    QList<LRelation> body;
    body.append(manageRelation(r->clone()));

    LRule rule = LRule(new Logic_Rule(head, body));

    QList<LRelation> answer;
    QSet<QString> answerString;

    QList<LRule> possibleAnswers;
    possibleAnswers.append(rule);


    while (!possibleAnswers.isEmpty()) {
                //qDebug() << "\nKnowledgeBase::evaluate() possibleAnswer : " << possibleAnswers.last()->toString();
        LRule possibleRule = possibleAnswers.last();
        possibleAnswers.removeLast();

        // If the rule is fully grounded
        if(possibleRule->getHead()->isGround()){
            // If we know it's already true for that grounding, we can discard it
            if(answerString.contains(possibleRule->getHead()->toString())){
                continue;
            }
        }

        // If the body is empty, we have an answer to our evaluation query
        if(possibleRule->isBodyEmpty()){
            //            qDebug() << "We have an answer : " << possibleRule->getHead()->toString();
            answer.append(possibleRule->getHead());
            answerString << possibleRule->getHead()->toString();
            continue;
        }

        // Else, we take the first relation and process it
        QList<LRule> substitutions = ruleSubstitution(possibleRule);
        possibleAnswers.append(substitutions);
    }
    //    if(answer.isEmpty()){
    //                qDebug() << "No answer";
    //    }
    return answer;
}



/**
 * @brief KnowledgeBase::ruleSubstitution
 * @param rule
 * @return
 * Take the first relation of the body :
 *  - substitute it with a relation from constantToRelationEvaluationMap
 *  - substitute it with a rule from constantToRuleEvaluationMap
 */
QList<LRule> KnowledgeBase::ruleSubstitution(LRule rule){
        //qDebug() << "\nKnowledgeBase::ruleSubstitution " << rule->toString();
    QList<LRule> answer;

    LRelation firstRelation = rule->getBody().first();
    LTerm head = firstRelation->getHead();

    //
    if(firstRelation->isNegation()){
        LRelation nonNegativeRelation = firstRelation->clone();
        nonNegativeRelation->setNegation(false);
        QList<LRelation> subproblemAnswer = evaluate(nonNegativeRelation);
        if(subproblemAnswer.size() == 0){
            QList<LRelation> endOfBody = rule->getBody();
            endOfBody.removeFirst();

            LRule partialAnswer = manageRule(LRule(new Logic_Rule(rule->getHead(), endOfBody)));
            answer.append(partialAnswer);
        }
        else{

        }
        return answer;
    }

    if(head->toString() == QString("distinct")){

    }

//    if(head->toString() == QString("not")){
//        QList<LRelation> subProblem = evaluate(firstRelation->getBody()[0]);
//        if(subProblem.empty()){

//        }
//    }

    // Substitute with facts
    if(constantToRelationEvaluationMap.contains(head)){
        QList<LRelation> canBeSubstituted = constantToRelationEvaluationMap[head];
        for(LRelation r : canBeSubstituted){
                        //qDebug() << "\n    Sub possible with relation " << r->toString();
            URelation unification = URelation(new Unification_Relation(firstRelation, r));
            if(unification->isUnificationValid()){

                LRule tempRule = unification->applySubstitution(rule);
                //                qDebug() << "    Unification is valid and gives : " << tempRule->toString();
                //unification->printSolverResults();

                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), endOfBody)));
                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
                answer.append(partialAnswer);
            }
        }
    }

    // Substitute with temporary facts
    if(constantToTempRelationEvaluationMap.contains(head)){
        QList<LRelation> canBeSubstituted = constantToTempRelationEvaluationMap[head];
        for(LRelation r : canBeSubstituted){
                        //qDebug() << "\n    Sub possible with temp relation " << r->toString();
            URelation unification = URelation(new Unification_Relation(firstRelation, r));
            if(unification->isUnificationValid()){

                LRule tempRule = unification->applySubstitution(rule);
                //                qDebug() << "    Unification is valid and gives : " << tempRule->toString();
                //unification->printSolverResults();

                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), endOfBody)));
                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
                answer.append(partialAnswer);
            }
        }
    }

    // Substitute with rule
    if(constantToRuleEvaluationMap.contains(head)){
        QList<LRule> canBeSubstituted = constantToRuleEvaluationMap[head];
        for(LRule r : canBeSubstituted){
                        //qDebug() << "\n    Sub possible with rule" << r->toString();
            LRule skolemRule = buildSkolemRule(rule, r);
            //            qDebug() << "    Which is updated in " << skolemRule->toString();
            URelation unification = URelation(new Unification_Relation(firstRelation, skolemRule->getHead()));
            if(unification->isUnificationValid()){
                unification->applySubstitutionInPlace(skolemRule);
                LRule tempRule = unification->applySubstitution(rule);

                //                qDebug() << "    Unification is valid and gives original rule : " << tempRule->toString();
                //                qDebug() << "    and the skolem : " << skolemRule->toString();
                //unification->printSolverResults();

                QList<LRelation> body;
                body.append(skolemRule->getBody());
                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();
                body.append(endOfBody);

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), body)));
                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
                answer.append(partialAnswer);
            }
        }
    }
    return answer;
}

/**
 * @brief KnowledgeBase::buildSkolemRule
 * @param originalRule
 * @param ruleFromKB
 * @return
 * We have the first relation of the body, and we want to substitute it with a rule in our KB
 * It's necessary to rename variables in the body of this rule to prevent name clashing
 */
LRule KnowledgeBase::buildSkolemRule(LRule originalRule, LRule ruleFromKB){
    LRule answer = ruleFromKB->clone();

    QSet<QString> var1 = originalRule->getFreeVariables();
    QSet<QString> var2 = ruleFromKB->getFreeVariables();

    //    qDebug() << "        SKOLEM";
    //    for(QString varName : var1){
    //        qDebug() << "        Var in 1st rule : " << varName;
    //    }
    //    for(QString varName : var2){
    //        qDebug() << "        Var in 2nd rule : " << varName;
    //    }

    QSet<QString> skolemizedVariables; // All the skolem variables we used so far

    for(QString varName : var2){
        if(!var1.contains(varName)){
            continue;
        }
        //        qDebug() << "        Matching variable between the 2 rules : " << varName;

        QString skolemVariable;
        bool readableSkolem = false;

        // First try
        // Try to add a character to the end of the name of the variable
        for(int i=0; i<26; ++i){
            QChar c(97+25-i);
            QString s("?");
            s.append(c);
            //            qDebug() << "        Try skolem variable " << s;

            if(var1.contains(s)){
                continue;
            }
            if(var2.contains(s)){
                continue;
            }
            if(skolemizedVariables.contains(s)){
                continue;
            }

            skolemizedVariables << s;
            readableSkolem = true;
            skolemVariable = s;
            break;
        }
        // Second try
        // Add a number that is guaranteed to be unique
        if(!readableSkolem){
            skolemVariable = QString("SK_").append(QString::number(KnowledgeBase::skolemNumber));
            skolemNumber++;
        }

        LTerm v1 = LTerm(new Logic_Term(varName, VARIABLE));
        LTerm v2 = LTerm(new Logic_Term(skolemVariable, VARIABLE));

        //        qDebug() << "        Try to skolemize var " << varName << " into skolem " << skolemVariable << " in rule " << answer->toString();
        answer->substitute(v1, v2);
        //        qDebug() << "        Result is " << answer->toString();
    }
    return answer;
}




/**
 * @brief KnowledgeBase::createRegExp
 * Regular expressions for parsing
 */
void KnowledgeBase::createRegExp(){
    ruleRegExp = QRegExp("<=");
    whitespaceRegExp = QRegExp("\\s+");
    leftPar = QRegExp("^\\($");
    rightPar = QRegExp("^\\)$");
    inputRegExp = QRegExp("(\\(|\\s)input(?![\\w|_|-])");
    nextRegExp = QRegExp("^next_");
}

/**
 * @brief KnowledgeBase::storeConstants
 * We have :
 *  - object constants
 *  - relation constants
 *  - function constants
 * Let's store all of them in memory
 */
void KnowledgeBase::storeConstants(){
    qDebug() << "Store constants";
    constantMap.clear();
    objectConstantSet.clear();
    functionConstantSet.clear();
    relationConstantSet.clear();

    QList<LRelation> clonedRelationList;
    for(LRelation relation : relationList){
        clonedRelationList.append(manageRelation(relation));
    }
    relationList = clonedRelationList;

    QList<LRule> clonedRuleList;
    for(LRule rule : ruleList){
        clonedRuleList.append(manageRule(rule));
    }
    ruleList = clonedRuleList;

#ifndef QT_NO_DEBUG
    for(LRelation relation : relationList){
        qDebug() << "Relation " << relation->toString() << " head address " << relation->getHead().data();
    }
    for(LRule rule : ruleList){
        qDebug() << "Rule " << rule->toString() << " head address " << rule->getHead()->getHead().data();
    }
#endif
}

LRule KnowledgeBase::manageRule(LRule r){
    LRelation head = manageRelation(r->getHead());
    QList<LRelation> body;
    for(LRelation sentence : r->getBody()){
        body.append(manageRelation(sentence));
    }
    return LRule(new Logic_Rule(head, body));
}


LRelation KnowledgeBase::manageRelation(LRelation r){
    LTerm head = manageRelationConstant(r->getHead());

    QList<LTerm> body;
    for(LTerm term : r->getBody()){
        body.append(manageTerm(term));
    }
    return LRelation(new Logic_Relation(head, body, r->getQualifier(), r->isNegation()));
}

LTerm KnowledgeBase::manageTerm(LTerm t){
    LTerm answer;
    switch(t->getType()){
    case(CONSTANT):
        answer = manageObjectConstant(t);
        break;
    case(VARIABLE):
        return t;
    case(FUNCTION):
        LTerm head = manageFunctionConstant(t->getHead());
        QList<LTerm> body;
        for(LTerm subTerm : t->getBody()){
            body.append(manageTerm(subTerm));
        }
        answer = LTerm(new Logic_Term(head, body));
        break;
    }
    return answer;
}

LTerm KnowledgeBase::manageConstant(LTerm c){
    QString s = c->toString();
    if(!constantMap.contains(s)){
        constantMap.insert(s, c);
    }
    return constantMap[s];
}

LTerm KnowledgeBase::manageFunctionConstant(LTerm c){
    LTerm constant = manageConstant(c);
    if(!functionConstantSet.contains(constant)){
        functionConstantSet.insert(constant);
    }
    return constant;
}

LTerm KnowledgeBase::manageRelationConstant(LTerm c){
    LTerm constant = manageConstant(c);
    if(!relationConstantSet.contains(constant)){
        relationConstantSet.insert(constant);
    }
    return constant;
}

LTerm KnowledgeBase::manageObjectConstant(LTerm c){
    LTerm constant = manageConstant(c);
    if(!objectConstantSet.contains(constant)){
        objectConstantSet.insert(constant);
    }
    return constant;
}





void KnowledgeBase::printConstants(){
    qDebug() << "\n\nLIST OF CONSTANTS";
    for(LTerm constant : objectConstantSet){
        qDebug() << "Object constant : " << constant->toString();
    }
    for(LTerm constant : relationConstantSet){
        qDebug() << "Relation constant : " << constant->toString();
    }
    for(LTerm constant : functionConstantSet){
        qDebug() << "Function constant : " << constant->toString();
    }

    qDebug() << "Constant map size " << constantMap.size();

    Q_ASSERT((objectConstantSet.size() + functionConstantSet.size() + relationConstantSet.size()) == constantMap.size());
}

void KnowledgeBase::printConstantsWithArity(){
    qDebug() << "\n\nLIST OF CONSTANTS";
    for(LTerm constant : objectConstantSet){
        qDebug() << "Object constant : " << constant->toString();
    }
    for(LTerm constant : relationConstantSet){
        qDebug() << "Relation constant : " << constant->toString() << "\twith arity " << arity[constant];
    }
    for(LTerm constant : functionConstantSet){
        qDebug() << "Function constant : " << constant->toString() << "\twith arity " << arity[constant];
    }
}

/**
 * @brief KnowledgeBase::linkBaseToNext
 * next(cell(1,1,x)) is obviously linked to base(cell(1,1,x))
 * We just need to make this relation explicit
 * So what computing the next state will just result in looking at the next propositions
 */
void KnowledgeBase::linkBaseToNext(){
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

    Logic::init();
    for(LTerm term : mapNextToBase.keys()){
        QString stringConstant = term->toString();
        if(!Logic::mapString2GDLType.contains(stringConstant)){
            Logic::mapString2GDLType[stringConstant] = Logic::LOGIC_KEYWORD::NEXT;
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() << "\nLINK BASE TO NEXT";

    for(LTerm base : mapBaseToNext.keys()){
        qDebug() << "Base proposition " << base->toString() << "\tmatched with : " << mapBaseToNext[base]->toString();
    }
    for(LTerm base : mapNextToBase.keys()){
        qDebug() << "Next proposition " << base->toString() << "\tmatched with : " << mapNextToBase[base]->toString();
    }
#endif

}


/**
 * @brief KnowledgeBase::buildRelationTypesAndQualifiers
 * A Qualifier is an "invisible" keyword such as base, init or true
 * They are present to explicitely tell what is the "minimal description" of the game state
 * A Type is a GDL keyword (role, terminal...)
 */
void KnowledgeBase::buildRelationTypesAndQualifiers(){
    qDebug() << "\n\nORDERED RELATIONS";
    qDebug() << "There are " << relationList.size() << " relations";


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

    qDebug() << "\nPrinting relation qualifiers";
    for(Logic::LOGIC_QUALIFIER qualifier : mapQualifierToRelationContainer.keys()){
        QList<LRelation> container = mapQualifierToRelationContainer[qualifier];
        for(LRelation relation : container){
            qDebug() << "Relation of qualifier " << Logic::getStringFromGDLQualifier(qualifier) << "\t: " << relation->toString();
        }
    }

    qDebug() << "\nPrinting relation types";
    for(Logic::LOGIC_KEYWORD type : mapTypeToRelationContainer.keys()){
        QList<LRelation> container = mapTypeToRelationContainer[type];
        for(LRelation relation : container){
            qDebug() << "Relation of type " << Logic::getStringFromGDLKeyword(type) << "\t: " << relation->toString();
        }
    }
}


/**
 * @brief KnowledgeBase::buildRulesTypesAndQualifiers
 * A Qualifier is an "invisible" keyword such as base, init or true
 * They are present to explicitely tell what is the "minimal description" of the game state
 * A Type is a GDL keyword (role, terminal...)
 */
void KnowledgeBase::buildRulesTypesAndQualifiers(){
    qDebug() << "\n\nORDERED RULES";
    qDebug() << "There are " << ruleList.size() << " rules";

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



    qDebug() << "\nPrinting rules qualifiers";
    for(Logic::LOGIC_QUALIFIER qualifier : mapQualifierToRuleContainer.keys()){
        QList<LRule> container = mapQualifierToRuleContainer[qualifier];
        for(LRule rule : container){
            qDebug() << "Rule of qualifier \t" << Logic::getStringFromGDLQualifier(qualifier) << "\t: " << rule->toString();
        }
    }
    qDebug() << "\nPrinting rule types";
    for(Logic::LOGIC_KEYWORD type : mapTypeToRuleContainer.keys()){
        QList<LRule> container = mapTypeToRuleContainer[type];
        for(LRule rule : container){
            qDebug() << "Rule of type \t" << Logic::getStringFromGDLKeyword(type) << "\t: " << rule->toString();
        }
    }



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
 * @brief KnowledgeBase::buildStandardEvaluationStructures
 * This is what you need to compute the next state
 * Remove :
 *  - The "base" relations/rules
 *  - The "init" relations/rules
 *  - The "input" relations/rules
 *  - The "does" relations/rules
 */
void KnowledgeBase::buildStandardEvaluationStructures(){
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
 * @brief KnowledgeBase::buildBaseInputEvaluationStructures
 * This is what you need to compute the base and input propositions
 * This literally is used only once, at the beginning of the game
 */
void KnowledgeBase::buildBaseInputEvaluationStructures(){
    evaluationRules.clear();
    evaluationRules = ruleList;

    evaluationRelations.clear();
    evaluationRelations = relationList;

    buildEvaluationMap();
}

/**
 * @brief KnowledgeBase::buildEvaluationMap
 */
void KnowledgeBase::buildEvaluationMap(){
    constantToRelationEvaluationMap.clear();
    for(LRelation relation : evaluationRelations){
        LTerm head = relation->getHead();
        if(!constantToRelationEvaluationMap.contains(head)){
            constantToRelationEvaluationMap.insert(head, QList<LRelation>());
        }
        constantToRelationEvaluationMap[head].append(relation);
    }

    constantToRuleEvaluationMap.clear();
    for(LRule rule : evaluationRules){
        LTerm head = rule->getHead()->getHead();
        if(!constantToRuleEvaluationMap.contains(head)){
            constantToRuleEvaluationMap.insert(head, QList<LRule>());
        }
        constantToRuleEvaluationMap[head].append(rule);
    }

#ifndef QT_NO_DEBUG
    //    qDebug() << "\nCONSTANT TO RELATION MAP";
    //    for(LTerm relationConstant : constantToRelationMap.keys()){
    //        for(LRelation r : constantToRelationMap[relationConstant]){
    //            qDebug() << "Relation constant " << relationConstant->toString() << "\tleads to relation " << r->toString();
    //        }
    //    }

    //    qDebug() << "\nCONSTANT TO RULE MAP";
    //    for(LTerm relationConstant : constantToRuleMap.keys()){
    //        for(LRule r : constantToRuleMap[relationConstant]){
    //            Q_ASSERT(relationConstant.data() == constantMap[QString(relationConstant->toString())].data());
    //            qDebug() << "Relation constant " << relationConstant->toString() << "\tleads to rule " << r->toString();
    //        }
    //    }
#endif
}

void KnowledgeBase::loadTempRelations(const QVector<LRelation> contents){
    evaluationTempRelations.clear();
    constantToTempRelationEvaluationMap.clear();

    for(LRelation relation : contents){
        evaluationTempRelations.append(relation);

        LTerm head = relation->getHead();
        if(!constantToTempRelationEvaluationMap.contains(head)){
            constantToTempRelationEvaluationMap.insert(head, QList<LRelation>());
        }
        constantToTempRelationEvaluationMap[head].append(relation);
    }
}



/**
 * @brief KnowledgeBase::buildArity
 * I thought this would be useful, but it's just a debugging tool as of now
 *
 */
void KnowledgeBase::buildArity(){
    constantToRelationMap.clear();
    constantToRuleMap.clear();
    for(LRelation relation : relationList){
        LTerm head = relation->getHead();
        if(!constantToRelationMap.contains(head)){
            constantToRelationMap.insert(head, QList<LRelation>());
        }
        constantToRelationMap[head].append(relation);
    }
    for(LRule rule : ruleList){
        LTerm head = rule->getHead()->getHead();
        if(!constantToRuleMap.contains(head)){
            constantToRuleMap.insert(head, QList<LRule>());
        }
        constantToRuleMap[head].append(rule);
    }


    arity.clear();

    // Special treatment for the keyword distinct
    // 1/30/15 : I have no idea why the need for this ><
    if(!constantMap.contains(QString("distinct"))){
        constantMap.insert(QString("distinct"), LTerm(new Logic_Term(QString("distinct"), CONSTANT)));
    }
    LTerm distinctConstant = constantMap[QString("distinct")];
    arity.insert(distinctConstant, 2);

    for(LTerm key : constantToRelationMap.keys()){
        QList<LRelation> relations = constantToRelationMap[key];
        int size = relations.size();
        Q_ASSERT(size>=1);
        int ar = relations[0]->getBody().size();
        for(int i=1; i<relations.size(); ++i){
            Q_ASSERT(relations[i]->getBody().size() == ar);
        }
        arity.insert(key, ar);
    }

    // Head of rules
    for(LTerm key : constantToRuleMap.keys()){
        QList<LRule> rules = constantToRuleMap[key];
        int size = rules.size();
        Q_ASSERT(size>=1);
        for(int i=0; i<size; ++i){
            LRule rule = rules[i];
            LRelation head = rule->getHead();
            LTerm relationHead = head->getHead();
            int ar = head->getBody().size();
            if(arity.contains(relationHead)){
                Q_ASSERT(arity[relationHead] == ar);
            }
            else{
                arity.insert(relationHead, ar);
            }
        }
    }



#ifndef QT_NO_DEBUG
    qDebug() << "\nCheck that arity is consistent";
    // Check rules
    for(LTerm key : constantToRuleMap.keys()){
        QList<LRule> rules = constantToRuleMap[key];
        for(LRule rule : rules){

            LRelation relationHead = rule->getHead();
            LTerm headConstant = relationHead->getHead();
            int ar = relationHead->getBody().size();
            Q_ASSERT(arity.contains(headConstant));
            Q_ASSERT(arity[headConstant] == ar);

            for(LRelation sentence : rule->getBody()){
                checkArity(sentence);
            }
        }
    }

    // Check relations
    for(LTerm key : constantToRelationMap.keys()){
        QList<LRelation> relations = constantToRelationMap[key];
        for(LRelation relation : relations){
            LTerm headConstant = relation->getHead();
            int ar = relation->getBody().size();
            Q_ASSERT(arity.contains(headConstant));
            Q_ASSERT(arity[headConstant] == ar);
        }
    }
#endif


}


/**
 * @brief KnowledgeBase::checkArity
 * @param relation
 * Debugging tool
 */
void KnowledgeBase::checkArity(LRelation relation){
    LTerm relationHead = relation->getHead();
    int ar = relation->getBody().size();
    Q_ASSERT(arity.contains(relationHead));
    Q_ASSERT(arity[relationHead] == ar);
}


// Is just a debugging tool
// It is not needed for the computation
// It is just to check that the GDL rules are well formed
void KnowledgeBase::generateStratum(){
    qDebug() << "\n\nGENERATE STRATUM";

    for(LTerm constant : arity.keys()){
        stratumMap.insert(constant, LStratum(new Stratum2(constant)));
    }

    for(LRule rule : ruleList){
        LTerm head = rule->getHead()->getHead();
        LStratum stratum = stratumMap[head];


        QSet<LTerm> dependents;
        QSet<LTerm> dependentsNegative;

        for(LRelation relation : rule->getBody()){
            LTerm relationConstant = relation->getHead();
            if(relation->isNegation()){
                dependentsNegative.insert(relationConstant);
            }
            else{
                dependents.insert(relationConstant);
            }
        }

        for(LTerm d : dependents){
            Q_ASSERT(stratumMap.contains(d));
            stratum->addDependency(stratumMap[d]);
        }



        for(LTerm dn : dependentsNegative){
            Q_ASSERT(stratumMap.contains(dn));
            stratum->addDependencyNegative(stratumMap[dn]);
        }
    }


    bool update = true;
    int nbStrongIteration = 0;

    while(update && nbStrongIteration<100){
        update = false;
        nbStrongIteration++;
        for(LStratum stratum : stratumMap.values()){
            update = update || stratum->updateStrataStrongly();
        }
    }

    update = true;
    int nbIteration = 0;

    while(update && nbIteration<10000){
        update = false;
        nbIteration++;
        for(LStratum stratum : stratumMap.values()){
            update = update || stratum->updateStrata();
        }
    }

    qDebug() << "Nb strong iterations : " << nbStrongIteration;
    qDebug() << "Nb iterations : " << nbIteration;

    QList<LStratum> listOfStratum = stratumMap.values();
    qSort(listOfStratum.begin(), listOfStratum.end(), Stratum2::greaterThan);

    int strata = -1;
    for(LStratum s : listOfStratum){
        int currentStrata = s->getStrata();
        if(strata != currentStrata){
            stratifiedConstants.push_back(QList<LTerm>());
            strata = currentStrata;
        }
        stratifiedConstants.last().append(s->getNode());
    }

    qDebug() << "\n";
    int index = 0;
    for(QList<LTerm> v : stratifiedConstants){
        qDebug() << "Stratum " << index << "\tis comprised of : ";
        for(LTerm c : v){
            qDebug() << c->toString();
        }
        qDebug() << "\n";
        index++;
    }
}




/**
 * @brief KnowledgeBase::buildBaseInitInputDoesPropositions
 */
void KnowledgeBase::buildBaseInitInputDoesPropositions(){
    qDebug() << "\nBASE AND INPUT PROPOSITIONS";


    buildBaseInputEvaluationStructures();


    basePropositions.clear();
    for(LRelation relation : baseRelations){
        basePropositions[relation->toString()] = relation;
    }

    for(LRule rule : baseRules){
        qDebug () << "Base rule " << rule->toString();
        LRelation head = rule->getHead();
        //qDebug () << "Head  " << head->toString();
        QList<LRelation> evaluate = evaluateRelation(head);
        for(LRelation relation : evaluate){
            QString stringRelation = relation->toString();
            if(!basePropositions.contains(stringRelation)){
                basePropositions[stringRelation] = relation;
            }
        }
    }

    qDebug() << "There are " << basePropositions.size() << " base propositions";
    for(QString s : basePropositions.keys()){
        qDebug() << "Base proposition : " << s;
    }

    baseConstants.clear();
    for(LRelation relation : basePropositions.values()){
        LTerm head = relation->getHead();
        baseConstants.insert(head->toString(), head);
    }

    for(LTerm term : baseConstants.values()){
        qDebug() << "Base constant : " << term->toString();
    }

    initPropositions.clear();
    for(LRelation relation : initRelations){
        LRelation relationWithoutInit = relation->clone();
        relationWithoutInit->setQualifier(Logic::LOGIC_QUALIFIER::NO_QUAL);
        initPropositions[relationWithoutInit->toString()] = relationWithoutInit;
    }

    for(LRule rule : initRules){
        qDebug () << "init rule " << rule->toString();
        LRelation head = rule->getHead();
        //qDebug () << "Head  " << head->toString();
        QList<LRelation> evaluate = evaluateRelation(head);
        for(LRelation relation : evaluate){
            QString stringRelation = relation->toString();
            if(!initPropositions.contains(stringRelation)){
                initPropositions[stringRelation] = basePropositions[stringRelation];
            }
        }
    }

    qDebug() << "There are " << initPropositions.size() << " init propositions";
    for(QString s : initPropositions.keys()){
        qDebug() << "init proposition : " << s;
    }

    inputPropositions.clear();
    for(LRelation relation : inputRelations){
        inputPropositions[relation->toString()] = relation;
    }

    for(LRule rule : inputRules){
        qDebug () << "Input rule " << rule->toString();
        LRelation head = rule->getHead();
        //qDebug () << "Head  " << head->toString();
        QList<LRelation> evaluate = evaluateRelation(head);
        for(LRelation relation : evaluate){
            QString stringRelation = relation->toString();
            if(!inputPropositions.contains(stringRelation)){
                inputPropositions[stringRelation] = relation;
            }
        }
    }

    qDebug() << "There are " << inputPropositions.size() << " input propositions";
    for(QString s : inputPropositions.keys()){
        qDebug() << "Input proposition : " << s;
    }

    doesPropositions.clear();
    for(LRelation relation : doesPropositions){
        doesPropositions[relation->toString()] = relation;
    }

    for(LRule rule : doesRules){
        qDebug () << "Does rule " << rule->toString();
        LRelation head = rule->getHead();
        //qDebug () << "Head  " << head->toString();
        QList<LRelation> evaluate = evaluateRelation(head);
        for(LRelation relation : evaluate){
            QString stringRelation = relation->toString();
            if(!doesPropositions.contains(stringRelation)){
                doesPropositions[stringRelation] = relation;
            }
        }
    }

    qDebug() << "There are " << doesPropositions.size() << " does propositions";
    for(QString s : doesPropositions.keys()){
        qDebug() << "Does proposition : " << s;
    }
}

void KnowledgeBase::buildRolePropositions(){
    rolePropositions.clear();
    for(LRelation relation : roleRelations){
        rolePropositions.insert(relation->toString(), relation);
    }

    for(LRule rule : roleRules){
        qDebug () << "Role rule " << rule->toString();
        LRelation head = rule->getHead();
        QList<LRelation> evaluate = evaluateRelation(head);
        for(LRelation relation : evaluate){
            rolePropositions.insert(relation->toString(), relation);
        }
    }
}

const QMap<QString, LRelation>& KnowledgeBase::getInitPropositions() const{
    return initPropositions;
}

const QMap<QString, LRelation>& KnowledgeBase::getRoles() const{
    return rolePropositions;
}


/**
 * @brief KnowledgeBase::printFreeVariables
 * Debug?
 */
void KnowledgeBase::printFreeVariables(){
    qDebug() <<"\nPRINT FREE VARIABLES";

    for(LRule r : ruleList){
        qDebug() << "\nFree variable of rule " << r->toString();
        QSet<QString> freeVar = r->getFreeVariables();
        for(QString s : freeVar){
            qDebug() << "\tVariable " << s;
        }
    }
}





















Stratum2::Stratum2(LTerm n):
    node(n){
    strata = 0;
}

void Stratum2::addDependency(LStratum s){
    dependency.insert(s);
}

void Stratum2::addDependencyNegative(LStratum s){
    dependencyNegative.insert(s);
}

LTerm Stratum2::getNode(){
    return node;
}

int Stratum2::getStrata() const{
    return strata;
}

QString Stratum2::toString(){
    QString answer = node->toString();
    answer = answer + "\thas stratum : " + QString::number(strata) + "\tDependency :";
    for(LStratum s : dependency){
        answer = answer + s->getNode()->toString() + '\t';
    }
    for(LStratum s : dependencyNegative){
        answer = answer + "not (" + s->getNode()->toString() + ")\t";
    }
    return answer;
}

bool Stratum2::updateStrata(){
    bool update = false;

    for(LStratum s : dependency){
        if(strata < s->getStrata()){
            strata = s->getStrata();
            update = true;
        }
    }

    for(LStratum s : dependencyNegative){
        if(strata <= s->getStrata()){
            strata = s->getStrata() + 1;
            update = true;
        }
    }

    return update;
}

bool Stratum2::updateStrataStrongly(){
    bool update = false;

    for(LStratum s : dependency){
        if(strata <= s->getStrata()){
            strata = s->getStrata() + 1;
            update = true;
        }
    }

    for(LStratum s : dependencyNegative){
        if(strata <= s->getStrata()){
            strata = s->getStrata() + 1;
            update = true;
        }
    }

    return update;
}











QList<LRelation> KnowledgeBase::evaluate2(LRelation r){
    //    qDebug() << "\nKnowledgeBase::evaluate(). Relation is : " << r->toString();
    LRelation head = manageRelation(r->clone());
    QList<LRelation> body;
    body.append(manageRelation(r->clone()));

    LRule rule = LRule(new Logic_Rule(head, body));

    QList<LRelation> answer;
    QSet<QString> answerString;

    QList<LRule> possibleAnswers;
    possibleAnswers.append(rule);


    while (!possibleAnswers.isEmpty()) {
                qDebug() << "\nKnowledgeBase::evaluate() possibleAnswer : " << possibleAnswers.last()->toString();
        LRule possibleRule = possibleAnswers.last();
        possibleAnswers.removeLast();

        // If the rule is fully grounded
        if(possibleRule->getHead()->isGround()){
            // If we know it's already true for that grounding, we can discard it
            if(answerString.contains(possibleRule->getHead()->toString())){
                continue;
            }
        }

        // If the body is empty, we have an answer to our evaluation query
        if(possibleRule->isBodyEmpty()){
            //            qDebug() << "We have an answer : " << possibleRule->getHead()->toString();
            answer.append(possibleRule->getHead());
            answerString << possibleRule->getHead()->toString();
            continue;
        }

        // Else, we take the first relation and process it
        QList<LRule> substitutions = ruleSubstitution2(possibleRule);
        possibleAnswers.append(substitutions);
    }
    //    if(answer.isEmpty()){
    //                qDebug() << "No answer";
    //    }
    return answer;
}

QList<LRule> KnowledgeBase::ruleSubstitution2(LRule rule){
        qDebug() << "\nKnowledgeBase::ruleSubstitution " << rule->toString();
    QList<LRule> answer;

    LRelation firstRelation = rule->getBody().first();
    LTerm head = firstRelation->getHead();



    // Substitute with facts
    if(constantToRelationEvaluationMap.contains(head)){
        QList<LRelation> canBeSubstituted = constantToRelationEvaluationMap[head];
        for(LRelation r : canBeSubstituted){
                        qDebug() << "\n    Sub possible with relation " << r->toString();
            URelation unification = URelation(new Unification_Relation(firstRelation, r));
            if(unification->isUnificationValid()){

                LRule tempRule = unification->applySubstitution(rule);
                //                qDebug() << "    Unification is valid and gives : " << tempRule->toString();
                //unification->printSolverResults();

                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), endOfBody)));
                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
                answer.append(partialAnswer);
            }
        }
    }

    // Substitute with temporary facts
    if(constantToTempRelationEvaluationMap.contains(head)){
        QList<LRelation> canBeSubstituted = constantToTempRelationEvaluationMap[head];
        for(LRelation r : canBeSubstituted){
                        qDebug() << "\n    Sub possible with temp relation " << r->toString();
            URelation unification = URelation(new Unification_Relation(firstRelation, r));
            if(unification->isUnificationValid()){

                LRule tempRule = unification->applySubstitution(rule);
                //                qDebug() << "    Unification is valid and gives : " << tempRule->toString();
                //unification->printSolverResults();

                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), endOfBody)));
                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
                answer.append(partialAnswer);
            }
        }
    }

    // Substitute with rule
    if(constantToRuleEvaluationMap.contains(head)){
        QList<LRule> canBeSubstituted = constantToRuleEvaluationMap[head];
        for(LRule r : canBeSubstituted){
                        qDebug() << "\n    Sub possible with rule" << r->toString();
            LRule skolemRule = buildSkolemRule(rule, r);
            //            qDebug() << "    Which is updated in " << skolemRule->toString();
            URelation unification = URelation(new Unification_Relation(firstRelation, skolemRule->getHead()));
            if(unification->isUnificationValid()){
                unification->applySubstitutionInPlace(skolemRule);
                LRule tempRule = unification->applySubstitution(rule);

                //                qDebug() << "    Unification is valid and gives original rule : " << tempRule->toString();
                //                qDebug() << "    and the skolem : " << skolemRule->toString();
                //unification->printSolverResults();

                QList<LRelation> body;
                body.append(skolemRule->getBody());
                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();
                body.append(endOfBody);

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), body)));
                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
                answer.append(partialAnswer);
            }
        }
    }
    return answer;
}






