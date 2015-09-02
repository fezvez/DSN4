#include "serverwidget.h"

#include <QHeaderView>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QStringBuilder>
#include <QMessageBox>

#include "kifloader.h"
#include "parser.h"
#include "Prover/gdlprover.h"

ServerWidget::ServerWidget()
{
    regEndsInKif = QRegExp("\\.kif$");
    server = new Server();
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
    connect(server, SIGNAL(emitError(QString,int,QAbstractSocket::SocketError)), this, SLOT(displayError(QString,int,QAbstractSocket::SocketError)));
    connect(server, SIGNAL(playerReady(int,bool)), this, SLOT(playerReady(int,bool)));
    connect(server, SIGNAL(playerName(int,QString)), this, SLOT(setPlayerName(int,QString)));
}

void ServerWidget::displayError(QString ip, int port, QAbstractSocket::SocketError socketError){
       qDebug() << "Error for ip/port " << ip << ":" << port;
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
}

void ServerWidget::playerReady(int playerIndex, bool isReady){
//    qDebug() << "ServerWidget::playerReady()";
    QString indexString = QString::number(playerIndex);
    if(isReady){
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

void ServerWidget::setPlayerName(int playerIndex, QString name){
    labelPlayerInstructions[playerIndex]->setText(QString("IP/port for %1 : ").arg(name));
}

void ServerWidget::output(QString s){
    textEdit->append(s);
}

void ServerWidget::ping(){
    textEdit->append("\nPING");

    QStringList adressList = getAdressList();
    QVector<int> portList = getPortList();
    server->setupPlayers(adressList, portList);

    server->ping();
}

void ServerWidget::start(){
    textEdit->append("\nSTART");

    for(QLabel* label : labelStatus){
        label->setText("-");
    }

    QStringList adressList = getAdressList();
    QVector<int> portList = getPortList();
    server->setupPlayers(adressList, portList);
    server->setupClock(lineEditStartclock->text().toInt(), lineEditPlayclock->text().toInt());

    pingButton->setDisabled(true);
    startButton->setDisabled(true);
    abortButton->setDisabled(false);

    server->start();
}

void ServerWidget::abort(){
    qDebug() << "Abort";
    pingButton->setDisabled(false);
    startButton->setDisabled(false);
    abortButton->setDisabled(true);
}

void ServerWidget::stop(){
    qDebug() << "Stop";
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

    server->setupGame(filename);
    QStringList roles = server->getRoles();
    setupPlayers(roles);
}

void ServerWidget::setupPlayers(QStringList roles){
    int nbRoles = roles.size();
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
