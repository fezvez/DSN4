#ifndef MONTECARLOGAMER_H
#define MONTECARLOGAMER_H

#include "player.h"
#include "../StateMachine/proverstatemachine.h"

class MonteCarloGamer : public Player
{
public:
    MonteCarloGamer(int p = 9147);
    ~MonteCarloGamer();

    void initializeStateMachine(QString filename, QString role);

    void metagame(qint64 timeout);
    Move selectMove(qint64 timeout);

    int performDepthCharge(MachineState state);
    int nbDepthCharge;
    int nbStatesExplored;
};

#endif // MONTECARLOGAMER_H
