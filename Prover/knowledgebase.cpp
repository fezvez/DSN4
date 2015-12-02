#include "KnowledgeBase.h"

#include <QDebug>
#include <QLinkedList>


#include "flags.h"

int KnowledgeBase::skolemNumber = 1000;

QRegExp KnowledgeBase::ruleRegExp = QRegExp("<=");
QRegExp KnowledgeBase::whitespaceRegExp = QRegExp("\\s+");
QRegExp KnowledgeBase::wordRegExp = QRegExp("^\\S+$");
QRegExp KnowledgeBase::leftPar = QRegExp("^\\($");
QRegExp KnowledgeBase::rightPar = QRegExp("^\\)$");
QRegExp KnowledgeBase::inputRegExp = QRegExp("(\\(|\\s)input(?![\\w|_|-])");
QRegExp KnowledgeBase::nextRegExp = QRegExp("^next_");
QRegExp KnowledgeBase::newlineRegExp = QRegExp("[\\n\\r]");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KnowledgeBase::KnowledgeBase()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// GETTERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QMap<QString, LTerm> KnowledgeBase::getConstantMap(){
    return constantMap;
}

QMap<LTerm, QList<LRule>> KnowledgeBase::getConstantToRuleMap(){
    return constantToRuleMap;
}

QMap<LTerm, QList<LRelation>> KnowledgeBase::getConstantToRelationMap(){
    return constantToRelationEvaluationMap;
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

QMap<LTerm, int> KnowledgeBase::getArity(){
    return arity;
}

QMap<LTerm, LStratum> KnowledgeBase::getStratumMap(){
    return stratumMap;
}

QList<QList<LTerm>> KnowledgeBase::getStratifiedConstants(){
    return stratifiedConstants;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// SETUP
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KnowledgeBase::setup(QString filename){
    parser.generateHerbrandFromFile(filename);
    setup(parser.getRelations(), parser.getRules());
}

void KnowledgeBase::setup(QList<LRelation> relations, QList<LRule> rules){

    criticalDebug("Knowledge Base Setup Start");

    relationList = relations;
    ruleList = rules;
    
    storeConstants();   // Also cleans the rules and relations
    buildFullConstantMap();
    buildArity();
    
    // By default, use every single rules and relations if ou want to evaluate a query
    // Subclasses of KnowledgeBase can add or remove evaluation rules/relations before making a query
    evaluationRules = ruleList;
    evaluationRelations = relationList;
    buildEvaluationMap();

    printFreeVariables();
    printConstantsWithArity();
    // Optional
    //    generateStratum();

    criticalDebug("Knowledge Base Setup Finished");
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
    qDebug() << "";
    qDebug() << "Ensuring all the Object Constants with the same name share the same address";

    QMap<QString, QString> mapTermToPointer;

    int padSpaceSize = 16;
    for(LRelation relation : relationList){
        LTerm head = relation->getHead();


        if(!mapTermToPointer.contains(head->toString())){
            mapTermToPointer.insert(head->toString(), QString("0x%1").arg((quintptr)head.data(), QT_POINTER_SIZE * 2, 16, QChar('0')));
            qDebug() << "Relation constant " << padSpace(head->toString(), padSpaceSize)  << "address " << head.data();
        }
        Q_ASSERT(mapTermToPointer[head->toString()] == QString("0x%1").arg((quintptr)head.data(), QT_POINTER_SIZE * 2, 16, QChar('0')));
    }
    for(LRule rule : ruleList){
        LTerm head = rule->getHead()->getHead();

        if(!mapTermToPointer.contains(head->toString())){
            mapTermToPointer.insert(head->toString(), QString("0x%1").arg((quintptr)head.data(), QT_POINTER_SIZE * 2, 16, QChar('0')));
            qDebug() << "Relation constant " << padSpace(head->toString(), padSpaceSize) << "address " << head.data();
        }
        Q_ASSERT(mapTermToPointer[head->toString()] == QString("0x%1").arg((quintptr)head.data(), QT_POINTER_SIZE * 2, 16, QChar('0')));

        for(LRelation relation : rule->getBody()){
            LTerm head = relation->getHead();

            if(!mapTermToPointer.contains(head->toString())){
                mapTermToPointer.insert(head->toString(), QString("0x%1").arg((quintptr)head.data(), QT_POINTER_SIZE * 2, 16, QChar('0')));
                qDebug() << "Relation constant " << padSpace(head->toString(), padSpaceSize) << "address " << head.data();
            }
            Q_ASSERT(mapTermToPointer[head->toString()] == QString("0x%1").arg((quintptr)head.data(), QT_POINTER_SIZE * 2, 16, QChar('0')));
        }
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

#ifndef QT_NO_DEBUG
    qDebug() << "";
    qDebug() << "How many rules/relations for a specific Object Constant : ";
    int padSpaceSize = 16;
    for(LTerm head : constantToRelationMap.keys()){
        qDebug() << "Relation " << padSpace(head->toString(), padSpaceSize) << "has nb entries : " << constantToRelationMap[head].size();
    }

    for(LTerm head : constantToRuleMap.keys()){
        qDebug() << "Rule     " << padSpace(head->toString(), padSpaceSize) << "has nb entries : " << constantToRuleMap[head].size();
    }
#endif
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
    
    constantToRuleEvaluationMap.clear();
    for(LRule rule : evaluationRules){
        LTerm head = rule->getHead()->getHead();
        if(!constantToRuleEvaluationMap.contains(head)){
            constantToRuleEvaluationMap.insert(head, QList<LRule>());
        }
        constantToRuleEvaluationMap[head].append(rule);
    }
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
                Q_ASSERT_X(arity[relationHead] == ar, relationHead->toString().toLatin1().data(), "Different arities (bug in the GDL)");
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
            Q_ASSERT_X(arity.contains(headConstant), headConstant->toString().toLatin1().data(), "Object constant not in memory (bug in the software)");
            Q_ASSERT_X(arity[headConstant] == ar, headConstant->toString().toLatin1().data(), "Different arities (bug in the GDL)");
            
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// EVALUATE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * @brief KnowledgeBase::evaluateRelation
 * @param relation
 * @return
 * Takes a relation (optionaly with variables) and returns all the grounded
 * relations that are true
 *
 * Explanation on how it works
 * One day I will be great and use magic sets
 * Currently, that's what I hacked in
 * You get a relation like (legal ?x ?y)
 * You build a rule (legal ?x ?y) :- (legal ?x ?y) (excuse the mix between KIF and HRF)
 * You put this rule in the "Could potentially be a solution"
 *
 * At each step, you pop the first rule in "Could potentially be a solution"
 * You try to unify the first element of the body
 * If you can unify it with a relation (necessarily grounded) just remove the first element
 * If you can unify it with a rule, just replace the first element with the body of that rule
 * If no unification is possible, fail
 * In the end, you should end up with an empty body and the head of the rule has been fully grounded
 */
QList<LRelation> KnowledgeBase::evaluate(QString relation){
    return evaluate(manageRelation(parser.parseRelation(relation)));
}

QList<LRelation> KnowledgeBase::evaluate(LRelation r){
    specialDebug("\n\nKnowledgeBase::EVALUATE(). Relation is : ", r->toString());
    LRelation relation = manageRelation(r);

    QList<LRelation> body;
    body.append(relation);
    LRule rule = LRule(new Logic_Rule(relation, body));

    QList<LRelation> answer;
    QSet<QString> answerString;

    QList<LRule> possibleAnswers;
    possibleAnswers.append(rule);


    while (!possibleAnswers.isEmpty()) {
        LRule possibleRule = possibleAnswers.last();
        possibleAnswers.removeLast();
        specialDebug("KnowledgeBase::evaluate(). Possible Answer under scrutiny: ", possibleRule->toString());

        // If the rule is fully grounded
        if(possibleRule->getHead()->isGround()){
            // If we know it's already true for that grounding, we can discard it
            if(answerString.contains(possibleRule->getHead()->toString())){
                specialDebug("    This possible answer leads to an already known answer. We stop there");
                continue;
            }
        }

        // If the body is empty, we have an answer to our evaluation query
        if(possibleRule->isBodyEmpty()){
            specialDebug("    We have a new answer : ", possibleRule->getHead()->toString());
            answer.append(possibleRule->getHead());
            answerString << possibleRule->getHead()->toString();
            continue;
        }

        // Else, we take the first relation and process it
        QList<LRule> substitutions = ruleSubstitution(possibleRule);
        possibleAnswers.append(substitutions);
    }

    specialDebug("\n\nKnowledgeBase::EVALUATE() finished");
    if(answer.isEmpty()){
        specialDebug("No answer");
    }
    else{
        for(QString string : answerString){
            specialDebug("    ", string);
        }
    }
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
    specialDebug("\n\nKnowledgeBase::ruleSubstitution ", rule->toString());

    QList<LRule> answer;

    LRelation firstRelation = rule->getBody().first();
    LTerm head = firstRelation->getHead();

    specialDebug("The first relation is ", firstRelation->toString());

    // "Not" special case
    if(firstRelation->isNegation()){
        LRelation nonNegativeRelation = Logic_Relation::clone(firstRelation);
        nonNegativeRelation->setNegation(false);
        specialDebug("Negation, trying to solve a subproblem ", nonNegativeRelation->toString());
        QList<LRelation> subproblemAnswer = evaluate(nonNegativeRelation);
        if(subproblemAnswer.size() == 0){
            specialDebug("Negation Subproblem ", nonNegativeRelation->toString(), " is false");
            QList<LRelation> endOfBody = rule->getBody();
            endOfBody.removeFirst();

            LRule partialAnswer = manageRule(LRule(new Logic_Rule(rule->getHead(), endOfBody)));
            answer.append(partialAnswer);
        }
        else{
            specialDebug("Negation Subproblem ", nonNegativeRelation->toString(), " is true, we stop here");
        }
        return answer;
    }

    // Distinct special case
    if(head->toString() == QString("distinct")){
        QList<LTerm> body = firstRelation->getBody();

        if(body[0]->toString() != body[1]->toString()){
            QList<LRelation> endOfBody = rule->getBody();
            endOfBody.removeFirst();

            LRule partialAnswer = manageRule(LRule(new Logic_Rule(rule->getHead(), endOfBody)));
            answer.append(partialAnswer);

            specialDebug("Distinct : The constants are indeed distinct");
        }
        else{
            specialDebug("Distinct : The constants are identical, we stop here");
        }
        return answer;

    }

    // Otherwise
    // Substitute with facts
    if(constantToRelationEvaluationMap.contains(head)){
        QList<LRelation> canBeSubstituted = constantToRelationEvaluationMap[head];
        for(LRelation r : canBeSubstituted){
            specialDebug("Sub possible with relation ", r->toString());
            URelation unification = URelation(new Unification_Relation(firstRelation, r));
            if(unification->isUnificationValid()){

                LRule tempRule = unification->applySubstitution(rule);
                specialDebug("    Unification is valid and gives : ", tempRule->toString());
                //unification->printSolverResults();

                // Because we unified with a relation, we know the first element of the body is valid
                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), endOfBody)));
                specialDebug("    Partial Answer : ", partialAnswer->toString());
                answer.append(partialAnswer);
            }
            else{
                specialDebug("    Unification fails with relation", r->toString());
            }
        }
    }

    // Substitute with temporary facts
    if(constantToTempRelationEvaluationMap.contains(head)){
        QList<LRelation> canBeSubstituted = constantToTempRelationEvaluationMap[head];
        for(LRelation r : canBeSubstituted){
            specialDebug("Sub possible with temp relation ", r->toString());
            URelation unification = URelation(new Unification_Relation(firstRelation, r));
            if(unification->isUnificationValid()){

                LRule tempRule = unification->applySubstitution(rule);
                specialDebug("    Unification is valid and gives : ", tempRule->toString());
                //unification->printSolverResults();

                // Because we unified with a relation, we know the first element of the body is valid
                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), endOfBody)));
                specialDebug("    Partial Answer : ", partialAnswer->toString());
                answer.append(partialAnswer);
            }
            else{
                specialDebug("    Unification fails with relation", r->toString());
            }
        }
    }

    // Substitute with rule
    if(constantToRuleEvaluationMap.contains(head)){
        QList<LRule> canBeSubstituted = constantToRuleEvaluationMap[head];
        for(LRule r : canBeSubstituted){
            specialDebug("Sub possible with rule", r->toString());
            LRule skolemRule = buildSkolemRule(rule, r);
            specialDebug("    Which is skolemized in ", skolemRule->toString());
            URelation unification = URelation(new Unification_Relation(firstRelation, skolemRule->getHead()));
            if(unification->isUnificationValid()){
                unification->applySubstitutionInPlace(skolemRule);
                LRule tempRule = unification->applySubstitution(rule);

                specialDebug("    Unification is valid and gives original rule : ", tempRule->toString());
                specialDebug("    and the skolem : ", skolemRule->toString());
                //unification->printSolverResults();

                // We replace the first element of the original body with the full body of the rule
                QList<LRelation> body;
                body.append(skolemRule->getBody());
                QList<LRelation> endOfBody = tempRule->getBody();
                endOfBody.removeFirst();
                body.append(endOfBody);

                LRule partialAnswer = manageRule(LRule(new Logic_Rule(tempRule->getHead(), body)));
                specialDebug("    Partial Answer : ", partialAnswer->toString());
                answer.append(partialAnswer);
            }
            else{
                specialDebug("    Unification fails with rule", r->toString());
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


LRelation KnowledgeBase::getRelationfromString(QString s){
    return manageRelation(parser.parseRelation(s));
}

LTerm KnowledgeBase::getTermFromString(QString s){
    return manageTerm(parser.parseTerm(s));
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// MANAGE TO SHARE OBJECTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
//// STRATUM
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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










////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// DEBUGGING TOOLS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KnowledgeBase::printRuleEvaluation(){
    qDebug() << "printRuleEvaluation()";
    for(LRule rule : evaluationRules){
        qDebug() << rule->toString() << " with head " << rule->getHead()->getHead()->toString();
    }
}

void KnowledgeBase::printRelationEvaluation(){
    qDebug() << "printRelationEvaluation()";
    for(LRelation relation : evaluationRelations){
        qDebug() << relation->toString() << " with head " << relation->getHead()->toString() << " at address " << relation->getHead().data();
    }
}

void KnowledgeBase::printTempRelationEvaluation(){
    qDebug() << "printTempRelationEvaluation()";
    for(LRelation relation : evaluationTempRelations){
        qDebug() << relation->toString() << " with head " << relation->getHead()->toString() << " at address " << relation->getHead().data();
    }
}

/**
 * @brief KnowledgeBase::checkArity
 * @param relation
 * Debugging tool
 */
void KnowledgeBase::checkArity(LRelation relation){
        qDebug() << "Check arity " << relation->toString();
    LTerm relationHead = relation->getHead();
    int ar = relation->getBody().size();


    // HACK
    //Q_ASSERT(arity.contains(relationHead));
    if(!arity.contains(relationHead)){
        arity[relationHead] = ar;
    }

    Q_ASSERT(arity[relationHead] == ar);
}

/**
 * @brief KnowledgeBase::printFreeVariables
 * Debug?
 */
void KnowledgeBase::printFreeVariables(){
#ifndef QT_NO_DEBUG
    qDebug() <<"\n\nPrint Free Variables\n";

    for(LRule r : ruleList){
        qDebug() << "Free variable of rule " << r->toString();
        QSet<QString> freeVar = r->getFreeVariables();
        for(QString s : freeVar){
            qDebug() << "\tVariable " << s;
        }
    }
#endif
}




void KnowledgeBase::printConstantsWithArity(){
#ifndef QT_NO_DEBUG
    int padSpaceSize = 16;
    qDebug() << "\n\nLIST OF CONSTANTS";
    for(LTerm constant : objectConstantSet){
        qDebug() << "Object constant : " << padSpace(constant->toString(), padSpaceSize) << " with address : " << constant.data();
    }
    for(LTerm constant : relationConstantSet){
        qDebug() << "Relation constant : " << padSpace(constant->toString(), padSpaceSize) << " with address : " << constant.data()<< "\tand arity " << arity[constant];
    }
    for(LTerm constant : functionConstantSet){
        qDebug() << "Function constant : " << padSpace(constant->toString(), padSpaceSize) << " with address : " << constant.data()<< "\tand arity " << arity[constant];
    }
#endif
}

