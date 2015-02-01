#include "propositiondatabase.h"

#include <QDebug>


PropositionDatabase::PropositionDatabase()
{

}

QString PropositionDatabase::getCurrentString(){
    return QString("");
}

PProposition PropositionDatabase::addProposition(QList<PConstant> prop){
    PProposition answer;
    prop.size();
//    Q_ASSERT(prop.length()>0);
//    PConstant head = prop.first();
//    prop.pop_front();
//    if(prop.length()>1){
//        if(!databaseMap.contains(head)){
//            databaseMap.insert(head, PSDatabase(new PropositionDatabase(head, PDatabase(this))));
//        }
//        PDatabase subDatabase = databaseMap[head];
//        return subDatabase->addProposition(prop);
//    }
//    else{
//        if(propositionMap.contains(head)){
//            qDebug() << "Redundancy in the proposition database";
//            return propositionMap[head];
//        }
//        else{
//            QString propString;
//            propString = propString + ' ' + head->toString();
//            PProposition p;
//            propositionMap.insert(head, p);
//            return p;
//        }
//    }

    return answer;
}





/**
 * @brief PropositionDatabase::SubDatabase::SubDatabase
 */


PropositionDatabase::SubDatabase::SubDatabase(){

}

QString PropositionDatabase::SubDatabase::getCurrentString(){
    return QString("");
}

PProposition PropositionDatabase::SubDatabase::addProposition(QList<PConstant> prop){
    Q_ASSERT(prop.length()>0);
    PConstant head = prop.first();
    prop.pop_front();
    if(prop.length()>1){
        if(!databaseMap.contains(head)){
            //databaseMap.insert(head, PDatabase(new PropositionDatabase(head, PDatabase(this))));
        }
        PDatabase subDatabase = databaseMap[head];
        return subDatabase->addProposition(prop);
    }
    else{
        if(propositionMap.contains(head)){
            qDebug() << "Redundancy in the proposition database";
            return propositionMap[head];
        }
        else{
            QString propString = currentString;
            propString = propString + ' ' + head->toString();
            PProposition p;
            propositionMap.insert(head, p);
            return p;
        }
    }
}
