#include "propnet_test.h"

#include <QDebug>

void Propnet_Test::propnet1()
{
    PropnetProver prover;

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

void Propnet_Test::propnet2()
{

    PropnetProver prover;

    QString filename = "connectionTestGame2.kif";
    QFile file(filename);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);
        QString rules = "((role white)(base (count 1))(base (count 2))(base (count 3))(base (l 1))(base (l 2))(input white move1)(input white move2)(init (count 1))(init (l 1))(init (l 2))(<= (legal white move1)(true (l 1)))(<= (legal white move2)(true (l 2)))(<= (next (l 1))(does white move1))(<= (next (l 2))(does white move2))(<= (next (count 2))(true (count 1)))(<= (next (count 3))(true (count 2)))(goal white 0)(<= terminal (true (count 3))))";
        QStringList rulesBis = Parser::splitSeveral(rules);
        for(QString rule : rulesBis){
            out << rule;
        }
        file.close();
    }
    prover.loadKifFile(filename);

    prover.toFile("connectionTestGame2.dot");



}

void Propnet_Test::propnetTicTacToe(){
    PropnetProver prover;
    prover.loadKifFile("../../../../tictactoe.kif");
    prover.toFile("tictactoe.dot");
}



