#include "propnetprover.h"

#include "../kifloader.h"

#include <QStringList>
#include <QDebug>

PropnetProver::PropnetProver()
{

}

PropnetProver::~PropnetProver()
{

}

void PropnetProver::setup(QList<LRelation> relations, QList<LRule> rules){
    KnowledgeBase::setup(relations, rules);

    qDebug() << "Propnet";
    KnowledgeBase::generateStratum();

    generatePropnet();

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
}

void PropnetProver::generatePropnet(){
    qDebug() << "\n\nGENERATE PROPNET";

    QList<QList<LTerm> > stratifiedConstants = getStratifiedConstants();

    for(auto list : stratifiedConstants){

    }
}
