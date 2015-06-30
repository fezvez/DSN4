#include "propnet_test.h"

#include <QDebug>

void Propnet_Test::stratum()
{
    qDebug() << "PT";
    PropnetProver prover;



    {
        QString filename = "connectionTestGame.kif";
        QFile file(filename);
        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream out(&file);
            QString rules = "((role white)(base (r 1))(base (r 2))(input white move)(init (r 1))(legal white move)(<= (next (r 2))(true (r 1)))(goal white 0)(<= terminal (true (r 2))))";
            QStringList rulesBis = Parser::splitSeveral(rules);
            for(QString rule : rulesBis){
                out << rule;
            }
            file.close();
        }
        prover.loadKifFile(filename);

        prover.toFile("connectionTestGame.dot");
    }

//    prover.loadKifFile("../../../tictactoe.kif");
    qDebug() << "PropnetProver created";

    QVERIFY(true);

}

void Propnet_Test::stratum2()
{
//    qDebug() << "PT";
//    PropnetProver prover;
//    prover.loadKifFile("../../../tictactoe.kif");
//    prover.toFile("tictactoe.dot");
}



