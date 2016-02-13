#ifndef MONTECARLOGAMER_H
#define MONTECARLOGAMER_H

#include "player.h"
#include "../StateMachine/proverstatemachine.h"
#include "../StateMachine/propnetstatemachine.h"

class MonteCarloGamer : public Player
{
public:
    MonteCarloGamer(int p = 9147);
    ~MonteCarloGamer();

    void initializeStateMachine(QString filename, QString role);

    void metagame(qint64 timeout);
    Move selectMove(qint64 timeout);

    int performDepthCharge(MachineState state);
    int performDepthCharge2(MachineState state);

    int nbDepthCharge;
    int nbStatesExplored;
    int myRoleIndex;
};

#endif // MONTECARLOGAMER_H
