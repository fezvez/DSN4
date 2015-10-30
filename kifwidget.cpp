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
#include <QSizePolicy>

#include "Unification/unification_relation.h"
#include "Prover/gdlprover.h"

//
KifWidget::KifWidget(QWidget *parent) :
    QWidget(parent)
{
    setUpLayout();
    initialize();
}

KifWidget::~KifWidget()
{

}


/**
 * GUI
 */
void KifWidget::setUpLayout(){
    // Widgets

    // Top menu
    labelTopMenu = new QLabel("Make a query", this);
    lineEditQuery = new QLineEdit("terminal", this);
    checkBoxInit = new QCheckBox("Init ", this);
    queryButton = createButton(tr("&Query"), SLOT(query()));

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
    topMenuGroupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    textEditGroupBox = new QGroupBox(tr("Text edit"), this);
    textEditGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    topMenuLayout->addWidget(lineEditQuery);
    topMenuLayout->addWidget(checkBoxInit);
    topMenuLayout->addWidget(queryButton);
    topMenuGroupBox->setLayout(topMenuLayout);


    QGridLayout *mainDisplayLayout = new QGridLayout(this);
    mainDisplayLayout->addWidget(tabWidget,0,0,1,1);
    textEditGroupBox->setLayout(mainDisplayLayout);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(topMenuGroupBox,0,0,1,3);
    layout->addWidget(leftMenuGroupBox,1,0,2,1);
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
            this, SLOT(openFileFromUserInteraction(int,int)));
}

void KifWidget::createMainDisplay(){
    textEditDebug = new QPlainTextEdit(this);
    textEditDebug->setFont(QFont("Courier"));

    textEditGDL = new QPlainTextEdit(this);
    textEditGDL->setFont(QFont("Courier"));
    textEditGDL->setLineWrapMode(QPlainTextEdit::NoWrap);
    highlighter = new Highlighter(textEditGDL->document());

    textEditQueryAnswer = new QPlainTextEdit(this);
    textEditQueryAnswer->setFont(QFont("Courier"));

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(textEditDebug, tr("Debug"));
    tabWidget->addTab(textEditGDL, tr("GDL"));
    tabWidget->addTab(textEditQueryAnswer, tr("Query"));

    tabWidget->setMinimumSize(640, 360);
//    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



/**
 * INIT
 */
void KifWidget::initialize(){
    regEndsInKif = QRegExp("\\.kif$");

    // Some default text
    output(tr("  --------------------------- HELLO! ---------------------------  \n"));
    textEditQueryAnswer->appendPlainText(tr("No query asked yet"));

    connect(lineEditFindFile, SIGNAL(textChanged(QString)), this, SLOT(find()));
    connect(directoryComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(find()));
    connect(this, SIGNAL(kifProcessed(QStringList)), this, SLOT(debugFile(QStringList)));
    connect(textEditGDL, SIGNAL(textChanged()), this, SLOT(gdlTextChanged()));

    find();
    openFile("tictactoe.kif");
}

void KifWidget::debugFile(QStringList stringList){
    output("STATIC ANALYSIS");

    QTextCursor cursor = textEditGDL->textCursor();
    cursor.setPosition(0);
    textEditGDL->setTextCursor(cursor);

    if(!parser){
        parser = PParser(new Parser(this));
    }

    parser->generateHerbrandFromRawKif(stringList);



    // Check parenthesis
    // Check syntax

    GDLProver prover;
    prover.setup(parser->getRelations(), parser->getRules());

    // Check arity
}



/**
 * FILE LOGIC
 */

void KifWidget::openFileFromUserInteraction(int row, int /* column */)
{
    // Find file name
    QTableWidgetItem *item = filesTable->item(row, 0);
    QString filename(currentDir.absoluteFilePath(item->text()));

    // Give user feedback
    tabWidget->setCurrentIndex(1);

    output("\n--------------------------- LOADING FILE ---------------------------  \n");
    output(QString("File name : ").append(item->text()));

    // Actually open the file
    openFile(filename);
}

void KifWidget::openFile(QString filename){
    //    qDebug() << "file "<< filename;
    textEditGDL->clear();

    if(filename.contains(regEndsInKif)){
        // Amusing, no RAII here (&QObject::deleteLater)
        KifLoader *kifLoader = new KifLoader(this, filename);
        connect(kifLoader, &KifLoader::lineProcessed, textEditGDL, &QPlainTextEdit::appendPlainText);
        connect(kifLoader, &KifLoader::finished, kifLoader, &QObject::deleteLater);
        connect(kifLoader, SIGNAL(kifProcessed(QStringList)),this, SIGNAL(kifProcessed(QStringList)));
        connect(kifLoader, SIGNAL(emitOutput(QString)), this, SLOT(output(QString)));
        kifLoader->start();
    }
    // Should I even do this?
    else{
        FileLoader *fileLoader = new FileLoader(this, filename);
        connect(fileLoader, &FileLoader::lineProcessed, textEditGDL, &QPlainTextEdit::appendPlainText);
        connect(fileLoader, &FileLoader::finished, fileLoader, &QObject::deleteLater);
        fileLoader->start();
    }

    hasGDLChanged = true;
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

void KifWidget::query(){
    qDebug() << "Query";

    QString queryString = lineEditQuery->text();
    qDebug() << "Query string " << queryString;


    if(hasGDLChanged){

        //            kb.setup();
    }
    hasGDLChanged = false;

    QString plainText = textEditGDL->toPlainText();



}

void KifWidget::output(const QString & string){
    textEditDebug->appendPlainText(string);
}

void KifWidget::gdlTextChanged(){
    hasGDLChanged = true;
}



Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    logicQualifierFormat.setForeground(Qt::darkBlue);
    QStringList logicQualifierPatterns;
    logicQualifierPatterns << "\\binit\\b" << "\\btrue\\b" << "\\bbase\\b";
    foreach (const QString &pattern, logicQualifierPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = logicQualifierFormat;
        highlightingRules.append(rule);
    }

    logicKeywordFormat.setForeground(Qt::darkMagenta);
    logicKeywordFormat.setFontWeight(QFont::Bold);
    QStringList logicKeywordPatterns;
    logicKeywordPatterns << "\\bnext\\b" << "\\brole\\b" << "\\bgoal\\b"
                         << "\\bterminal\\b" << "\\blegal\\b" << "\\binput\\b"
                         << "\\bdoes\\b";
    foreach (const QString &pattern, logicKeywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = logicKeywordFormat;
        highlightingRules.append(rule);

    }

    logicSemanticsFormat.setForeground(Qt::darkYellow);
    QStringList logicSemanticsPatterns;
    logicSemanticsPatterns << "\\bdistinct\\b" << "\\bset\\b" << "\\bnot\\b";
    foreach (const QString &pattern, logicSemanticsPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = logicSemanticsFormat;
        highlightingRules.append(rule);
    }

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp(";[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    variableFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\?[A-Za-z0-9_]+");
    rule.format = variableFormat;
    highlightingRules.append(rule);

}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
//    //! [7] //! [8]
//    setCurrentBlockState(0);
//    //! [8]

//    //! [9]
//    int startIndex = 0;
//    if (previousBlockState() != 1)
//        startIndex = commentStartExpression.indexIn(text);

//    //! [9] //! [10]
//    while (startIndex >= 0) {
//        //! [10] //! [11]
//        int endIndex = commentEndExpression.indexIn(text, startIndex);
//        int commentLength;
//        if (endIndex == -1) {
//            setCurrentBlockState(1);
//            commentLength = text.length() - startIndex;
//        } else {
//            commentLength = endIndex - startIndex
//                    + commentEndExpression.matchedLength();
//        }
//        setFormat(startIndex, commentLength, multiLineCommentFormat);
//        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
//    }
}
