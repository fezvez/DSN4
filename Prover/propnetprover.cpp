#include "propnetprover.h"

#include "../kifloader.h"

#include <QStringList>
#include <QDebug>
#include <QFile>

#include "../Logic/logic_rule.h"
#include "../Unification/unification_relation.h"
#include "../PropNet/componentand.h"
#include "../PropNet/componentor.h"
#include "../PropNet/componentnot.h"

#include <chrono>
#include <thread>

PropnetProver::PropnetProver()
{
    propositionDatabase = PDatabase(new PropositionDatabase());
}

PropnetProver::~PropnetProver()
{

}

void PropnetProver::setup(QList<LRelation> relations, QList<LRule> rules){
    GDLProver::setup(relations, rules);

#ifndef QT_NO_DEBUG
    qDebug() << "Propnet setup";
#endif
    KnowledgeBase::generateStratum();

    //        qDebug() << "Propnet setup";
    generatePropnet();
    initializeSavedValuesMap();
    cleanPropnet();
    buildBaseDoesPropositions();
    buildMapNextPropositionToBaseProposition();

    buildDepthChargeMembers();
    buildOptimizedPropnet();
}

void PropnetProver::loadKifFile(QString filename){
    KifLoader kifLoader(nullptr, filename);
    QStringList sL = kifLoader.runSynchronously();

    Parser parser;
    parser.loadKif(sL);

    setup(parser.getRelations(), parser.getRules());
}

/***
 *
 * Propnet generation algorithm
 *
 * Proceeds by stratum level
 * It is guaranteed that level 0 only includes relations (and no rules)
 *
 * When you have a rule, it is guaranteed that all relations in the body have lower or equal strata
 *
 * At stratum level L, you have N relation constant
 *
 * Convert all the relations in propositions
 * For each relation constant
 *  For each rule
 *    Generate all possible unifications (nontrivial)
 *
 *
 *
 * Generate all unifications :
 *  Find first ungrouded relation
 *  Unify it with all the propositions you have so far
 *  Recursively call the function on all the rules obtained
 *
 * */

void PropnetProver::generatePropnet(){
#ifndef QT_NO_DEBUG
    qDebug() << "GENERATE PROPNET";
#endif


    propositionDatabase->clear();
    alreadyInDatabase.clear();
    //    components.clear();


    QList<QList<LTerm> > stratifiedConstants = getStratifiedConstants();

    // I wonder whether I could have simply iterated over KnowledgeBase::relationConstantSet

    // Add all the relations
    for(auto list : stratifiedConstants){
        for(LTerm relationConstant : list){
            //            qDebug() << "Relation constant " << relationConstant->toString();
            if(relationConstant->toString() == "input"){
                continue;
            }


            if(constantToRelationMap.contains(relationConstant)){
                //                qDebug() << "Relation constant " << relationConstant->toString() << " is indeed insde";
                QList<LRelation> sublist = constantToRelationMap[relationConstant];
                //                qDebug() << "Relation constant " << relationConstant->toString() << " nb " << sublist.size();
                for(LRelation relation : sublist){
                    if(relation->getQualifier() == Logic::LOGIC_QUALIFIER::INIT){
                        //                        qDebug() << "Skipping init relation " << relation->toString();
                        continue;
                    }
                    propositionDatabase->getProposition(relation);
                }
            }
            else{
                // There are no relations, only rules, with the relation constant as "head"
                LTerm term = constantMap[relationConstant->toString()];
                if(!constantToRuleMap.contains(term)){
                    if(term->toString() == "distinct"){
                        continue;
                    }
                    qDebug() << "Bug with relation constant that is in no rule and no relation : " << term->toString();
                    Q_ASSERT(false);
                }
            }
        }
    }



    // Add all the rules

    // Do it by strata level
    int strataLevel = 0;
    for(auto list : stratifiedConstants){

        //        qDebug() << "\n\nNEW STRATA of size " << list.size() << " at level " << strataLevel;
        strataLevel++;

        // Compute all the groundings
        // TODO
        //  - We ground all the rules of strata X each time, useless
        //  - If a grounding is true, no need to analyze it next time

        // We are in a certain level of the strata
        // You may have to iterate in the case of recursion
        bool update;
        while(true){
            update = false;

            for(LTerm relationConstant : list){
                if(relationConstant->toString() == "input"){
                    continue;
                }

                //                qDebug() << "Relation constant " << relationConstant->toString();
                // For each relation constant, compute all the rules
                if(constantToRuleMap.contains(relationConstant)){
                    for(LRule rule : constantToRuleMap[relationConstant]){
                        //                        qDebug() << "GROUNDING FUNDAMENTAL rule " << rule->toString();
                        QList<LRule> groundedRules = getGrounding(rule);
                        //                        qDebug() << "Nb groundings : " << groundedRules.size();
                        for(LRule rule : groundedRules){
                            //                            if(rule->getHead()->getHead()->toString() == "legal"){
                            //                                qDebug() << "GNAAAR";
                            //                                qDebug() << "\t\tGrounded rule " << rule->toString();
                            //                                qDebug() << rule->getHead()->getBody()[0]->toString() << " address " << rule->getHead()->getBody()[0].data();
                            //                            }
                            //                            qDebug() << "\t\tGrounded rule " << rule->toString();
                            bool updated = addRuleToDatabase(rule);
                            //                            qDebug() << "Updated "<< updated;
                            update = update || updated;
                        }
                    }
                }
            }
            if(!update){
                break;
            }
        }
    }

#ifndef QT_NO_DEBUG
    propositionDatabase->printAllPropositions();
    qDebug() << "\n";
#endif
}



QList<LRule> PropnetProver::getGrounding(LRule rule){
    QList<LRule> answer;

    if(rule->isGround()){
        //        if(rule->getHead()->getHead()->toString() == "cellOpen"){
        //            qDebug() << " next_cellXYZ rule " << rule->toString();
        //        }
        for(LRelation relation : rule->getBody()){
            // Special case for distinct
            if(relation->getHead()->toString() == "distinct"){
                QList<LTerm> body = relation->getBody();
                if(body[0]->toString() == body[1]->toString()){
                    return answer;
                }
                continue;
            }


            QString shortName = relation->toStringWithNoQualifier();
            //            if(rule->getHead()->getHead()->toString() == "cellOpen"){
            //                qDebug() << " shortName " << shortName;
            //            }
            if(!propositionDatabase->contains(shortName)){
                return answer;
            }
        }
        answer.append(rule);
        return answer;
    }

    // Grab the first ungrounded relation
    QList<LRelation> body = rule->getBody();
    LRelation firstUngroundedRelation;
    for(LRelation relation : body){
        if(!relation->isGround()){
            firstUngroundedRelation = relation;
            break;
        }
    }
    Q_ASSERT(firstUngroundedRelation);

    //    qDebug() << "First ungrounded relation " << firstUngroundedRelation->toString();

    LTerm relationConstant = firstUngroundedRelation->getHead();

    PRelationDatabase relationDatabase = propositionDatabase->getRelationDatabase(relationConstant->toString());
    QList<PProposition> possibleSubstitutes = relationDatabase->getAllPropositions();

    //    qDebug() << firstUngroundedRelation->toString() << " can be subbed by " << possibleSubstitutes.size();
    for(PProposition propSubstitute : possibleSubstitutes){
        //        qDebug() << "Possible sub " << propSubstitute->getName();
        LRelation possibleSubstitute = propSubstitute->getRelation();

        Unification_Relation unification = Unification_Relation(firstUngroundedRelation, possibleSubstitute);
        //        qDebug() << "Unification " << unification.toString();
        if(unification.isUnificationValid()){
            LRule newRule = unification.applySubstitution(rule);
            LRule newRule2 = manageRule(newRule);
            QString nameBefore = newRule2->toString();
            newRule2->rebuildName();
            // TODO eliminate this rebuildname() if it proves useless
            Q_ASSERT(nameBefore == newRule2->toString());
            QList<LRule> answersFromNewRule = getGrounding(newRule2);
            answer += answersFromNewRule;
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() << "Grounding rule " << rule->toString() << " with nb possible substitutions : " << answer.size();
#endif
    return answer;
}

bool PropnetProver::addRuleToDatabase(LRule rule){
    // line x :- (cell 1 1 x) (cell 1 2 x) (cell 1 3 x)


    QString ruleString = rule->toString();

    if(alreadyInDatabase.contains(ruleString)){
        return false;
    }


    alreadyInDatabase.insert(ruleString);

    // Find the proposition "line x"
    LRelation head = rule->getHead();
    PProposition headProposition = propositionDatabase->getProposition(head);

    // Create the component
    PCAnd fundamentalComponent = PCAnd(new ComponentAnd());
    for(LRelation relation : rule->getBody()){
        //        qDebug() << "Adding to the AND component " << relation->toStringWithNoQualifier();

        if(relation->isNegation()){
            //            qDebug() << "It's negation";
            PCNot cnot = PCNot(new ComponentNot());
            LRelation nonNegationRelation = Logic_Relation::clone(relation);
            nonNegationRelation->setNegation(false);
            PProposition tempProp = propositionDatabase->getProposition(nonNegationRelation);
            cnot->addInput(tempProp);

            fundamentalComponent->addInput(cnot);
            continue;
        }
        if(relation->getHead()->toString() == "distinct"){
            //            qDebug() << "It's distinct";
            //            QList<LTerm> body = relation->getBody();
            //            if(body[0]->toString() == body[1]->toString()){
            //                return false;
            //            }
            continue;
        }

        // Else
        fundamentalComponent->addInput(propositionDatabase->getProposition(relation));
    }


    // Find if it has an OR as single input
    // If not, put the OR in place


    if(!headProposition->hasInput()){
        //        qDebug() << "Head had no OR input";
        PCOr orComponent = PCOr(new ComponentOr());
        orComponent->addInput(fundamentalComponent);
        headProposition->addInput(orComponent.staticCast<Component>());
    }else{
        PCOr orComponent = headProposition->getSingleInput().dynamicCast<ComponentOr>();
        Q_ASSERT(orComponent);
        if(orComponent){
            //            qDebug() << "Head has OR input";
            orComponent->addInput(fundamentalComponent);
        }
        else{
            Q_ASSERT(false);
            qDebug() << "BUG : Head has strange as fuck input";
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() << "Adding rule to DB : " << rule->toString();
#endif

    return true;
}


void PropnetProver::initializeSavedValuesMap(){
    hasCorrectSavedValue.clear();
    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
        hasCorrectSavedValue[proposition] = false;
    }
}


/***
                                 * CLEAN PROPNET
                                 */

void PropnetProver::cleanPropnet(){
#ifndef QT_NO_DEBUG
    qDebug() << "CLEAN PROPNET";
#endif

    // Remove the input of does and base
    auto propositionMap = propositionDatabase->getPropositionsMap();
    auto end = propositionMap.end();
    for(auto it = propositionMap.begin(); it != end; ++it){
        PProposition proposition = it.value();
        QList<PComponent> clearInput;
        QString relationHead = proposition->getRelation()->getHead()->toString();
        if(relationHead == "does"){
            proposition->setInputs(clearInput);
            continue;
        }
        if(proposition->getRelation()->getQualifier() == Logic::BASE){
            proposition->setInputs(clearInput);
            continue;
        }
    }

    // Remove the ComponentAnd and ComponentOr who have a single input
    bool iterateAgain;
    do{
        iterateAgain = cleanPropnetIteration();
    }while(iterateAgain);

    buildComponents();

#ifndef QT_NO_DEBUG
    qDebug() << "CLEAN PROPNET ENDED";

    for(auto it = propositionMap.begin(); it != end; ++it){
        PProposition proposition = it.value();
        if(!proposition->hasInput()){
            qDebug() << "Proposition " << proposition->getName() << " has no input";
            continue;
        }
        qDebug() << proposition->printFullDebug();
        qDebug() << "Proposition " << proposition->getName() << " has input proposition : ";
        for(PProposition inputProp : proposition->getInputPropositions()){
            qDebug() << "\t" << inputProp->getName();
        }
    }
    qDebug() << "\n";
#endif
}

bool PropnetProver::cleanPropnetIteration(){
    bool update = false;

    buildComponents();

    for(PComponent component : components){
        if(component->getInputs().isEmpty()){
            continue;
        }

        QList<PComponent> newInputs;
        for(PComponent input : component->getInputs()){

            PCAnd componentAnd = input.dynamicCast<ComponentAnd>();
            PCOr componentOr = input.dynamicCast<ComponentOr>();
            if(!componentAnd && !componentOr){
                newInputs.append(input);
                continue;
            }

            // It's an AND/OR
            if(input->getInputs().size()==1){
                update = true;
                newInputs.append(input->getInputs()[0]);
            }else{
                newInputs.append(input);
            }
        }

        component->setInputs(newInputs);
    }

    return update;


}

void PropnetProver::buildComponents(){
    components.clear();
    auto propositionMap = propositionDatabase->getPropositionsMap();
    auto end = propositionMap.end();
    for(auto it = propositionMap.begin(); it != end; ++it){
        PProposition proposition = it.value();
        proposition->buildInputPropositions();
        components.append(proposition);
        components += getSubComponents(proposition);
    }
#ifndef QT_NO_DEBUG
    qDebug() << "The propnet has nb components : " << components.size();
#endif
}

QList<PComponent> PropnetProver::getSubComponents(PComponent component){
    //        qDebug() << "getSubComponents";
    QList<PComponent> answer;
    for(PComponent c : component->getInputs()){
        if(c.dynamicCast<Proposition>()){
            continue;
        }
        answer += c;
        answer += getSubComponents(c);
    }
    //    PProposition prop = component.dynamicCast<Proposition>();
    //    if(prop){
    //        qDebug() << "Proposition " << prop->getName() << " has nb inputs " << answer.size();
    //    }
    return answer;
}


PDatabase PropnetProver::getDatabase(){
    return propositionDatabase;
}

/***
                                 * TO FILE
                                */
void PropnetProver::toFile(QString filename){
    indexDot = 0;
    indexDotMap.clear();

    QFile file(filename);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "Saving file " << filename;
        QTextStream out(&file);
        out << "digraph propnet {\n";
        for(PComponent component : components){
            PProposition proposition = component.dynamicCast<Proposition>();
            if(proposition){
                QString propositionDotName = proposition->getName().replace(' ', '_').remove('(').remove(')');
                out << propositionDotName << " [shape=box];\n";
                if(!proposition->hasInput()){
                    continue;
                }
                PComponent singleInput = proposition->getSingleInput();
                PProposition singleInputProposition = singleInput.dynamicCast<Proposition>();
                if(singleInputProposition){
                    QString singleInputPropositionDotName = singleInputProposition->getName().replace(' ', '_').remove('(').remove(')');
                    out << propositionDotName << " -> " << singleInputPropositionDotName << ";\n";
                }
                else{

                    out << propositionDotName << " -> " << singleInput->getComponentDotName() << getIndexDot(singleInput) << ";\n";
                }
            }
            else{
                for(PComponent subComponent : component->getInputs()){
                    out << component->getComponentDotName() << getIndexDot(component) << " -> ";
                    PProposition subProposition = subComponent.dynamicCast<Proposition>();
                    if(subProposition){
                        QString subPropositionDotName = subProposition->getName().replace(' ', '_').remove('(').remove(')');
                        out << subPropositionDotName << ";\n";
                    }else{
                        out << subComponent->getComponentDotName() << getIndexDot(subComponent) << ";\n";
                    }
                }
            }
        }

        out << "}";
        file.close();
    }else{
        qDebug() << "Fail to open file " << filename;
    }


}

int PropnetProver::getIndexDot(PComponent c){
    if(indexDotMap.contains(c)){
        return indexDotMap[c];
    }
    indexDotMap.insert(c, indexDot);
    indexDot++;
    return (indexDot-1);
}

/***
                                 * Give answers
                                 */
PProposition PropnetProver::getPropositionFromString(QString s){
    return propositionDatabase->getProposition(s);
}

void PropnetProver::loadPropnetBasePropositions(QVector<LRelation> baseProp){
    // Did we already load these base propositions?
    if(basePropositionsAlreadyLoaded.size() == baseProp.size()){
        bool alreadyInMemory = true;
        for(LRelation relation : baseProp){
            if(!basePropositionsAlreadyLoaded.contains(relation->toStringWithNoQualifier())){
                alreadyInMemory = false;
                break;
            }
        }
        if(alreadyInMemory){
            // Cool, we can save computation time!
            return;
        }
    }


    clearBasePropositions();
    for(LRelation relation : baseProp){
        propositionDatabase->getProposition(relation->toStringWithNoQualifier())->setValue(true);
    }

    clearAllSavedValues();
    auto end = basePropositions.cend();
    for(auto it = basePropositions.cbegin(); it != end; ++it){
        PProposition baseProp = it.value();
        hasCorrectSavedValue[baseProp] = true;
    }

    basePropositionsAlreadyLoaded.clear();
    for(LRelation relation : baseProp){
        basePropositionsAlreadyLoaded.insert(relation->toStringWithNoQualifier());
    }
}

void PropnetProver::loadPropnetDoesPropositions(QVector<LRelation> doesProp){
    clearAllSavedValues();
    clearDoesPropositions();
    for(LRelation relation : doesProp){
        propositionDatabase->getProposition(relation->toString())->setValue(true);
    }
}

void PropnetProver::buildBaseDoesPropositions(){
    basePropositions.clear();
    doesPropositions.clear();
    viewPropositions.clear();

    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
        LRelation relation = proposition->getRelation();
        if(relation->getQualifier() == Logic::LOGIC_QUALIFIER::BASE){
            basePropositions.insert(relation->toStringWithNoQualifier(), proposition);
            continue;
        }
        if(relation->getHead()->toString() == QString("does")){
            doesPropositions.insert(relation->toString(),proposition);
            continue;
        }
        viewPropositions.insert(relation->toString(),proposition);
    }

#ifndef QT_NO_DEBUG
    qDebug() << "Base Propositions nb : " << basePropositions.size();

    for(PProposition baseProp : basePropositions.values()){
        qDebug() << "\tBase proposition " << baseProp->getName();
    }
    qDebug() << "Does Propositions nb : " << doesPropositions.size();
    for(PProposition doesProp : doesPropositions.values()){
        qDebug() << "\tDoes proposition " << doesProp->getName();
    }
    qDebug() << "\n";

    qDebug() << "View Propositions nb : " << viewPropositions.size();
    for(PProposition viewProp : viewPropositions.values()){
        qDebug() << "\tDoes proposition " << viewProp->getName();
    }
    qDebug() << "\n";
#endif
}

void PropnetProver::clearAllSavedValues(){
    auto end = hasCorrectSavedValue.cend();
    for (auto it = hasCorrectSavedValue.cbegin(); it != end; ++it){
        hasCorrectSavedValue[it.key()] = false;
    }
}

void PropnetProver::clearViewPropositionSavedValues(){
    auto end = viewPropositions.cend();
    for (auto it = viewPropositions.cbegin(); it != end; ++it){
        hasCorrectSavedValue[it.value()] = false;
    }
}

void PropnetProver::clearBasePropositions(){
    auto end = basePropositions.cend();
    for (auto it = basePropositions.cbegin(); it != end; ++it){
        it.value()->setValue(false);
    }
}

void PropnetProver::clearDoesPropositions(){
    auto end = doesPropositions.cend();
    for (auto it = doesPropositions.cbegin(); it != end; ++it){
        it.value()->setValue(false);
    }
}

void PropnetProver::clearViewPropositions(){
    auto end = viewPropositions.cend();
    for (auto it = viewPropositions.cbegin(); it != end; ++it){
        it.value()->setValue(false);
    }
}

bool PropnetProver::propnetEvaluate(QString s){
    return propnetEvaluate(propositionDatabase->getProposition(s));
}

bool PropnetProver::propnetEvaluate(PProposition proposition){
    //        qDebug() << "propnetEvaluate of proposition : " << proposition->getName();
    if(hasCorrectSavedValue.value(proposition)){
        return proposition->getValue();
    }

    if(!proposition->hasInput()){
        //                qDebug() << "propnetEvaluate : this proposition has no input. It's value is " << proposition->getValue();
        hasCorrectSavedValue[proposition] = true;
        return proposition->getValue();
    }



    //        qDebug() << "propnetEvaluate : this proposition has inputs. Time to evaluate them";
    //        for(PProposition inputProposition : proposition->getInputPropositions()){
    //                    qDebug() << "\tpropnetEvaluate : input proposition : " << inputProposition->getName();
    //        }

    for(PProposition inputProposition : proposition->getInputPropositions()){
        propnetEvaluate(inputProposition);
    }


    PComponent singleInput = proposition->getSingleInput();
    bool newValue = singleInput->computeValue();
    proposition->setValue(newValue);

    hasCorrectSavedValue[proposition] = true;

    //        qDebug() << "propnetEvaluate finished. " << proposition->getName() << " is " << newValue;

    return newValue;
}

void PropnetProver::buildDepthChargeMembers(){
    depthChargeGoalVector.clear();
    for(int i = 0; i<roles.size(); ++i){
        depthChargeGoalVector.append(-1);
    }

    roleIndex.clear();
    depthChargeLegalMoves.clear();
    int index = 0;
    for(LTerm roleTerm : roles){
        QString roleString = roleTerm->toString();
        roleIndex.insert(roleString, index);
        depthChargeLegalMoves.insert(roleString, QVector<PProposition>());
        index++;
    }

}

QMap<PProposition, PProposition> PropnetProver::getMapNextPropositionToBaseProposition(){
    return mapNextPropositionToBaseProposition;
}
void PropnetProver::buildMapNextPropositionToBaseProposition(){
    mapNextPropositionToBaseProposition.clear();

    for(LTerm nextTerm :mapNextToBase.keys()){
        QList<PProposition> nextPropositions = propositionDatabase->getPropositions(nextTerm->toString());
        for(PProposition nextProposition : nextPropositions){
            //            qDebug() << "Next proposition " << nextProposition->getName();
            QString nextPropositionString = nextProposition->getName();
            QString basePropositionString = nextPropositionString.remove("next_");
            //            qDebug() << "BP " << basePropositionString;
            PProposition baseProposition = propositionDatabase->getProposition(basePropositionString);

            mapNextPropositionToBaseProposition[nextProposition] = baseProposition;
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() << "LINK NEXT TO BASE : ";
    for(PProposition nextProposition : mapNextPropositionToBaseProposition.keys()){
        PProposition baseProposition = mapNextPropositionToBaseProposition[nextProposition];
        qDebug() << "Linking " << nextProposition->getName() << " to " << baseProposition->getName();
    }
    qDebug() << "\n";
#endif
}

QVector<int> PropnetProver::depthCharge(QVector<LRelation> baseProp){
    //    qDebug() << "PropnetProver::depthCharge";
    loadPropnetBasePropositions(baseProp);
    int nbStateExpanded = 0;

    for(int i = 0; i<roles.size(); ++i){
        depthChargeGoalVector[i] = -1;
    }

    //    qDebug() << "Nb roles " << goals.size();
    //    return goals;


    clearAllSavedValues();


    while(true){
        //        qDebug() << "State ";
        //        for(PProposition baseProp : basePropositions){
        //            if(baseProp->getValue()){
        //                qDebug() << "Proposition " << baseProp->getName();
        //            }
        //        }


        // Is terminal
        //        qDebug() << "Step 1";
        bool isTerminal = propnetEvaluate(propositionDatabase->getProposition("terminal"));

        // If it's the case, get goal, and return the value(s)
        //        qDebug() << "Step 2";
        if(isTerminal){
            break;
        }

        // Else, get a set of legal moves
        //        qDebug() << "Step 3";

        // Clear all legal moves from previous iterations
        auto legalMovesEnd = depthChargeLegalMoves.end();
        for (auto it = depthChargeLegalMoves.begin(); it != legalMovesEnd; ++it){
            it.value().clear();
        }


        QList<PProposition> legalPropositions = propositionDatabase->getPropositions("legal");
        for(PProposition legalProposition : legalPropositions){
            bool isLegal = propnetEvaluate(legalProposition);
            if(isLegal){
                depthChargeLegalMoves[legalProposition->getRelation()->getBody()[0]->toString()].append(legalProposition);
            }
        }

        //        qDebug() << "Nb legal moves for player 0 : " << legalMoves.values()[0].size();

        // Get next state
        //        qDebug() << "Step 4";
        clearDoesPropositions();
        auto legalMovesCEnd = depthChargeLegalMoves.cend();
        for (auto it = depthChargeLegalMoves.cbegin(); it != legalMovesCEnd; ++it){
            QVector<PProposition> legalMovesForSpecificPlayer = it.value();
            PProposition randomLegal =   legalMovesForSpecificPlayer[qrand()%(legalMovesForSpecificPlayer.size())];
            QString doesString = randomLegal->getName().replace("legal", "does");
            PProposition doesProposition = propositionDatabase->getProposition(doesString);


            //            if(propositionDatabase->getProposition(QString("( 2)"))->getValue()){
            //                doesProposition = propositionDatabase->getProposition("(does robot b)");
            //            }
            //            if(propositionDatabase->getProposition(QString("( 3)"))->getValue()){
            //                doesProposition = propositionDatabase->getProposition("(does robot c)");
            //            }
            //            if(propositionDatabase->getProposition(QString("( 4)"))->getValue()){
            //                doesProposition = propositionDatabase->getProposition("(does robot a)");
            //            }
            //            if(propositionDatabase->getProposition(QString("( 5)"))->getValue()){
            //                doesProposition = propositionDatabase->getProposition("(does robot b)");
            //            }
            //            if(propositionDatabase->getProposition(QString("( 6)"))->getValue()){
            //                doesProposition = propositionDatabase->getProposition("(does robot a)");
            //            }

            doesProposition->setValue(true);
            //            qDebug() << "Does Proposition " << doesProposition->getName();
        }


        auto mapNextToBaseCEnd = mapNextPropositionToBaseProposition.cend();
        for(auto it = mapNextPropositionToBaseProposition.cbegin(); it != mapNextToBaseCEnd; ++it){
            PProposition nextProp = it.key();
            propnetEvaluate(nextProp);
        }

        // Load the correct base propositions in memory
        //        qDebug() << "Step 5";
        clearBasePropositions();
        for(auto it = mapNextPropositionToBaseProposition.cbegin(); it != mapNextToBaseCEnd; ++it){
            PProposition nextProp = it.key();
            if(nextProp->getValue()){
                mapNextPropositionToBaseProposition.value(nextProp)->setValue(true);
            }
        }

        nbStateExpanded++;
        // Clear the saved values
        clearAllSavedValues();
    }



    for(PProposition goalProp : propositionDatabase->getPropositions("goal")){
        //        qDebug() << "Goal prop " << goalProp->getName();
        bool isCorrectGoalValue = propnetEvaluate(goalProp);
        if(isCorrectGoalValue){
            QString roleString = goalProp->getRelation()->getBody()[0]->toString();
            int goalValue = goalProp->getRelation()->getBody()[1]->toString().toInt();
            depthChargeGoalVector[roleIndex[roleString]] = goalValue;
            //            if(goalValue > 0){
            //                qDebug() << "BLA goal > 0";
            //            }
        }
    }
    //    qDebug() << "Goal value " << goals[0] << "\n\n";

    depthChargeGoalVector.append(nbStateExpanded);

    return depthChargeGoalVector;
}


/***
 *
 */
void PropnetProver::buildOptimizedPropnet(){


    buildOptimizedMembers();
    buildPropositionsOptimized();

}

void PropnetProver::buildOptimizedMembers(){
#ifndef QT_NO_DEBUG
    qDebug() << "PROPNET OPTIMIZED";
#endif

    propositionIndexOptimized.clear();
    propnetOptimizedIndexReversed.clear();

    auto mapAllProposition = propositionDatabase->getPropositionsMap();
    uint32_t index = 0;
    for(auto it = mapAllProposition.cbegin(), end = mapAllProposition.cend(); it!=end; ++it){
        PProposition currentProp = it.value();
        propositionIndexOptimized.insert(currentProp, index);
        propnetOptimizedIndexReversed.push_back(currentProp);
        index++;
    }

#ifndef QT_NO_DEBUG
    qDebug() << "There are " << propositionIndexOptimized.size() << " propositions";
//    for(PProposition prop : propositionIndexOptimized.keys()){
//        qDebug() << "\tProposition " << prop->getName() << " has index " << propositionIndexOptimized[prop];
//    }
    int index2 = 0;
        for(PProposition prop : propnetOptimizedIndexReversed){
            qDebug() << "\tProposition " << prop->getName() << " has index " << index2;
            index2++;
        }
#endif

    depthChargeGoalVectorOptimized.clear();
    currentDoesMovesOptimized.clear();
    legalMovesOptimized.clear();
    doesMovesOptimized.clear();
    randomRoleOrderOptimized.clear();
    goalOptimized.clear();
    goalValueOptimized.clear();
    for(int i = 0; i<roles.size(); ++i){
        depthChargeGoalVectorOptimized.append(-1);
        currentDoesMovesOptimized.append(-1);
        legalMovesOptimized.append(QVector<int>());
        doesMovesOptimized.append(QVector<int>());
        randomRoleOrderOptimized.append(QVector<int>());
        goalOptimized.append(QVector<int>());
        goalValueOptimized.append(QVector<int>());
    }

#ifndef QT_NO_DEBUG
    qDebug() << "Role index : ";
    for(QString roleString : roleIndex.keys()){
        qDebug() << "Role " << roleString << " has index " << roleIndex[roleString];
    }
#endif



    nextPropositionsOptimized.clear();
    basePropositionsOptimized.clear();
    for(auto it = mapNextPropositionToBaseProposition.begin(), end = mapNextPropositionToBaseProposition.end(); it != end; ++it){

        nextPropositionsOptimized.push_back(propositionIndexOptimized.value(it.key()));
        basePropositionsOptimized.push_back(propositionIndexOptimized.value(it.value()));
    }
#ifndef QT_NO_DEBUG
    qDebug() << "Nb next propositions : " << nextPropositionsOptimized.size();
    for(int index : nextPropositionsOptimized){
        qDebug() << "\tIndex " << index;
    }
#endif


    for(PProposition baseProp : basePropositions.values()){
        int index = propositionIndexOptimized.value(baseProp);
        if(!basePropositionsOptimized.contains(index)){
            basePropositionsOptimized.push_back(index);
        }
    }
#ifndef QT_NO_DEBUG
    qDebug() << "Nb base propositions : " << basePropositionsOptimized.size();
    for(int index : basePropositionsOptimized){
        qDebug() << "\tIndex " << index;
    }
#endif


    PProposition terminalProposition = propositionDatabase->getProposition("terminal");
    terminalPropositionIndexOptimized = propositionIndexOptimized.value(terminalProposition);



    // Clear() was done before
    QList<PProposition> legalPropositions = propositionDatabase->getPropositions("legal");
    for(PProposition legalProposition : legalPropositions){
        QString role = legalProposition->getRelation()->getBody()[0]->toString();
        int rIndex = roleIndex.value(role);
        legalMovesOptimized[rIndex].push_back(propositionIndexOptimized.value(legalProposition));
        randomRoleOrderOptimized[rIndex].push_back(randomRoleOrderOptimized[rIndex].size());
        QString doesString = legalProposition->getName().replace("legal", "does");
        doesMovesOptimized[rIndex].push_back(propositionIndexOptimized.value(propositionDatabase->getProposition(doesString)));
    }

#ifndef QT_NO_DEBUG
    for(int i = 0; i<roles.size(); ++i){
        QVector<int> & legalMoves = legalMovesOptimized[i];
        QString str = "Legal moves for role " + roles[i]->toString() + " : ";
        for(int index : legalMoves){
            str += QString::number(index);
            str += " / ";
        }
        str.remove(str.size()-2,3);
        qDebug() << str;
    }
#endif

#ifndef QT_NO_DEBUG
    for(int i = 0; i<roles.size(); ++i){
        QVector<int> & legalMoves = randomRoleOrderOptimized[i];
        QString str = "Random order for role " + roles[i]->toString() + " : ";
        for(int index : legalMoves){
            str += QString::number(index);
            str += " / ";
        }
        str.remove(str.size()-2,3);
        qDebug() << str;
    }
#endif

    QList<PProposition> goalPropositions = propositionDatabase->getPropositions("goal");
    for(PProposition goalProposition : goalPropositions){
        QString role = goalProposition->getRelation()->getBody()[0]->toString();
        int rIndex = roleIndex.value(role);
        goalOptimized[rIndex].push_back(propositionIndexOptimized.value(goalProposition));
        int goalReward = goalProposition->getRelation()->getBody()[1]->toString().toInt();
        goalValueOptimized[rIndex].push_back(goalReward);
    }

#ifndef QT_NO_DEBUG
    for(int i = 0; i<roles.size(); ++i){
        QVector<int> & goalProps = goalOptimized[i];
        QVector<int> & goalValues = goalValueOptimized[i];
        for(int i=0; i<goalProps.size(); ++i){
            qDebug() << "Goal " << goalProps[i] << " has value " << goalValues[i];
        }
    }
#endif
}

void PropnetProver::buildPropositionsOptimized(){
    propnetOptimized.clear();
    propnetOptimized.reserve(propositionIndexOptimized.size());

    auto mapAllProposition = propositionDatabase->getPropositionsMap();
    for(auto it = mapAllProposition.cbegin(), end = mapAllProposition.cend(); it!=end; ++it){
        PProposition currentProp = it.value();
        std::vector<std::vector<uint32_t> > andOrInput;

        if(!currentProp->hasInput()){
            propnetOptimized.push_back(PropositionOptimized(andOrInput));
            continue;
        }

        PComponent singleInput = currentProp->getSingleInput();

        PCOr singleInputOr = singleInput.dynamicCast<ComponentOr>();
        if(singleInputOr){
            if(singleInputOr->getInputs().size() == 0){
                Q_ASSERT(false);
            }

            int index = -2;
            for(PComponent subComponent : singleInputOr->getInputs()){
                index += 2;
                andOrInput.push_back(std::vector<uint32_t>());
                andOrInput.push_back(std::vector<uint32_t>());

                PCAnd subComponentAnd = subComponent.dynamicCast<ComponentAnd>();
                if(subComponentAnd){
                    fillComponentAndOptimized(subComponentAnd, andOrInput[index], andOrInput[index+1]);
                    continue;
                }

                // Directly a proposition
                PProposition subComponentProposition = subComponent.dynamicCast<Proposition>();
                if(subComponentProposition){
                    andOrInput[index].push_back(propositionIndexOptimized.value(subComponentProposition));
                    continue;
                }

                PCNot subComponentNot = subComponent.dynamicCast<ComponentNot>();
                if(subComponentNot){
                    andOrInput[index+1].push_back(propositionIndexOptimized.value(subComponentNot->getInputs()[0].dynamicCast<Proposition>()));
                    continue;
                }

                Q_ASSERT(false);
            }
            propnetOptimized.push_back(PropositionOptimized(andOrInput));
            continue;
        }

        PProposition singleInputProposition = singleInput.dynamicCast<Proposition>();
        if(singleInputProposition){
            andOrInput.push_back(std::vector<uint32_t>());
            andOrInput.push_back(std::vector<uint32_t>());
            andOrInput[0].push_back(propositionIndexOptimized.value(singleInputProposition));
            propnetOptimized.push_back(PropositionOptimized(andOrInput));
            continue;
        }

        PCAnd singleInputAnd = singleInput.dynamicCast<ComponentAnd>();
        if(singleInputAnd){
            andOrInput.push_back(std::vector<uint32_t>());
            andOrInput.push_back(std::vector<uint32_t>());
            fillComponentAndOptimized(singleInputAnd, andOrInput[0], andOrInput[1]);
            propnetOptimized.push_back(PropositionOptimized(andOrInput));
            continue;
        }

        PCNot singleInputNot = singleInput.dynamicCast<ComponentNot>();
        if(singleInputNot){
            andOrInput.push_back(std::vector<uint32_t>());
            andOrInput.push_back(std::vector<uint32_t>());
            andOrInput[1].push_back(propositionIndexOptimized.value(singleInputNot->getInputs()[0].dynamicCast<Proposition>()));
            propnetOptimized.push_back(PropositionOptimized(andOrInput));
            continue;
        }
        Q_ASSERT(false);
    }
}

void PropnetProver::fillComponentAndOptimized(PCAnd p, std::vector<uint32_t> & prop, std::vector<uint32_t> & negProp){
    for(PComponent component : p->getInputs()){
        PProposition componentProposition = component.dynamicCast<Proposition>();
        if(componentProposition){
            prop.push_back(propositionIndexOptimized.value(componentProposition));
            continue;
        }

        PCNot componentNot = component.dynamicCast<ComponentNot>();
        if(componentNot){
            PProposition proposition = componentNot->getInputs()[0].dynamicCast<Proposition>();
            if(proposition){
                negProp.push_back(propositionIndexOptimized.value(proposition));
                continue;
            }
            else{
                Q_ASSERT(false);
            }
        }
        Q_ASSERT(false);
    }
}


QVector<int> PropnetProver::depthChargeOptimized(QVector<LRelation> baseProp){
//    volatile int j = 12;
//    qDebug() << "j=" << j;
//    for(int i=0; i<1000000; ++i){
//       j += i;
//    }
////    using namespace std::literals;
//    std::this_thread::sleep_for(std::chrono::seconds(2));
//    qDebug() << "j=" << j;
    loadPropnetBasePropositionsOptimized(baseProp);
    int nbStateExpanded = 0;

    for(int i = 0; i<roles.size(); ++i){
        depthChargeGoalVectorOptimized[i] = -1;
    }


    while(true){


        // Is terminal
        bool isTerminal = propnetOptimized[terminalPropositionIndexOptimized].computeValue(propnetOptimized);

        // If it's the case, get goal, and return the value(s)
        //        qDebug() << "Step 2";
        if(isTerminal){
            break;
        }

        // Else, get a set of legal moves
        //        qDebug() << "Step 3";

        // Get the first legal move which is true, and set the
        // corresponding does to true
        randomizeLegalMoveOptimized();

#ifndef QT_NO_DEBUG
//    for(int i = 0; i<roles.size(); ++i){
//        QVector<int> & legalMoves = randomRoleOrderOptimized[i];
//        QString str = "Random order for role " + roles[i]->toString() + " : ";
//        for(int index : legalMoves){
//            str += QString::number(index);
//            str += " / ";
//        }
//        str.remove(str.size()-2,3);
//        qDebug() << str;
//    }
#endif

        for(int indexRole=0; indexRole<roles.size(); ++indexRole){
            for(int indexDoesProposition : doesMovesOptimized[indexRole]){
                propnetOptimized[indexDoesProposition].setValue(false);
            }

            QVector<int> & legalForOneRole = legalMovesOptimized[indexRole];
            QVector<int> & randomizedOrder = randomRoleOrderOptimized[indexRole];
            for(int indexRandom : randomizedOrder){
                int indexLegal = legalForOneRole[indexRandom];
                bool isLegal = propnetOptimized[indexLegal].computeValue(propnetOptimized);
                if(isLegal){
//                    currentDoesMovesOptimized[indexRole] = doesMovesOptimized[indexRole][indexRandom];
//                    propnetOptimized[indexLegal].setValue(true);
                    propnetOptimized[doesMovesOptimized[indexRole][indexRandom]].setValue(true);
                    propnetOptimized[doesMovesOptimized[indexRole][indexRandom]].setAlreadyComputed(true);
                    break;
                }
            }
        }


        // Compute the next state
        for(int i = 0; i<nextPropositionsOptimized.size(); ++i){
            propnetOptimized[nextPropositionsOptimized[i]].computeValue(propnetOptimized);
        }

        for(int i = 0; i<basePropositionsOptimized.size(); ++i){
            propnetOptimized[basePropositionsOptimized[i]].setValue(false);
        }

        // Swap the next and base
        for(int i = 0; i<nextPropositionsOptimized.size(); ++i){
            bool nextValue = propnetOptimized[nextPropositionsOptimized[i]].getValue();
            propnetOptimized[basePropositionsOptimized[i]].setValue(nextValue);
            propnetOptimized[basePropositionsOptimized[i]].setAlreadyComputed(true);
        }

        nbStateExpanded++;

        clearAllSavedValuesOptimized();
    }

    QVector<int> answer;

    // Goals
    for(int indexRole = 0; indexRole<roles.size(); ++indexRole){
        QVector<int> & goalPropositions = goalOptimized[indexRole];

        for(int i=0; i<goalPropositions.size(); ++i){
            int indexGoalProposition = goalPropositions[i];
            if(propnetOptimized[indexGoalProposition].computeValue(propnetOptimized)){
                answer.push_back(goalValueOptimized[indexRole][i]);
                break;
            }
        }
    }

    answer.append(nbStateExpanded);

    return answer;

}

void PropnetProver::loadPropnetBasePropositionsOptimized(QVector<LRelation> & baseProps){
    // Did we already load these base propositions?
    //    if(basePropositionsAlreadyLoaded.size() == baseProp.size()){
    //        bool alreadyInMemory = true;
    //        for(LRelation relation : baseProp){
    //            if(!basePropositionsAlreadyLoaded.contains(relation->toStringWithNoQualifier())){
    //                alreadyInMemory = false;
    //                break;
    //            }
    //        }
    //        if(alreadyInMemory){
    //            // Cool, we can save computation time!
    //            return;
    //        }
    //    }


    clearBasePropositionsOptimized();
    for(LRelation relation : baseProps){
        PProposition baseProp = propositionDatabase->getProposition(relation->toStringWithNoQualifier());
        uint32_t index = propositionIndexOptimized.value(baseProp);
        propnetOptimized[index].setValue(true);
    }

    clearAllSavedValuesOptimized();
}

void PropnetProver::clearBasePropositionsOptimized(){
    for(int index : basePropositionsOptimized){
        propnetOptimized[index].setValue(false);
    }
}

void PropnetProver::clearAllSavedValuesOptimized(){
    for(PropositionOptimized & proposition : propnetOptimized){
        proposition.setAlreadyComputed(false);
    }
}

void PropnetProver::randomizeLegalMoveOptimized(){
    for(QVector<int> & vec : randomRoleOrderOptimized){
        for(int i = vec.size()-1; i>0; i--){
            int j = qrand()%(i+1);
            int temp = vec[i];
            vec[i] = vec[j];
            vec[j] = temp;
        }
    }
}

bool PropnetProver::propnetEvaluateOptimized(QString s){
    return propnetOptimized[propositionIndexOptimized[propositionDatabase->getProposition(s)]].computeValue(propnetOptimized);
}

/***
                                 * Misc
                                 */

void PropnetProver::debug(){

    qDebug() << "DEBUG";
    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
        qDebug() << proposition->printFullDebug();
        qDebug() << "\tProposition inputs : ";
        for(PProposition inputProp : proposition->getInputPropositions()){
            qDebug() << "\t\t" << inputProp->getName();
        }
    }
    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){

    }

    qDebug() << "\n";
}
