#include "serverwidget.h"

#include <QHeaderView>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QStringBuilder>
#include <QMessageBox>

#include "parser.h"
#include "Prover/gdlprover.h"

// WARNING : I keep on invoking several methods in a row, in an object in another thread
// Because I rely on calling these methods in order, it WILL fail one day

ServerWidget::ServerWidget()
{
    qDebug() << "Thread ServerWidget " << thread();
    regEndsInKif = QRegExp("\\.kif$");
    server = new Server();
    serverThread = new QThread(this);
    qDebug() << "Thread server " << server->thread();
    server->moveToThread(serverThread);
    qDebug() << "Thread server " << server->thread();
    serverThread->start();


    setupGUI();
    setupSignalsSlots();


}

ServerWidget::~ServerWidget()
{

}

void ServerWidget::setupGUI(){
    filesTable = new QTableWidget(0,2,this);
    textEdit = new QTextEdit(this);
    inputWidget = new QWidget(this);


    labelStartclock = new QLabel(tr("Startclock"), inputWidget);
    labelPlayclock = new QLabel(tr("Playclock"), inputWidget);
    lineEditStartclock = new QLineEdit("10", inputWidget);
    lineEditPlayclock = new QLineEdit("10", inputWidget);

    playersWidget = new QWidget(inputWidget);  // Starts empty

    pingButton = new QPushButton("Ping", inputWidget);
    startButton = new QPushButton("Start", inputWidget);
    abortButton = new QPushButton("Abort", inputWidget);

    pingButton->setDisabled(true);
    startButton->setDisabled(true);
    abortButton->setDisabled(true);


    // Input area
    QVBoxLayout * inputLayout = new QVBoxLayout();
    QHBoxLayout * clockLayout = new QHBoxLayout();
    QHBoxLayout * buttonLayout = new QHBoxLayout();

    // If a layout is not top level, it must be added to its parent layout before using it
    inputLayout->addLayout(clockLayout);
    inputLayout->addWidget(playersWidget);
    inputLayout->addLayout(buttonLayout);

    inputWidget->setLayout(inputLayout);

    clockLayout->addWidget(labelStartclock);
    clockLayout->addWidget(lineEditStartclock);
    clockLayout->addWidget(labelPlayclock);
    clockLayout->addWidget(lineEditPlayclock);

    playersWidgetLayout = new QGridLayout(playersWidget);
    playersWidget->setLayout(playersWidgetLayout);


    buttonLayout->addWidget(pingButton);
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(abortButton);


    // Global layout
    QGridLayout * layout = new QGridLayout(this);
    layout->addWidget(filesTable,0,0,2,1);
    layout->addWidget(inputWidget,0,1,1,1);
    layout->addWidget(textEdit,1,1,1,1);
    layout->setColumnStretch(0,2);
    layout->setColumnStretch(1,3);
    setLayout(layout);



    setupFilesTable();
    findAndDisplayFiles();
}

void ServerWidget::setupSignalsSlots(){
    connect(pingButton, SIGNAL(clicked()), this, SLOT(ping()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(abortButton, SIGNAL(clicked()), this, SLOT(abort()));
    connect(server, SIGNAL(emitError(int,QAbstractSocket::SocketError)), this, SLOT(displayError(int,QAbstractSocket::SocketError)));
    connect(server, SIGNAL(playerAvailable(int,bool)), this, SLOT(playerAvailable(int,bool)));
    connect(server, SIGNAL(playerReady(int,bool)), this, SLOT(playerReady(int,bool)));
    connect(server, SIGNAL(playerName(int,QString)), this, SLOT(setPlayerName(int,QString)));
    connect(server, SIGNAL(outputPlayerMessage(int,QString)), this, SLOT(displayPlayerMessage(int,QString)));
    connect(server, SIGNAL(emitOutput(QString)), textEdit, SLOT(append(QString)));
    connect(server, SIGNAL(matchFinished(QList<int>)), this, SLOT(playerGoals(QList<int>)));
    connect(server, SIGNAL(done()), this, SLOT(done()));

}

void ServerWidget::displayError(int playerIndex, QAbstractSocket::SocketError socketError){
    qDebug() << "Error for player " << playerIndex;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << "The host closed the connection";
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "The host was not found. Please check the "
                    "host name and port settings.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "The connection was refused by the peer. Is the player online?";
        break;
    case QAbstractSocket::SocketTimeoutError:
        qDebug() << "The connection timeouted";
        break;
    default:
        qDebug() << "Unknown error occurred:";
    }

    switch (socketError) {

    case QAbstractSocket::SocketTimeoutError:
        displayPlayerMessage(playerIndex, "Timeout");
        break;
    default:
        displayPlayerMessage(playerIndex, "Error");
    }
}

void ServerWidget::displayPlayerMessage(int playerIndex, QString message){
    textEdit->append(QString("Player %1 : %2").arg(playerIndex).arg(message));
}

void ServerWidget::playerAvailable(int playerIndex, bool isAvailable){
    Q_ASSERT(playerIndex >= 0 && playerIndex < nbRoles);
    //    qDebug() << "ServerWidget::playerReady()";
    QString indexString = QString::number(playerIndex);
    if(isAvailable){
        labelStatus[playerIndex]->setText("OK") ;
        QString message = QString("PING : Player ") % indexString % " is available";
        textEdit->append(message);
    }
    else{
        labelStatus[playerIndex]->setText("NO");
        QString message = QString("PING : Player ") % indexString % " is not available";
        textEdit->append(message);
    }

}

void ServerWidget::playerReady(int playerIndex, bool isReady){
    Q_ASSERT(playerIndex >= 0 && playerIndex < nbRoles);
    //    qDebug() << "ServerWidget::playerReady()";
    QString indexString = QString::number(playerIndex);
    if(isReady){
        labelStatus[playerIndex]->setText("OK") ;
        QString message = QString("Metagame : Player ") % indexString % " is ready";
        textEdit->append(message);
    }
    else{
        labelStatus[playerIndex]->setText("NO");
        QString message = QString("Metagame : Player ") % indexString % " was not ready on time";
        textEdit->append(message);
    }

}

void ServerWidget::setPlayerName(int playerIndex, QString name){
    Q_ASSERT(playerIndex >= 0 && playerIndex < nbRoles);
    labelPlayerInstructions[playerIndex]->setText(QString("IP/port for %1 : ").arg(name));
}

void ServerWidget::playerGoals(QList<int> goals){
    Q_ASSERT(goals.size() == nbRoles);
    for(int i = 0; i<goals.size(); ++i){
        labelStatus[i]->setText(QString::number(goals[i]));
    }
}

void ServerWidget::output(QString s){
    textEdit->append(s);
}

void ServerWidget::ping(){
    textEdit->append("\nPING");

    QStringList adressList = getAdressList();
    QVector<int> portList = getPortList();

    adressList = adressList.mid(0, nbRoles);
    portList = portList.mid(0, nbRoles);

    QMetaObject::invokeMethod(server, "setupPlayers", Qt::AutoConnection,
                              Q_ARG(QStringList, adressList),
                              Q_ARG(QVector<int>, portList));
    QMetaObject::invokeMethod(server, "ping", Qt::AutoConnection);

    // Direct calls like this would execute the function in the current thread
//    server->setupPlayers(adressList, portList);
//    server->ping();
}

void ServerWidget::start(){
    textEdit->append("\nSTART");

    for(QLabel* label : labelStatus){
        label->setText("-");
    }

    QStringList adressList = getAdressList();
    QVector<int> portList = getPortList();

    QMetaObject::invokeMethod(server, "setupPlayers", Qt::AutoConnection,
                              Q_ARG(QStringList, adressList),
                              Q_ARG(QVector<int>, portList));
    QMetaObject::invokeMethod(server, "setupClock", Qt::AutoConnection,
                              Q_ARG(int, lineEditStartclock->text().toInt()),
                              Q_ARG(int, lineEditPlayclock->text().toInt()));

//    server->setupPlayers(adressList, portList);
//    server->setupClock(lineEditStartclock->text().toInt(), lineEditPlayclock->text().toInt());

    pingButton->setDisabled(true);
    startButton->setDisabled(true);
    abortButton->setDisabled(false);

    QMetaObject::invokeMethod(server, "start", Qt::AutoConnection);
//    server->start();
}

void ServerWidget::abort(){
    qDebug() << "Abort";
    pingButton->setDisabled(false);
    startButton->setDisabled(false);
    abortButton->setDisabled(true);
}

// A bit different, is launched after all the player return "done"
void ServerWidget::done(){
    textEdit->append("\nDONE");

    pingButton->setDisabled(false);
    startButton->setDisabled(false);
    abortButton->setDisabled(true);
}

QStringList ServerWidget::getAdressList(){
    QStringList answer;
    for(QLineEdit* lineEdit : lineEditIP){
        answer << lineEdit->text();
    }
    return answer;
}

QVector<int> ServerWidget::getPortList(){
    QVector<int> answer;
    for(QLineEdit* lineEdit : lineEditPort){
        answer << lineEdit->text().toInt();
    }
    return answer;
}


// INIT

void ServerWidget::setupFilesTable()
{
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(true);
    filesTable->setMinimumWidth(360);

    connect(filesTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(openFile(int)));
}

void ServerWidget::findAndDisplayFiles()
{
    filesTable->setRowCount(0);

    currentDir = QDir::currentPath();
#ifdef TARGET_OS_MAC
    currentDir.cdUp();
    currentDir.cdUp();
    currentDir.cdUp();
    currentDir.cdUp();
#endif
    output(QString("Directory analyzed by the Server : ").append(currentDir.absolutePath()));

    QStringList filesTemp = currentDir.entryList(QStringList(), QDir::Files | QDir::NoSymLinks);
    QStringList files;
    for(QString file : filesTemp){
        if(!file.contains(regEndsInKif)){
            continue;
        }
        files.append(file);
    }

    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                                          .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }
}

void ServerWidget::openFile(int row)
{
    //    qDebug() << "OPEN FILE";
    QTableWidgetItem *item = filesTable->item(row, 0);
    QString filename(currentDir.absoluteFilePath(item->text()));


    QMetaObject::invokeMethod(server, "setupGame", Qt::DirectConnection,
                              Q_ARG(QString, filename));
//    server->setupGame(filename);


    QStringList roles;
    QMetaObject::invokeMethod(server, "getRoles", Qt::DirectConnection,
                              Q_RETURN_ARG(QStringList, roles));
    // QStringList roles = server->getRoles();
    qDebug() << "Nb roles " << roles.size();
    nbRoles = roles.size();
    setupPlayers(roles);
}

void ServerWidget::setupPlayers(QStringList roles){
    int nbCurrentPlayers = labelPlayerInstructions.size();
    for(int i = nbCurrentPlayers; i<nbRoles; ++i){
        labelPlayerInstructions.append(new QLabel(tr("IP/port for player : "), playersWidget));
        labelRoles.append(new QLabel(playersWidget));
        lineEditIP.append(new QLineEdit(playersWidget));
        lineEditPort.append(new QLineEdit(playersWidget));
        labelStatus.append(new QLabel(playersWidget));

        playersWidgetLayout->addWidget(labelPlayerInstructions[i], i, 0, 1, 1);
        playersWidgetLayout->addWidget(labelRoles[i], i, 1, 1, 1);
        playersWidgetLayout->addWidget(lineEditIP[i], i, 2, 1, 1);
        playersWidgetLayout->addWidget(lineEditPort[i], i, 3, 1, 1);
        playersWidgetLayout->addWidget(labelStatus[i], i, 4, 1, 1);
    }

    for(int i = 0; i<nbRoles; ++i){
        labelRoles[i]->setText(roles[i]);
        lineEditIP[i]->setText("127.0.0.1");
        lineEditPort[i]->setText(QString::number(9147+i));
        labelStatus[i]->setText("-");

        labelPlayerInstructions[i]->show();
        labelRoles[i]->show();
        lineEditIP[i]->show();
        lineEditPort[i]->show();
        labelStatus[i]->show();
    }
    for(int i = nbRoles; i<nbCurrentPlayers; ++i){
        labelPlayerInstructions[i]->hide();
        labelRoles[i]->hide();
        lineEditIP[i]->hide();
        lineEditPort[i]->hide();
        labelStatus[i]->hide();
    }

    pingButton->setDisabled(false);
    startButton->setDisabled(false);
    abortButton->setDisabled(true);
}
