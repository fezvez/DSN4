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
#include "../flags.h"


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

    generatePropnet();

//    qDebug() << "\n\nPropnet BEFORE CLEAN";
//    debug();

    cleanPropnet();

//    qDebug() << "\n\nPropnet AFTER CLEAN";
//    debug();

    buildComponents();
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
    qDebug() << "\n\nGENERATE PROPNET";
#endif


    propositionDatabase->clear();

    QList<QList<LTerm> > stratifiedConstants = getStratifiedConstants();

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
//                qDebug() << "There are no relations of head " << relationConstant->toString();
                LTerm term = constantMap[relationConstant->toString()];
                Q_ASSERT(!constantToRelationMap.contains(term));
            }
        }
    }

#ifndef QT_NO_DEBUG
    propositionDatabase->printAllPropositions();
#endif

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
}



QList<LRule> PropnetProver::getGrounding(LRule rule){
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
            newRule->rebuildName();
            QList<LRule> answersFromNewRule = getGrounding(newRule);
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
            LRelation nonNegationRelation = relation->clone();
            nonNegationRelation->setNegation(false);
            PProposition tempProp = propositionDatabase->getProposition(nonNegationRelation);
            cnot->addInput(tempProp);

            fundamentalComponent->addInput(cnot);
            continue;
        }
        if(relation->getHead()->toString() == "distinct"){
//            qDebug() << "It's distinct";
            QList<LTerm> body = relation->getBody();
            if(body[0]->toString() == body[1]->toString()){
                return false;
            }
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





/***
 * CLEAN PROPNET
 */

void PropnetProver::cleanPropnet(){

    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
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

    bool iterateAgain;
    do{
        iterateAgain = cleanPropnetIteration();
    }while(iterateAgain);

//    buildComponents();
}

bool PropnetProver::cleanPropnetIteration(){
    bool update = false;

    buildComponents();
#ifndef QT_NO_DEBUG
    qDebug() << "\n\nCLEAN PROPNET";
#endif
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


    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
        QString relationHead = proposition->getRelation()->getHead()->toString();
        QList<PComponent> clearInput;
        if(relationHead == "does"){
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
            newInput.append(singleInput->getInputs()[0]->getInputs()[0]);
            proposition->setInputs(newInput);

//            qDebug() << proposition->getName() << " has now for input " << proposition->getSingleInput().dynamicCast<Proposition>()->getName();
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

void PropnetProver::buildComponents(){
    components.clear();
    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
        components.append(proposition);
        components += getSubComponents(proposition);
    }
#ifndef QT_NO_DEBUG
    qDebug() << "\n\nBuild components";
        qDebug() << "Nb components " << components.size();
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



/***
 * TO FILE
*/
void PropnetProver::toFile(QString filename){
    indexDot = 0;

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

void PropnetProver::loadPropnetBasePropositions(QVector<LRelation> basePropositions){

}

/***
 * Misc
 */

void PropnetProver::debug(){
    for(PProposition proposition : propositionDatabase->getPropositionsMap().values()){
        qDebug() << proposition->printFullDebug();
    }
}
