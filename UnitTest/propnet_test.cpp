#include "propnet_test.h"

#include <QDebug>

void Propnet_Test::stratum()
{
    qDebug() << "PT";
    PropnetProver prover;
    prover.loadKifFile("../../../tictactoe.kif");



}



