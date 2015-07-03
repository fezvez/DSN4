#include "statemachine.h"

#include <QDebug>
#include "../flags.h"

QVector<Role> StateMachine::getRoles(){
    return roles;
}

MachineState StateMachine::getInitialState(){
    return initialState;
}

QList<Move> StateMachine::getRandomLegalJointMove(const MachineState& state, Role role, Move move){
    QList<Move> answer;
#ifndef QT_NO_DEBUG
    qDebug() << "getRandomLegalJointMove() for role " << role.toString() << " doing move " << move.toString();
#endif
    QString roleString = role.getTerm()->toString();
    for(Role r : roles){
        if(r.getTerm()->toString() == roleString){
            answer.append(move);
            continue;
        }
        QList<Move> moves = getLegalMoves(state, r);
        answer.append(moves[qrand()%(moves.size())]);
    }
    Q_ASSERT(answer.size() == roles.size());

    return answer;
}

QList<Move> StateMachine::getRandomLegalJointMove(const MachineState& state){
    QList<Move> answer;

//    qDebug() << "getRandomLegalJointMove()";

    for(Role r : roles){
        QList<Move> moves = getLegalMoves(state, r);
        answer.append(moves[qrand()%(moves.size())]);
    }
    return answer;
}
