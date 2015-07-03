#include "machinestate.h"
#include <QStringBuilder>

MachineState::MachineState(){
    // contents will be empty, as expected
}

MachineState::MachineState(QVector<LRelation> c):
    contents(c)
{

}

QString MachineState::toString() const{
    QString answer;
    answer += "MachineState : ";
    for(LRelation relation : contents){
        answer += relation->toString();
        answer += "\t";
    }
    return answer;
}

const QVector<LRelation> MachineState::getContents() const{
    return contents;
}
