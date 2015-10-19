#include "playerwidget.h"

#include <QGridLayout>
#include <QTextEdit>

#include "Player/firstplayer.h"
#include "Player/montecarlogamer.h"

PlayerWidget::PlayerWidget()
{
    playerList = new QComboBox(this);
    playerList->insertItem(0, "Random");
    playerList->insertItem(1, "MCS");
    lineEditPort = new QLineEdit("9147", this);
    buttonCreate = new QPushButton(tr("Create Player"), this);

    labelPlayers = new QLabel(tr("Player"), this);
    labelPort = new QLabel(tr("Port"), this);

    tabPlayers = new QTabWidget(this);

    QLabel *noPlayerLabel = new QLabel("No players", tabPlayers);
    tabPlayers->addTab(noPlayerLabel, "No player");

    QGridLayout * layout = new QGridLayout(this);
    layout->addWidget(labelPlayers, 0,0);
    layout->addWidget(playerList,0,1);
    layout->addWidget(labelPort, 1,0);
    layout->addWidget(lineEditPort,1,1);
    layout->addWidget(buttonCreate,2,0,1,2);
    layout->addWidget(tabPlayers, 0,2,3,1);

    layout->setColumnStretch(2, 2);

    setLayout(layout);

    QObject::connect(buttonCreate, SIGNAL(clicked()), this, SLOT(createPlayer()));
}

PlayerWidget::~PlayerWidget()
{

}

void PlayerWidget::createPlayer(){

    if(tabPlayers->count() == 1 && tabPlayers->tabText(0) == "No player"){
        tabPlayers->removeTab(0);
    }

    QString portString = lineEditPort->text();
    int port = portString.toInt();

    Player * newPlayer;
    switch(playerList->currentIndex()){
    case(0):
        newPlayer = new FirstPlayer(port);
        break;
    case(1):
        newPlayer = new MonteCarloGamer(port);
        break;
    default:
        newPlayer = new FirstPlayer(port);
        break;
    }

    // The port may have changed
    port = newPlayer->getPort();
    portString = QString::number(port);


    QTextEdit * textEdit = new QTextEdit(this);

    tabPlayers->addTab(textEdit, portString);
    playerTextEdit.insert(port, textEdit);
    connect(newPlayer, SIGNAL(emitOutput(QString)), textEdit, SLOT(append(QString)));


    QString initialString = QString("Player created on port : %1").arg(portString);
    textEdit->append(initialString);
    QString playerNameString = QString("Player name is : %1").arg(newPlayer->getName());
    textEdit->append(playerNameString);

    tabPlayers->setCurrentIndex(tabPlayers->count() - 1);

    players.insert(players.size(), newPlayer);
}
