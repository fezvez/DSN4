#ifndef FIRSTPLAYER_H
#define FIRSTPLAYER_H

#include "player.h"
#include "../StateMachine/proverstatemachine.h"

class FirstPlayer : public Player
{
public:
    FirstPlayer(int p = 9147);
    ~FirstPlayer();

    void initializeStateMachine(QString filename, QString role);

    void metagame(qint64 timeout);
    Move selectMove(qint64 timeout);


};

#endif // FIRSTPLAYER_H
