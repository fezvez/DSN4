#include "propnetstatemachine_test.h"

#include "../StateMachine/propnetstatemachine.h"

void PropnetStateMachine_Test::connectionTestGame(){
    PropnetStateMachine psm;


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
    psm.initialize(filename);

    MachineState initialState = psm.getInitialState();
    QCOMPARE(initialState.getContents().size(), 1);
    QVERIFY(initialState.getContents()[0]->toString() == "(base (r 1))");

    QVector<Role> roles = psm.getRoles();
    for(Role role : roles){
        int goalValue = psm.getGoal(initialState, role);
        QCOMPARE(goalValue, 0);
    }

    QCOMPARE(psm.isTerminal(initialState), false);
    for(Role role : roles){
        QList<Move> legalMoves = psm.getLegalMoves(initialState, role);
        QCOMPARE(legalMoves.size(), 1);
    }

    Parser parser;
    LTerm moveTerm = parser.parseTerm("(move)");

    QList<Move> moves;
    moves.append(Move(moveTerm));

    MachineState nextState = psm.getNextState(initialState, moves);

    QCOMPARE(nextState.getContents().size(), 1);
    QCOMPARE(nextState.getContents()[0]->toStringWithNoQualifier(), QString("(r 2)"));
    QCOMPARE(psm.isTerminal(nextState), true);

}
void PropnetStateMachine_Test::connectionTestGame2(){
    PropnetStateMachine psm;

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
    psm.initialize(filename);


    MachineState initialState = psm.getInitialState();
    QCOMPARE(initialState.getContents().size(), 3);


    QVector<Role> roles = psm.getRoles();
    for(Role role : roles){
        int goalValue = psm.getGoal(initialState, role);
        QCOMPARE(goalValue, 0);
    }

    QCOMPARE(psm.isTerminal(initialState), false);
    for(Role role : roles){
        QList<Move> legalMoves = psm.getLegalMoves(initialState, role);
        QCOMPARE(legalMoves.size(), 2);
    }

    Parser parser;
    LTerm moveTerm1 = parser.parseTerm("(move1)");

    QList<Move> moves;
    moves.append(Move(moveTerm1));

    MachineState nextState = psm.getNextState(initialState, moves);
    QCOMPARE(nextState.getContents().size(), 2);

    LTerm moveTerm2 = parser.parseTerm("(move2)");

    moves.clear();
    moves.append(Move(moveTerm2));

    nextState = psm.getNextState(nextState, moves);
    QCOMPARE(nextState.getContents().size(), 2);

    QCOMPARE(psm.isTerminal(nextState), true);
}

void PropnetStateMachine_Test::connectionTestGameMC(){
    PropnetStateMachine psm;

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
    psm.initialize(filename);


    MachineState initialState = psm.getInitialState();
    QCOMPARE(initialState.getContents().size(), 3);


    QVector<Role> roles = psm.getRoles();
    for(Role role : roles){
        int goalValue = psm.getGoal(initialState, role);
        QCOMPARE(goalValue, 0);
    }

    QCOMPARE(psm.isTerminal(initialState), false);
    for(Role role : roles){
        QList<Move> legalMoves = psm.getLegalMoves(initialState, role);
        QCOMPARE(legalMoves.size(), 2);
    }

    QList<Move> moves = psm.getRandomLegalJointMove(initialState);

    MachineState nextState = psm.getNextState(initialState, moves);
    QCOMPARE(nextState.getContents().size(), 2);

    moves = psm.getRandomLegalJointMove(nextState);

    nextState = psm.getNextState(nextState, moves);
    QCOMPARE(nextState.getContents().size(), 2);

    QCOMPARE(psm.isTerminal(nextState), true);
}

void PropnetStateMachine_Test::tictactoe(){
    PropnetStateMachine psm;
    psm.initialize("../../../../tictactoe.kif");

    bool diagonalX = psm.evaluate("(diagonal x)");
    QCOMPARE(diagonalX, false);

    MachineState initialState = psm.getInitialState();


    QVector<Role> roles = psm.getRoles();
    for(Role role : roles){
        int goalValue = psm.getGoal(initialState, role);
        QCOMPARE(goalValue, 50);
    }

    QCOMPARE(psm.isTerminal(initialState), false);
    for(Role role : roles){
        QList<Move> legalMoves = psm.getLegalMoves(initialState, role);
        if(role.getTerm()->toString() == QString("white")){
            QCOMPARE(legalMoves.size(), 9);
        }
        else{
            QCOMPARE(legalMoves.size(), 1);
        }
    }

    Parser parser;
    LTerm moveTerm1 = parser.parseTerm("(mark 1 1)");
    LTerm moveTerm2 = parser.parseTerm("(noop)");

    QList<Move> moves;
    moves.append(Move(moveTerm1));
    moves.append(Move(moveTerm2));
    MachineState nextState = psm.getNextState(initialState, moves);
    QCOMPARE(nextState.getContents().size(), 10);

    bool isCorrect = false;
    for(LRelation relation : nextState.getContents()){
        if(relation->toString() == "(base (cell 1 1 x))"){
            isCorrect = true;
        }
    }
    QVERIFY(isCorrect);
}

void PropnetStateMachine_Test::connectfour(){
    PropnetStateMachine psm;
    psm.initialize("../../../../connectfour.kif");

    MachineState initialState = psm.getInitialState();
    QCOMPARE(initialState.getContents().size(), 1);

    QVector<Role> roles = psm.getRoles();
    for(Role role : roles){
        int goalValue = psm.getGoal(initialState, role);
        QCOMPARE(goalValue, 0);
    }

    QCOMPARE(psm.isTerminal(initialState), false);

    for(Role role : roles){
        QList<Move> legalMoves = psm.getLegalMoves(initialState, role);
        if(role.getTerm()->toString() == QString("red")){
            QCOMPARE(legalMoves.size(), 8);
        }
        else{
            QCOMPARE(legalMoves.size(), 1);
        }
    }

    Parser parser;
    LTerm moveTerm1 = parser.parseTerm("(drop 6)");
    LTerm moveTerm2 = parser.parseTerm("(noop)");

    QList<Move> moves;
    moves.append(Move(moveTerm1));
    moves.append(Move(moveTerm2));

    MachineState nextState = psm.getNextState(initialState, moves);
    QCOMPARE(nextState.getContents().size(), 2);

    bool isCorrect = false;
    for(LRelation relation : nextState.getContents()){
        if(relation->toString() == "(base (cell 6 1 red))"){
            isCorrect = true;
        }
    }
    QVERIFY(isCorrect);
}

