#include "server.h"

#include <QStringBuilder>
#include <QDateTime>


QRegularExpression Server::rxStatusAvailable = QRegularExpression("(status\\s+available)");
QRegularExpression Server::rxReady = QRegularExpression("ready");
QRegularExpression Server::rxPlayerName = QRegularExpression("\\(\\s*name\\s+(\\S+)\\s*\\)");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Server::Server()
{
    matchId = QString("x") % QString::number(QDateTime::currentMSecsSinceEpoch());
    waitingForMessage = WAITING_FOR_MESSAGE::NOTHING;
}

Server::~Server()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// GETTERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    //    qDebug() << "Server::getRoles()";
    return roles;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// SETUP OF THE GAME
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Server::setupGame(QString filename){
    //    qDebug() << "Server::setupGame() " << filename;

    if(waitingForMessage != WAITING_FOR_MESSAGE::NOTHING){
        return;
    }

    parser.generateHerbrandFromFile(filename);
    gameString = parser.getGameString();

    //    qDebug() << "GameString : " << gameString;

    proverSM.initialize(parser.getRelations(), parser.getRules());
    currentState = proverSM.getInitialState();

    emit emitOutput(QString("\nLoading a new game : %1").arg(filename));

    roles.clear();
    QVector<Role> rolesTerm = proverSM.getRoles();
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

    if(waitingForMessage != WAITING_FOR_MESSAGE::NOTHING){
        return;
    }

    addresses = adressesList;
    ports = portList;

    int nbPlayers = addresses.size();
    Q_ASSERT(ports.size() == nbPlayers);

    // Add additional players if need be
    for(int i = networkConnections.size(); i<nbPlayers; ++i){
        qDebug() << "Creation of a new server network object";
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

    qDebug() << "Server::setupPlayers is done, Nb players : " << nbPlayers;
}

void Server::setupClock(int startClock, int playClock){
    //    qDebug() << "Server::setupClock()";

    if(waitingForMessage != WAITING_FOR_MESSAGE::NOTHING){
        return;
    }

    startclock = startClock;
    playclock = playClock;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// SLOTS TO HANDLE THE PHASES : PING, START, PLAY, STOP, ABORT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Server::ping(){
    if(waitingForMessage != WAITING_FOR_MESSAGE::NOTHING){
        return;
    }
    //        qDebug() << "Server::ping()";
    emit emitOutput(QString("\nPING sent to all players"));
    waitForNewMessage(WAITING_FOR_MESSAGE::INFO);

    int nbPlayers = addresses.size();
    for(int i = 0; i<nbPlayers; ++i){
        networkConnections[i]->request("(info)", 5);
    }
}

void Server::start(){
    if(waitingForMessage != WAITING_FOR_MESSAGE::NOTHING){
        return;
    }

    waitForNewMessage(WAITING_FOR_MESSAGE::START);

    stepCounter = 0;    // Incrementation is in handleTransition()
    currentState = proverSM.getInitialState(); // Update handled in handleTransition()

    emit emitOutput(QString("\nMETAGAME : "));

    int nbPlayers = addresses.size();
    QString messageStart = QString("(start ") % matchId % " ";
    QString messageEnd = gameString % ") " % QString::number(startclock) % " " % QString::number(playclock) % ")";
    for(int i = 0; i<nbPlayers; ++i){
        QString message = messageStart % roles[i] % " (" % messageEnd;
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
    emit matchFinished(proverSM.getGoals(currentState));

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
    qDebug() << "Server::abort()";

    waitForNewMessage(WAITING_FOR_MESSAGE::NOTHING);

    int nbPlayers = addresses.size();
    QString message = QString("(abort ") % matchId % ")";

    for(int i = 0; i<nbPlayers; ++i){
        networkConnections[i]->request(message, startclock);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ?
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Server::waitForNewMessage(WAITING_FOR_MESSAGE type){
    //    qDebug() << "wait for message ";// << type;
    waitingForMessage = type;
    for(int i = 0; i<networkAnswers.size(); ++i){
        networkAnswers[i] = "";
    }
    nbNetworkAnswers = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// TWO WAYS TO HANDLE MESSAGE : EITHER IT'S A STRING, OR IT'S AN ERROR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    {
        auto match = rxPlayerName.match(message);
        if(match.hasMatch()){
            emit playerName(playerIndex, match.captured(1));
        }
        match = rxStatusAvailable.match(message);
        if(match.hasMatch()){
            emit playerAvailable(playerIndex, true);
        }
        else{
            emit playerAvailable(playerIndex, false);
        }
        break;
    }
    case(WAITING_FOR_MESSAGE::START):
    {
        auto match = rxReady.match(message);
        if(match.hasMatch()){
            emit playerReady(playerIndex, true);
        }
        else{
            emit playerReady(playerIndex, false);
        }
        break;
    }
    case(WAITING_FOR_MESSAGE::PLAY):
        moves[playerIndex] = message;
        emit outputPlayerMessage(playerIndex, message);
        break;
    case(WAITING_FOR_MESSAGE::DONE):
        if(message != "done"){
            qDebug() << "WOLOLO";
            manageError(address, port, QAbstractSocket::UnknownSocketError);
        }
        emit outputPlayerMessage(playerIndex, message);
        break;
    default:
        break;
    }

    //
    if(nbNetworkAnswers == addresses.size() && waitingForMessage != WAITING_FOR_MESSAGE::NOTHING){
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ONCE WE HAVE A MESSAGE/ERROR FROM EACH PLAYER, WE CAN TRANSITION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Server::handleTransition(){
    switch(waitingForMessage){
    case(WAITING_FOR_MESSAGE::INFO):
        waitingForMessage = WAITING_FOR_MESSAGE::NOTHING;
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
    default:
        break;
    }
}

void Server::computeNextTurn(){

    QList<Move> jointMove;
    QList<Move> randomJointMove = proverSM.getRandomLegalJointMove(currentState);


    for(int i = 0; i<moves.size(); ++i){
        QString potentialMove = moves[i];
        if(potentialMove.isEmpty() || !(isLegal(i, potentialMove))){
            jointMove.append(randomJointMove[i]);
            qDebug() << "Move received is wrong : " << potentialMove;
            qDebug() << "Generated random move for player " << i << " : " << randomJointMove[i].toString();
        }
        else{
            jointMove.append(proverSM.getMoveFromString(potentialMove));
        }
    }

    currentState = proverSM.getNextState(currentState, jointMove);

    emit emitOutput(QString("Server::computeNextTurn() : ").append(currentState.toString()));
}

bool Server::isLegal(int playerIndex, QString message){
    //    qDebug() << "isLegal() : " << message;
    Move move = proverSM.getMoveFromString(message);
    //    qDebug() << move.getTerm()->toString();

    QList<Move> legalMoves = proverSM.getLegalMoves(currentState, proverSM.getRoles()[playerIndex]);
    for(Move potentialMove : legalMoves){
        //        qDebug() << potentialMove.getTerm()->toString();
        if(move == potentialMove){
            return true;
        }
    }
    return false;
}

bool Server::isTerminal(){
    return proverSM.isTerminal(currentState);
}

int Server::getGoal(int playerIndex){
    return proverSM.getGoal(currentState, playerIndex);
}
