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
        answer += " ";
        answer += relation->toString();
    }
    return answer;
}

QStringList MachineState::toStringList() const{
    QStringList answer;
    for(LRelation relation : contents){
        answer << relation->toString();
    }
    return answer;
}

const QVector<LRelation> MachineState::getContents() const{
    return contents;
}
