#include "KnowledgeBase.h"

#include <QDebug>
#include <QLinkedList>

#include "Unification/unification_relation.h"
#include "flags.h"

int KnowledgeBase::skolemNumber = 1000;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTOR, GETTERS, SETTERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KnowledgeBase::KnowledgeBase()
{
    createRegExp();
}

QMap<QString, LTerm> KnowledgeBase::getConstantMap(){
    return constantMap;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTOR, GETTERS, SETTERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QMap<LTerm, QList<LRule>> KnowledgeBase::getConstantToRuleMap(){
    return constantToRuleMap;
}

QMap<LTerm, QList<LRelation>> KnowledgeBase::getConstantToRelationMap(){
    return constantToRelationEvaluationMap;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// SETUP
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void KnowledgeBase::setup(QList<LRelation> relations, QList<LRule> rules){
#ifndef QT_NO_DEBUG
    qDebug() <<"\n\nKNOWLEDGE BASE SETUP";
#endif
//    relationList.clear();
//    ruleList.clear();
//    for(LRelation relation : relations){
//        relationList.append(relation->clone());
//    }
//    for(LRule rule : rules){
//        ruleList.append(rule->clone());
//    }
    relationList = relations;
    ruleList = rules;
    
    storeConstants();   // Also cleans the rules and relations
    buildFullConstantMap();
    
    evaluationRules = ruleList;
    evaluationRelations = relationList;
    buildEvaluationMap();
    
    buildArity();
#ifndef QT_NO_DEBUG
    printConstantsWithArity();
#endif
    // Optional
    //    printFreeVariables();
    //    generateStratum();
    
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
    //    qDebug() << "Store constants";
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

void KnowledgeBase::buildFullConstantMap(){
    constantToRelationMap.clear();
    for(LRelation relation : relationList){
        LTerm head = relation->getHead();
        if(!constantToRelationMap.contains(head)){
            constantToRelationMap.insert(head, QList<LRelation>());
        }
        constantToRelationMap[head].append(relation);
    }
    
    constantToRuleMap.clear();
    for(LRule rule : ruleList){
        LTerm head = rule->getHead()->getHead();
        if(!constantToRuleMap.contains(head)){
            constantToRuleMap.insert(head, QList<LRule>());
        }
        constantToRuleMap[head].append(rule);
    }
}

void KnowledgeBase::buildEvaluationMap(){
    constantToRelationEvaluationMap.clear();
    for(LRelation relation : evaluationRelations){
        LTerm head = relation->getHead();
        if(!constantToRelationEvaluationMap.contains(head)){
            constantToRelationEvaluationMap.insert(head, QList<LRelation>());
        }
        constantToRelationEvaluationMap[head].append(relation);
    }
    
    //    for(LTerm key : constantToRelationEvaluationMap.keys()){
    //        qDebug() << "Relation " << key->toString() << " has nb entries : " << constantToRelationEvaluationMap[key].size();
    //    }
    
    constantToRuleEvaluationMap.clear();
    for(LRule rule : evaluationRules){
        LTerm head = rule->getHead()->getHead();
        if(!constantToRuleEvaluationMap.contains(head)){
            constantToRuleEvaluationMap.insert(head, QList<LRule>());
        }
        constantToRuleEvaluationMap[head].append(rule);
    }
    
    //    for(LTerm key : constantToRuleEvaluationMap.keys()){
    //        qDebug() << "Rule " << key->toString() << " has nb entries : " << constantToRuleEvaluationMap[key].size();
    //    }
}

/**
 * @brief KnowledgeBase::buildArity
 * I thought this would be useful, but it's just a debugging tool as of now
 *
 */
void KnowledgeBase::buildArity(){
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
//    qDebug() << "Cheack arity " << relation->toString();
    LTerm relationHead = relation->getHead();
    int ar = relation->getBody().size();
    Q_ASSERT(arity.contains(relationHead));
    Q_ASSERT(arity[relationHead] == ar);
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// EVALUATE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief KnowledgeBase::evaluateRelation
 * @param relation
 * @return
 * Takes a relation (optionnaly with variables) and returns all the grounded
 * relations that are true (given the current )
 */


QList<LRelation> KnowledgeBase::evaluate(LRelation r){
#ifndef QT_NO_DEBUG
//    qDebug() << "KnowledgeBase::evaluate(). Relation is : " << r->toString();
#endif
    LRelation relation = manageRelation(r);
    
    QList<LRelation> body;
    body.append(relation);
    LRule rule = LRule(new Logic_Rule(relation, body));
    
    QList<LRelation> answer;
    QSet<QString> answerString;
    
    QList<LRule> possibleAnswers;
    possibleAnswers.append(rule);
    
    
    while (!possibleAnswers.isEmpty()) {
//                qDebug() << "\tKnowledgeBase::evaluate() possibleAnswer : " << possibleAnswers.last()->toString();
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
//#ifndef QT_NO_DEBUG
//    qDebug() << "\nKnowledgeBase::ruleSubstitution " << rule->toString();
//#endif
    QList<LRule> answer;
    
    LRelation firstRelation = rule->getBody().first();
    LTerm head = firstRelation->getHead();
    
    //
    if(firstRelation->isNegation()){
        LRelation nonNegativeRelation = Logic_Relation::clone(firstRelation);
        nonNegativeRelation->setNegation(false);
//        qDebug() << "Negation, trying to solve a subproblem " << nonNegativeRelation->toString();
        QList<LRelation> subproblemAnswer = evaluate(nonNegativeRelation);
        if(subproblemAnswer.size() == 0){
//            qDebug() << "Subproblem " << nonNegativeRelation->toString() << " is false";
            QList<LRelation> endOfBody = rule->getBody();
            endOfBody.removeFirst();
            
            LRule partialAnswer = manageRule(LRule(new Logic_Rule(rule->getHead(), endOfBody)));
            answer.append(partialAnswer);
        }
        else{
//            qDebug() << "Subproblem " << nonNegativeRelation->toString() << " is true, we stop here";
        }
        return answer;
    }
    
    if(head->toString() == QString("distinct")){
        QList<LTerm> body = firstRelation->getBody();
        if(body[0]->toString() != body[1]->toString()){
            QList<LRelation> endOfBody = rule->getBody();
            endOfBody.removeFirst();
            
            LRule partialAnswer = manageRule(LRule(new Logic_Rule(rule->getHead(), endOfBody)));
            answer.append(partialAnswer);
        }
        else{
            
        }
        return answer;
        
    }
    
    
    // Substitute with facts
    if(constantToRelationEvaluationMap.contains(head)){
        QList<LRelation> canBeSubstituted = constantToRelationEvaluationMap[head];
        for(LRelation r : canBeSubstituted){
            //            qDebug() << "\n    Sub possible with relation " << r->toString();
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
            //            qDebug() << "\n    Sub possible with temp relation " << r->toString();
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
            //            qDebug() << "\n    Sub possible with rule" << r->toString();
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
 * It's necessary to rename variables in the body of this 2nd rule to prevent name clashing
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

void KnowledgeBase::loadAdditionalTempRelations(const QVector<LRelation> contents){
    for(LRelation relation : contents){
        evaluationTempRelations.append(relation);
        
        LTerm head = relation->getHead();
        if(!constantToTempRelationEvaluationMap.contains(head)){
            constantToTempRelationEvaluationMap.insert(head, QList<LRelation>());
        }
        constantToTempRelationEvaluationMap[head].append(relation);
    }
}




// Is just a debugging tool
// It is not needed for the computation
// It is just to check that the GDL rules are well formed
void KnowledgeBase::generateStratum(){
    stratumMap.clear();
    stratifiedConstants.clear();

    // Put everything in memory
    for(LTerm constant : relationConstantSet){
        stratumMap.insert(constant, LStratum(new Stratum(constant)));
    }
    
    // Create dependency
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
    
    while(update && nbStrongIteration<10000){
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
    
    
    
    QList<LStratum> listOfStratum = stratumMap.values();
    qSort(listOfStratum.begin(), listOfStratum.end(), Stratum::greaterThan);
    
    int strata = -1;
    for(LStratum s : listOfStratum){
        int currentStrata = s->getStrata();
        if(strata != currentStrata){
//            qDebug() << "Pushed a new strata level";
//            qDebug() << "Strata = " << strata;
//            qDebug() << "currentStrata = " << currentStrata;
            stratifiedConstants.push_back(QList<LTerm>());
            strata = currentStrata;


        }
        stratifiedConstants.last().append(s->getNode());
    }
    
#ifndef QT_NO_DEBUG
    qDebug() << "GENERATE STRATUM";
    qDebug() << "Nb strong iterations : " << nbStrongIteration;
    qDebug() << "Nb iterations : " << nbIteration;
    int index = 0;
    for(QList<LTerm> v : stratifiedConstants){
        qDebug() << "Stratum " << index << "is comprised of : ";
        for(LTerm c : v){
            qDebug() << "\t" << c->toString();
        }
        index++;
    }

    for(LStratum stratum : listOfStratum){
        qDebug() << "Stratum " << stratum->toString();
    }
    qDebug() << "\n";
#endif
}



QMap<LTerm, LStratum> KnowledgeBase::getStratumMap(){
    return stratumMap;
}

QList<QList<LTerm>> KnowledgeBase::getStratifiedConstants(){
    return stratifiedConstants;
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




void KnowledgeBase::printConstantsWithArity(){
    qDebug() << "\n\nLIST OF CONSTANTS";
    for(LTerm constant : objectConstantSet){
        qDebug() << "Object constant : " << constant->toString() << "\twith address : " << constant.data();
    }
    for(LTerm constant : relationConstantSet){
        qDebug() << "Relation constant : " << constant->toString() << "\twith address : " << constant.data()<< "\tand arity " << arity[constant];
    }
    for(LTerm constant : functionConstantSet){
        qDebug() << "Function constant : " << constant->toString() << "\twith address : " << constant.data()<< "\tand arity " << arity[constant];
    }
}

LTerm KnowledgeBase::getTermFromString(QString s){
    return manageTerm(parser.parseTerm(s));
}

LRelation KnowledgeBase::getRelationfromString(QString s){
    return manageRelation(parser.parseRelation(s));
}














Stratum::Stratum(LTerm n):
    node(n){
    strata = 0;
}

void Stratum::addDependency(LStratum s){
    dependency.insert(s);
}

void Stratum::addDependencyNegative(LStratum s){
    dependencyNegative.insert(s);
}

LTerm Stratum::getNode(){
    return node;
}

int Stratum::getStrata() const{
    return strata;
}

QString Stratum::toString(){
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

bool Stratum::updateStrata(){
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

bool Stratum::updateStrataStrongly(){
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











//QList<LRelation> KnowledgeBase::evaluate2(LRelation r){
//    //    qDebug() << "\nKnowledgeBase::evaluate(). Relation is : " << r->toString();
//    LRelation head = manageRelation(r->clone());
//    QList<LRelation> body;
//    body.append(manageRelation(r->clone()));

//    LRule rule = LRule(new Logic_Rule(head, body));

//    QList<LRelation> answer;
//    QSet<QString> answerString;

//    QList<LRule> possibleAnswers;
//    possibleAnswers.append(rule);


//    while (!possibleAnswers.isEmpty()) {
//        qDebug() << "\nKnowledgeBase::evaluate() possibleAnswer : " << possibleAnswers.last()->toString();
//        LRule possibleRule = possibleAnswers.last();
//        possibleAnswers.removeLast();

//        // If the rule is fully grounded
//        if(possibleRule->getHead()->isGround()){
//            // If we know it's already true for that grounding, we can discard it
//            if(answerString.contains(possibleRule->getHead()->toString())){
//                continue;
//            }
//        }

//        // If the body is empty, we have an answer to our evaluation query
//        if(possibleRule->isBodyEmpty()){
//            //            qDebug() << "We have an answer : " << possibleRule->getHead()->toString();
//            answer.append(possibleRule->getHead());
//            answerString << possibleRule->getHead()->toString();
//            continue;
//        }

//        // Else, we take the first relation and process it
//        QList<LRule> substitutions = ruleSubstitution2(possibleRule);
//        possibleAnswers.append(substitutions);
//    }
//    //    if(answer.isEmpty()){
//    //                qDebug() << "No answer";
//    //    }
//    return answer;
//}

//QList<LRule> KnowledgeBase::ruleSubstitution2(LRule rule){
//    qDebug() << "\nKnowledgeBase::ruleSubstitution " << rule->toString();
//    QList<LRule> answer;

//    LRelation firstRelation = rule->getBody().first();
//    LTerm head = firstRelation->getHead();



//    // Substitute with facts
//    if(constantToRelationEvaluationMap.contains(head)){
//        QList<LRelation> canBeSubstituted = constantToRelationEvaluationMap[head];
//        for(LRelation r : canBeSubstituted){
//            qDebug() << "\n    Sub possible with relation " << r->toString();
//            URelation unification = URelation(new Unification_Relation(firstRelation, r));
//            if(unification->isUnificationValid()){

//                LRule tempRule = unification->applySubstitution(rule);
//                //                qDebug() << "    Unification is valid and gives : " << tempRule->toString();
//                //unification->printSolverResults();

//                QList<LRelation> endOfBody = tempRule->getBody();
//                endOfBody.removeFirst();

//                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), endOfBody)));
//                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
//                answer.append(partialAnswer);
//            }
//        }
//    }

//    // Substitute with temporary facts
//    //    if(constantToTempRelationEvaluationMap.contains(head)){
//    //        QList<LRelation> canBeSubstituted = constantToTempRelationEvaluationMap[head];
//    //        for(LRelation r : canBeSubstituted){
//    //                        qDebug() << "\n    Sub possible with temp relation " << r->toString();
//    //            URelation unification = URelation(new Unification_Relation(firstRelation, r));
//    //            if(unification->isUnificationValid()){

//    //                LRule tempRule = unification->applySubstitution(rule);
//    //                //                qDebug() << "    Unification is valid and gives : " << tempRule->toString();
//    //                //unification->printSolverResults();

//    //                QList<LRelation> endOfBody = tempRule->getBody();
//    //                endOfBody.removeFirst();

//    //                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), endOfBody)));
//    //                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
//    //                answer.append(partialAnswer);
//    //            }
//    //        }
//    //    }

//    // Substitute with rule
//    if(constantToRuleEvaluationMap.contains(head)){
//        QList<LRule> canBeSubstituted = constantToRuleEvaluationMap[head];
//        for(LRule r : canBeSubstituted){
//            qDebug() << "\n    Sub possible with rule" << r->toString();
//            LRule skolemRule = buildSkolemRule(rule, r);
//            //            qDebug() << "    Which is updated in " << skolemRule->toString();
//            URelation unification = URelation(new Unification_Relation(firstRelation, skolemRule->getHead()));
//            if(unification->isUnificationValid()){
//                unification->applySubstitutionInPlace(skolemRule);
//                LRule tempRule = unification->applySubstitution(rule);

//                //                qDebug() << "    Unification is valid and gives original rule : " << tempRule->toString();
//                //                qDebug() << "    and the skolem : " << skolemRule->toString();
//                //unification->printSolverResults();

//                QList<LRelation> body;
//                body.append(skolemRule->getBody());
//                QList<LRelation> endOfBody = tempRule->getBody();
//                endOfBody.removeFirst();
//                body.append(endOfBody);

//                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), body)));
//                //                qDebug() << "    Partial Answer : " << partialAnswer->toString();
//                answer.append(partialAnswer);
//            }
//        }
//    }
//    return answer;
//}


void KnowledgeBase::printRuleEvaluation(){
    
}

void KnowledgeBase::printRelationEvaluation(){
    
}

void KnowledgeBase::printTempRelationEvaluation(){
    qDebug() << "printTempRelationEvaluation()";
    for(LRelation relation : evaluationTempRelations){
        qDebug() << relation->toString() << " with head " << relation->getHead()->toString() << " at address " << relation->getHead().data();
    }
}

QList<LRelation> KnowledgeBase::getEvaluationRelations(){
    return evaluationRelations;
}

QList<LRule> KnowledgeBase::getEvaluationRules(){
    return evaluationRules;
}

QList<LRelation> KnowledgeBase::getEvaluationTempRelations(){
    return evaluationTempRelations;
}

QMap<LTerm, QList<LRelation>> KnowledgeBase::getConstantToTempRelationEvaluationMap(){
    return constantToTempRelationEvaluationMap;
}



