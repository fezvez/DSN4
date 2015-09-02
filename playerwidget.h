#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMap>
#include <QTextEdit>

#include "Player/player.h"

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
    QMap<int, Player*> players;
};

#endif // PLAYERWIDGET_H
