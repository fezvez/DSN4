#include "mainwidget.h"

#include <QVBoxLayout>

MainWidget::MainWidget()
{
    fileWidget = new KifWidget();
    playerWidget = new PlayerWidget();
    serverWidget = new ServerWidget();

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(fileWidget, tr("File"));
    tabWidget->addTab(playerWidget, tr("Player"));
    tabWidget->addTab(serverWidget, tr("Server"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);

    setLayout(layout);
    resize(1120, 800);

    // Choose which tab you show at startup
    tabWidget->setCurrentIndex(0);

    setWindowTitle("Dynamic Semantic Net");
}

MainWidget::~MainWidget()
{

}

