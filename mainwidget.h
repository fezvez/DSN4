#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QTabWidget>

#include "kifwidget.h"
#include "playerwidget.h"
#include "serverwidget.h"

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget();
    ~MainWidget();

private:
    QTabWidget *tabWidget;
    KifWidget *fileWidget;
    PlayerWidget *playerWidget;
    QWidget *serverWidget;
};

#endif // MAINWIDGET_H
