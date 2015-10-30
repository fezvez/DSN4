#include "server.h"

#include <QStringBuilder>
#include <QDateTime>


Server::Server()
{
    matchId = QString("x") % QString::number(QDateTime::currentMSecsSinceEpoch());
    waitingForMessage = WAITING_FOR_MESSAGE::NOTHING;

    rxStatusAvailable = QRegExp("(status\\s+available)");
    rxReady = QRegExp("ready");
    rxPlayerName = QRegExp("\\(\\s*name\\s+(\\S+)\\s*\\)");
}

Server::~Server()
{

}

int Server::getPlayerIndex(QString address, int port){
    for(int i = 0; i<addresses.size(); ++i){
        if(addresses[i] == address && ports[i] == port){
            return i;
        }
    }
    qDebug() << "getPlayerIndex() BUG : " << address << " / " << port;
    return -1;
}

QStringList Server::getRoles(){
    qDebug() << "Server::getRoles()";
    return roles;
}

void Server::setupGame(QString filename){
    qDebug() << "Server::setupGame() " << filename;
//    KifLoader kifLoader(nullptr, filename);
//    QStringList gameStringList = kifLoader.runSynchronously();
//    // Much more efficient than looping over the strings and concatenating them
//    gameString = "( " % gameStringList.join(" ") % " )";

//    Parser parser;
//    parser.generateHerbrandFromRawKif(gameStringList);

    prover.initialize(filename);
//    prover.initialize(parser.getRelations(), parser.getRules());
    currentState = prover.getInitialState();

    emit emitOutput(QString("\nLoading a new game"));

    roles.clear();
    QVector<Role> rolesTerm = prover.getRoles();
    for(Role role : rolesTerm){
        roles << role.getTerm()->toString();
        emit emitOutput(QString("This game requires a player to play the role of : %1").arg(roles.last()));
    }

    moves.clear();
    for(int i = 0; i<roles.size(); ++i){
        moves.append("");
    }


}

void Server::setupPlayers(QStringList adressesList, QVector<int> portList){
//    qDebug() << "Server::setupPlayers()";
    addresses = adressesList;
    ports = portList;

    int nbPlayers = addresses.size();
    Q_ASSERT(ports.size() == nbPlayers);

    // Add additional players if need be
    for(int i = networkConnections.size(); i<nbPlayers; ++i){
        qDebug() << "creation";
        networkConnections.append(new ServerNetwork(this));
        connect(networkConnections.last(), SIGNAL(emitError(QString, int, QAbstractSocket::SocketError)),
                this, SLOT(manageError(QString,int,QAbstractSocket::SocketError)));
        connect(networkConnections.last(), SIGNAL(emitMessage(QString, int, QString)),
                this, SLOT(processMessage(QString,int,QString)), Qt::DirectConnection); // I don't know why a Queued Connection loses the message. I really don't

        networkAnswers.append(QString(""));
    }

    for(int i = 0; i< nbPlayers; ++i){
        ServerNetwork* network = networkConnections[i];
        network->setPlayerIP(addresses[i]);
        network->setPlayerPort(ports[i]);

        networkAnswers[i] = "";
    }

    qDebug() << "Thread Server " << thread();
}

void Server::setupClock(int startClock, int playClock){
    startclock = startClock;
    playclock = playClock;
}

void Server::ping(){
        qDebug() << "Server::ping()";
    waitForNewMessage(WAITING_FOR_MESSAGE::INFO);

    int nbPlayers = addresses.size();
    for(int i = 0; i<nbPlayers; ++i){
        networkConnections[i]->request("(info)", 5);
    }
}

void Server::start(){
    waitForNewMessage(WAITING_FOR_MESSAGE::START);

    stepCounter = 0;    // Incrementation is in handleTransition()
    currentState = prover.getInitialState(); // Update handeld in handleTransition()

    emit emitOutput(QString("\nMETAGAME"));

    int nbPlayers = addresses.size();
    QString messageStart = QString("(start ") % matchId % " ";
    QString messageEnd = gameString % " " % QString::number(startclock) % " " % QString::number(playclock) % ")";
    for(int i = 0; i<nbPlayers; ++i){
        QString message = messageStart % roles[i] % messageEnd;
        networkConnections[i]->request(message, startclock);
    }
}

void Server::play(){
    waitForNewMessage(WAITING_FOR_MESSAGE::PLAY);

        emit emitOutput(QString("\nPLAY step %1").arg(stepCounter));


    QThread::currentThread()->msleep(500);

    QString message = QString("(play ") % matchId % " ";
    if(stepCounter == 0){
        message += "nil)";
    }
    else{
//        bool singlePlayer = (moves.size() == 1);

//        if(!singlePlayer){
            message += "( ";
//        }

        for(QString move : moves){
            message += move % " ";
        }
//        if(!singlePlayer){
                    message += QString(") ");
//        }
        message += ")";
    }

    int nbPlayers = addresses.size();
    for(int i = 0; i<nbPlayers; ++i){
        networkConnections[i]->request(message, startclock);
    }

}

void Server::stop(){
    qDebug() << "Server::stop()";

    waitForNewMessage(WAITING_FOR_MESSAGE::DONE);

    emit emitOutput(QString("\nMATCH FINISHED"));
    emit matchFinished(prover.getGoals(currentState));

    QThread::currentThread()->msleep(500);


    int nbPlayers = addresses.size();
    QString message = QString("(stop ") % matchId % " ";

    // Likely to be not necessary
    if(stepCounter == 0){
        message += "nil)";
    }
    else{
        message += "( ";
        for(QString move : moves){
            message += QString("(") % move % ") ";
        }
        message += QString(") )");
    }

    for(int i = 0; i<nbPlayers; ++i){
        networkConnections[i]->request(message, startclock);
        emit outputPlayerMessage(i, QString("Score %1").arg(getGoal(i)));
    }

}

void Server::abort(){


}

void Server::waitForNewMessage(WAITING_FOR_MESSAGE type){
//    qDebug() << "wait for message ";// << type;
    waitingForMessage = type;
    for(int i = 0; i<networkAnswers.size(); ++i){
        networkAnswers[i] = "";
    }
    nbNetworkAnswers = 0;
}

void Server::processMessage(QString address, int port, QString message){
//    qDebug() << "\nServer::processMessage()";
    int playerIndex = getPlayerIndex(address, port);
    Q_ASSERT(playerIndex >=0);

    if(networkAnswers[playerIndex].isEmpty()){
        nbNetworkAnswers++;
    }
    else{
        qDebug() << "Server::processMessage() : SUPER WEIRD, player sent first message "
                 << networkAnswers[playerIndex] << " then message " <<  message;
    }
    networkAnswers[playerIndex] = message;
//    qDebug() << "Player " << playerIndex << " : " << message;

    // Send immediate feedback
    switch(waitingForMessage){
    case(WAITING_FOR_MESSAGE::INFO):
        if(rxPlayerName.indexIn(message)>=0){
            emit playerName(playerIndex, rxPlayerName.cap(1));
        }
        if(rxStatusAvailable.indexIn(message) >= 0){
            emit playerAvailable(playerIndex, true);
        }
        else{
            emit playerAvailable(playerIndex, false);
        }
        break;
    case(WAITING_FOR_MESSAGE::START):
        if(rxReady.indexIn(message) >= 0){
            emit playerReady(playerIndex, true);
        }
        else{
            emit playerReady(playerIndex, false);
        }
        break;
    case(WAITING_FOR_MESSAGE::PLAY):
        moves[playerIndex] = message;
        emit outputPlayerMessage(playerIndex, message);
        break;
    case(WAITING_FOR_MESSAGE::DONE):
        if(message != "done"){
            manageError(address, port, QAbstractSocket::UnknownSocketError);
        }
        emit outputPlayerMessage(playerIndex, message);
        break;
    default:
        break;
    }

    //
    if(nbNetworkAnswers == addresses.size()){
        // Finish the job
//        qDebug() << "Server::processMessage() : Got all the messages";
        handleTransition();
    }
}

void Server::manageError(QString address, int port, QAbstractSocket::SocketError error){


    int playerIndex = getPlayerIndex(address, port);

    if(networkAnswers[playerIndex].isEmpty()){
        nbNetworkAnswers++;
    }
    else{
        qDebug() << "Server::manageError() : SUPER WEIRD, player sent first message "
                 << networkAnswers[playerIndex] << " then error " <<  error;
    }

    switch(waitingForMessage){
    case(WAITING_FOR_MESSAGE::INFO):
    case(WAITING_FOR_MESSAGE::START):
        emit playerReady(playerIndex, false);
        break;
    case(WAITING_FOR_MESSAGE::PLAY):
        moves[playerIndex] = "";
        break;
    default:
        break;
    }

    if(nbNetworkAnswers == addresses.size()){
        handleTransition();
    }

    emit emitError(playerIndex, error);
}

void Server::handleTransition(){
    switch(waitingForMessage){
    case(WAITING_FOR_MESSAGE::INFO):
        break;
    case(WAITING_FOR_MESSAGE::START):
        play();
        break;
    case(WAITING_FOR_MESSAGE::PLAY):
        computeNextTurn();
        stepCounter++;
        if(isTerminal()){
            stop();
        }
        else{
            play();
        }
        break;
    case(WAITING_FOR_MESSAGE::DONE):
        emit done();
        break;
    case(WAITING_FOR_MESSAGE::ABORT):
        break;
    default:
        break;
    }
}

void Server::computeNextTurn(){
    qDebug() << "\nServer::computeNextTurn()";
    QList<Move> jointMove;
    QList<Move> randomJointMove = prover.getRandomLegalJointMove(currentState);


    for(int i = 0; i<moves.size(); ++i){
        QString potentialMove = moves[i];
        if(potentialMove.isEmpty() || !(isLegal(i, potentialMove))){
            jointMove.append(randomJointMove[i]);
            qDebug() << "Move received is wrong : " << potentialMove;
            qDebug() << "Generated random move for player " << i << " : " << randomJointMove[i].toString();
        }
        else{
            jointMove.append(prover.getMoveFromString(potentialMove));
        }
    }

    currentState = prover.getNextState(currentState, jointMove);

    qDebug() << "Current state is now : " << currentState.toString();
}

bool Server::isLegal(int playerIndex, QString message){
//    qDebug() << "isLegal() : " << message;
    Move move = prover.getMoveFromString(message);
//    qDebug() << move.getTerm()->toString();

    QList<Move> legalMoves = prover.getLegalMoves(currentState, prover.getRoles()[playerIndex]);
    for(Move potentialMove : legalMoves){
//        qDebug() << potentialMove.getTerm()->toString();
        if(move == potentialMove){
            return true;
        }
    }
    return false;
}

bool Server::isTerminal(){
    return prover.isTerminal(currentState);
}

int Server::getGoal(int playerIndex){
    return prover.getGoal(currentState, playerIndex);
}
