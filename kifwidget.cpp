#include "kifwidget.h"
#include "ui_widget.h"
#include "fileloader.h"
#include "kifloader.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QFileDialog>
#include <QProgressDialog>

#include <QTextStream>
#include <QDebug>
#include <QTextCursor>

#include "Unification/unification_relation.h"
#include "Prover/gdlprover.h"

//
KifWidget::KifWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    afac = 0;
//    ui->setupUi(this);
    setUpLayout();
    initialize();
}

KifWidget::~KifWidget()
{
    delete ui;
}


/**
 * LAYOUT
 */
void KifWidget::setUpLayout(){
    // Widgets

    // Top menu
    labelTopMenu = new QLabel("Select a .kif file and there should be some static analysis on it", this);

    // Left menu
    browseButton = createButton(tr("&Browse..."), SLOT(browse()));
    findButton = createButton(tr("&Find"), SLOT(find()));

    lineEditFindFile = new QLineEdit(this);
    lineEditFindFile->setText("*");

    directoryComboBox = createComboBox("");
    //directoryComboBox = createComboBox(QDir::currentPath());

    fileLabel = new QLabel(tr("Named:"));
    directoryLabel = new QLabel(tr("In directory:"));
    filesFoundLabel = new QLabel;

    leftMenuGroupBox = new QGroupBox(tr("Left Menu"), this);
    topMenuGroupBox = new QGroupBox(tr("Top menu"), this);
    textEditGroupBox = new QGroupBox(tr("Text edit"), this);

    createFilesTable();
    createMainDisplay();


    // Layout
    QGridLayout *leftMenuLayout = new QGridLayout(this);
    leftMenuLayout->addWidget(fileLabel, 0, 0);
    leftMenuLayout->addWidget(lineEditFindFile, 0, 1, 1, 2);
    leftMenuLayout->addWidget(directoryLabel, 1, 0);
    leftMenuLayout->addWidget(directoryComboBox, 1, 1);
    leftMenuLayout->addWidget(browseButton, 1, 2);
    leftMenuLayout->addWidget(filesTable, 2, 0, 1, 3);
    leftMenuLayout->addWidget(filesFoundLabel, 3, 0, 1, 2);
    leftMenuLayout->addWidget(findButton, 3, 2);
    leftMenuGroupBox->setLayout(leftMenuLayout);

    QHBoxLayout *topMenuLayout = new QHBoxLayout(this);
    topMenuLayout->addWidget(labelTopMenu);
    topMenuGroupBox->setLayout(topMenuLayout);

    QGridLayout *mainDisplayLayout = new QGridLayout(this);
    mainDisplayLayout->addWidget(tabWidget,0,0,1,1);
    textEditGroupBox->setLayout(mainDisplayLayout);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(leftMenuGroupBox,1,0,2,1);
    layout->addWidget(topMenuGroupBox,0,0,1,3);
    layout->addWidget(textEditGroupBox,1,1,2,2);
    this->setLayout(layout);
}

QPushButton* KifWidget::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

QComboBox* KifWidget::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox();
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    return comboBox;
}

void KifWidget::createFilesTable()
{
    filesTable = new QTableWidget(0, 2);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(true);
    filesTable->setMinimumWidth(360);

    connect(filesTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(openFileOfItem(int,int)));
}

void KifWidget::createMainDisplay(){
    textEditMain = new QTextEdit(this);
    textEditMain->setFont(QFont("Courier"));
    textEditMain->setLineWrapMode(QTextEdit::NoWrap);

    textEditStaticFile = new QTextEdit(this);
    textEditStaticFile->setFont(QFont("Courier"));
    textEditStaticFile->setReadOnly(true);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(textEditMain, tr("Main"));
    tabWidget->addTab(textEditStaticFile, tr("File"));

    tabWidget->setMinimumSize(640, 360);
}

/**
 * INIT
 */
void KifWidget::initialize(){
    regEndsInKif = QRegExp("\\.kif$");

    textEditMain->append(tr("  --------------------------- HELLO! ---------------------------  \n"));
    connect(lineEditFindFile, SIGNAL(textChanged(QString)), this, SLOT(find()));
    connect(directoryComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(find()));
    find();

    connect(this, SIGNAL(kifProcessed(QStringList)), this, SLOT(debugFile(QStringList)));

    //connect(this, SIGNAL(kifProcessed(QStringList)), this, SLOT(loadKif(QStringList)));

//    connect(parser.data(), SIGNAL(output(QString)), textEditMain, SLOT(append(QString)));
//    connect(parser.data(), SIGNAL(outputDebug(QString)), textEditDebug, SLOT(append(QString)));


}

void KifWidget::debugFile(QStringList stringList){
    QTextCursor cursor = textEditStaticFile->textCursor();
    cursor.setPosition(0);
    textEditStaticFile->setTextCursor(cursor);


    PParser parser = PParser(new Parser(this));
    parser->loadKif(stringList);

    GDLProver prover;
    prover.setup(parser->getRelations(), parser->getRules());

    output("Debugging the file");
}



/**
 * FILE LOGIC
 */

void KifWidget::openFileOfItem(int row, int /* column */)
{
    afac++;
    tabWidget->setCurrentIndex(1);
    textEditStaticFile->clear();

    QTableWidgetItem *item = filesTable->item(row, 0);
    //QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
    QString filename(currentDir.absoluteFilePath(item->text()));

    output(QString("Opening file : ").append(item->text()));
    if(filename.contains(regEndsInKif)){
        // Amusing, no RAII here
        // It seems dangerous, but I should read more about RAII and threads
        KifLoader *kifLoader = new KifLoader(this, filename);
        connect(kifLoader, &KifLoader::lineProcessed, textEditStaticFile, &QTextEdit::append);
        connect(kifLoader, &KifLoader::finished, kifLoader, &QObject::deleteLater);
        connect(kifLoader, SIGNAL(kifProcessed(QStringList)),this, SIGNAL(kifProcessed(QStringList)));
        connect(kifLoader, SIGNAL(emitOutput(QString)), this, SLOT(output(QString)));
        kifLoader->start();
    }
    else{
        FileLoader *fileLoader = new FileLoader(this, filename);
        connect(fileLoader, &FileLoader::lineProcessed, textEditStaticFile, &QTextEdit::append);
        connect(fileLoader, &FileLoader::finished, fileLoader, &QObject::deleteLater);
        fileLoader->start();
    }


}

void KifWidget::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Find Files"), QDir::currentPath());

    if (!directory.isEmpty()) {
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}


static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}


void KifWidget::find()
{

    afac++;
    filesTable->setRowCount(0);

    QString filename = lineEditFindFile->text();
    if (filename.isEmpty()){
        filename = "*";
    }
    QRegExp fileRegExp(filename);
    if(filename.contains("*")){
        fileRegExp.setPatternSyntax(QRegExp::Wildcard);
    }

    QString path = directoryComboBox->currentText();


//    updateComboBox(fileComboBox);
    updateComboBox(directoryComboBox);


    currentDir = QDir(path);
#ifdef TARGET_OS_MAC
    currentDir.cdUp();
    currentDir.cdUp();
    currentDir.cdUp();
#endif

    QStringList filesTemp;

    filesTemp = currentDir.entryList(QStringList(), QDir::Files | QDir::NoSymLinks);

    QStringList files;
    for(QString file : filesTemp){
        if(!file.contains(regEndsInKif)){
            continue;
        }
        if( !file.contains(filename) && !(fileRegExp.indexIn(file) != -1) && !fileRegExp.exactMatch(file) ){
            continue;
        }
        files.append(file);

    }

    showFiles(files);
}




void KifWidget::showFiles(const QStringList &files)
{
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
    filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()) +
                             (" (Double click on a file to open it)"));
    filesFoundLabel->setWordWrap(true);
}

void KifWidget::output(const QString & string){
    textEditMain->append(string);
}

