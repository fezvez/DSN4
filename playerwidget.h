#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMap>
#include <QTextEdit>
#include <QThread>

#include "Player/player.h"

class PlayerController : public QObject{

};

class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    PlayerWidget();
    ~PlayerWidget();

public slots:
    void createPlayer();


private:
    QComboBox *playerList;
    QLineEdit *lineEditPort;
    QPushButton *buttonCreate;

    QLabel *labelPlayers;
    QLabel *labelPort;

    QTabWidget *tabPlayers;

    QMap<int, QTextEdit* > playerTextEdit;

    // We want the players to be in their own thread
    // among other things so that the UI is still responsive
    QMap<int, Player*> players;
    QMap<int, QThread*> playerThreads;
};

#endif // PLAYERWIDGET_H
