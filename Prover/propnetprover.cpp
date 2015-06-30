#include "propnetprover.h"

#include "../kifloader.h"

#include <QStringList>
#include <QDebug>
#include <QFile>

#include "../Logic/logic_rule.h"
#include "../PropNet/propositionconstant.h"
#include "../Unification/unification_relation.h"

#include "../PropNet/componentand.h"
#include "../PropNet/componentor.h"
#include "../PropNet/componentnot.h"

PropnetProver::PropnetProver()
{
    propositionDatabase = PDatabase(new PropositionDatabase());
}

PropnetProver::~PropnetProver()
{
    qDebug() << "~PropnetProver";
}

void PropnetProver::setup(QList<LRelation> relations, QList<LRule> rules){
    GDLProver::setup(relations, rules);

    qDebug() << "Propnet";
    KnowledgeBase::generateStratum();

    generatePropnet();

    qDebug() << "\n\nPropnet BEFORE CLEAN";
    debug();
    cleanPropnet();
qDebug() << "\n\nPropnet AFTER CLEAN";
    debug();
    //    linkBaseToNext();

    //    buildRelationTypesAndQualifiers();
    //    buildRulesTypesAndQualifiers();
    //    buildBaseInitInputDoesPropositions();
    //    buildRolePropositions(relations);
    //    buildStandardEvaluationStructures();
}

void PropnetProver::loadKifFile(QString filename){
    qDebug() << "Propnet 0";
    KifLoader kifLoader(nullptr, filename);
    QStringList sL = kifLoader.runSynchronously();

    Parser parser;
    parser.loadKif(sL);

    setup(parser.getRelations(), parser.getRules());
    qDebug() << "Bla";
}

void PropnetProver::generatePropnet(){
    qDebug() << "\n\nGENERATE PROPNET";

    propositionDatabase->clear();

    QList<QList<LTerm> > stratifiedConstants = getStratifiedConstants();
    QMap<LTerm, QList<LRule>> constantToRuleMap = getConstantToRuleMap();
    QMap<LTerm, QList<LRelation>> constantToRelationMap = getConstantToRelationMap();

    qDebug() << "Adding relations";

    // Add all the relations
    for(auto list : stratifiedConstants){
        for(LTerm relationConstant : list){
            qDebug() << "Relation constant " << relationConstant->toString();
            if(relationConstant->toString() == "input"){
                continue;
            }

            if(constantToRelationMap.contains(relationConstant)){
                qDebug() << "Relation constant " << relationConstant->toString() << " is indeed insde";
                QList<LRelation> sublist = constantToRelationMap[relationConstant];
                qDebug() << "Relation constant " << relationConstant->toString() << " nb " << sublist.size();
                for(LRelation relation : sublist){
                    if(relation->getQualifier() == Logic::LOGIC_QUALIFIER::INIT){
                        qDebug() << "Skipping init relation " << relation->toString();
                        continue;
                    }
                    propositionDatabase->getProposition(relation);
                }
            }
            else{
                qDebug() << "There are no relations of head " << relationConstant->toString();
                LTerm term = constantMap[relationConstant->toString()];
                if(constantToRelationMap.contains(term)){
                    qDebug() << "\n\n\n\nFUCK";
                }
            }
        }
    }


    qDebug() << "Added all relations";
    propositionDatabase->printAllPropositions();


    // Add all the rules

    // Do it by strata level
    int strataLevel = 0;
    for(auto list : stratifiedConstants){

        qDebug() << "\n\nNEW STRATA of size " << list.size() << " at level " << strataLevel;
        strataLevel++;
//        if(strataLevel>2){
//            break;
//        }
        // We are in a certain level of the strata
        // You may have to iterate in the case of recursion
        bool update;
        while(true){
            update = false;

            for(LTerm relationConstant : list){
                if(relationConstant->toString() == "input"){
                    continue;
                }

                qDebug() << "Relation constant " << relationConstant->toString();
                // For each relation constant, compute all the rules
                if(constantToRuleMap.contains(relationConstant)){
                    for(LRule rule : constantToRuleMap[relationConstant]){
                        qDebug() << "GROUNDING FUNDAMENTAL rule " << rule->toString();
                        QList<LRule> groundedRules = getGrounding(rule);
                        qDebug() << "Nb groundings : " << groundedRules.size();
                        for(LRule rule : groundedRules){
                            qDebug() << "\t\tGrounded rule " << rule->toString();
                            bool updated = addRuleToDatabase(rule);
                            qDebug() << "Updated "<< updated;
                            //                            bool updated = false;
                            update = update || updated;
                        }
                    }
                }
            }
            //            break;
            if(!update){
                break;
            }
        }
    }

    qDebug() << "\n\nPROPNET GENERATION FINISHED";
}



QList<LRule> PropnetProver::getGrounding(LRule rule){
    qDebug() << "Grounding rule " << rule->toString();
    QList<LRule> answer;

    if(rule->isGround()){
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

    qDebug() << "First ungrounded relation " << firstUngroundedRelation->toString();

    LTerm relationConstant = firstUngroundedRelation->getHead();

    PRelationDatabase relationDatabase = propositionDatabase->getRelationDatabase(relationConstant->toString());
    QList<PProposition> possibleSubstitutes = relationDatabase->getAllPropositions();

    qDebug() << firstUngroundedRelation->toString() << " can be subbed by " << possibleSubstitutes.size();
    for(PProposition propSubstitute : possibleSubstitutes){
        qDebug() << "Possible sub " << propSubstitute->getName();
        LRelation possibleSubstitute = propSubstitute->getRelation();

        Unification_Relation unification = Unification_Relation(firstUngroundedRelation, possibleSubstitute);
        //        qDebug() << "Unification " << unification.toString();
        if(unification.isUnificationValid()){
            LRule newRule = unification.applySubstitution(rule);
            newRule->rebuildName();
            QList<LRule> answersFromNewRule = getGrounding(newRule);
            answer += answersFromNewRule;
        }
    }

    qDebug() << "We have " << answer.size() << " possible substitutions";
    return answer;
}

bool PropnetProver::addRuleToDatabase(LRule rule){
    qDebug() << "Adding rule to DB : " << rule->toString();
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
        qDebug() << "Adding to the AND component " << relation->toStringWithNoQualifier();

        if(relation->isNegation()){
            qDebug() << "It's negation";
            PCNot cnot = PCNot(new ComponentNot());
            LRelation nonNegationRelation = relation->clone();
            nonNegationRelation->setNegation(false);
            PProposition tempProp = propositionDatabase->getProposition(nonNegationRelation);
            cnot->addInput(tempProp);

            fundamentalComponent->addInput(cnot);
            continue;
        }
        if(relation->getHead()->toString() == "distinct"){
            qDebug() << "It's distinct";
            QList<LTerm> body = relation->getBody();
            if(body[0]->toString() == body[1]->toString()){
                return false;
            }
            continue;
        }


        fundamentalComponent->addInput(propositionDatabase->getProposition(relation));

    }

    qDebug() << "AND component created";

    // Find if it has an OR as single input
    // If not, put the OR in place

    //    PCOr orComponent = PCOr(new ComponentOr());

    if(!headProposition->hasInput()){
        qDebug() << "Head had no OR input";
        PCOr orComponent = PCOr(new ComponentOr());
        orComponent->addInput(fundamentalComponent);
        headProposition->addInput(orComponent.staticCast<Component>());
    }else{
        PCOr orComponent = headProposition->getSingleInput().dynamicCast<ComponentOr>();
        if(orComponent){
            qDebug() << "Head has OR input";
            orComponent->addInput(fundamentalComponent);
        }
        else{
            qDebug() << "BUG : Head has strange as fuck input";
        }
    }


    qDebug() << "Rule has been added " << rule->toString();

    return true;
}

void PropnetProver::toFile(QString filename){
    int andIndex = 0;
    int orIndex = 0;

    QFile file(filename);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "Saving file " << filename;
        QTextStream out(&file);
        out << "digraph propnet {\n";
        for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
            out << proposition->getName().replace(' ', '_').remove('(').remove(')') << " [shape=box];\n";
            out << proposition->getName().replace(' ', '_').remove('(').remove(')');
            if(!proposition->hasInput()){
                out << ";\n";
                continue;
            }

            out << " -> OR_" << orIndex << ";\n";

            for(PComponent andComponent : proposition->getSingleInput()->getInputs()){
                out << "OR_" << orIndex << " -> AND_" << andIndex << ";\n";

                for(PComponent subComponent : andComponent->getInputs()){
                    PProposition prop = subComponent.dynamicCast<Proposition>();
                    if(prop){
                        out << "AND_" << andIndex << " -> " << prop->getName().replace(' ', '_').remove('(').remove(')') << ";\n";
                    }

                    PCNot componentNot = subComponent.dynamicCast<ComponentNot>();
                    if(componentNot){
                        PProposition notInput = componentNot->getInputs()[0].dynamicCast<Proposition>();
                        QString str = notInput->getName().replace(' ', '_').remove('(').remove(')');
                        out << "NOT_" << str << " -> " << str << ";\n";
                    }
                }
                andIndex++;
            }
            orIndex++;
        }
        out << "}";
        file.close();
    }else{
        qDebug() << "Fail to open file " << filename;
    }


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

void PropnetProver::cleanPropnet(){
    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
        QString relationHead = proposition->getRelation()->getHead()->toString();
        QList<PComponent> clearInput;
        if(relationHead == "input"){
            proposition->setInputs(clearInput);
            continue;
        }
        if(proposition->getRelation()->getQualifier() == Logic::BASE){
            proposition->setInputs(clearInput);
            continue;
        }


        if(!proposition->hasInput()){
            continue;
        }
        PComponent singleInput = proposition->getSingleInput();
        if(singleInput->getInputs().size()==1){
            QList<PComponent> newInput;
            newInput.append(singleInput->getInputs()[0]);
            proposition->setInputs(newInput);
        }
        else{
            QList<PComponent> newInputForOr;
            for(PComponent andComponent : singleInput->getInputs()){
                if(andComponent->getInputs().size() == 1){
                    newInputForOr.append(andComponent->getInputs()[0]);
                }
                else{
                    newInputForOr.append(andComponent);
                }
            }
            singleInput->setInputs(newInputForOr);
        }
    }
}

void PropnetProver::debug(){
for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
    qDebug() << proposition->debug();
}
}
