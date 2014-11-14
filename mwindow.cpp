#include "mwindow.h"
#include <QHeaderView>
#include <QTextStream>
#include <QLineEdit>
#include <QDebug>
#include <QForeachContainer>
#include <QList>
#include <map>
using namespace std;

mwindow::mwindow(QWidget *parent) :
    QMainWindow(parent)
{

    theMenu = new QMenuBar(parent);
    theMenu->setGeometry(0,0,800,25);
    setMenuBar(theMenu);
    statusBar()->showMessage("Loaded.",0);
    menuFile = new QMenu(theMenu);
    actionOpen = new QAction(this);
    actionOpen->setText("&Open");
    actionOpen->setShortcuts(QKeySequence::Open);
    actionSave = new QAction(this);
    actionSave->setText("&Save as");
    actionSave->setShortcuts(QKeySequence::Save);

    theMenu->addAction(menuFile->menuAction());
    menuFile->addAction(actionOpen);
    menuFile->addAction(actionSave);
    menuFile->setTitle("&File");

    theCenter = new QWidget(parent);
    this->setCentralWidget(theCenter);
    theGrid = new QWidget(theCenter);
    theLayout = new QGridLayout(theGrid);
    theGrid->setGeometry(20,20,this->width()*.80,this->height()*.99);

    /*
     *   theTabs belongs to the central widget theCenter;
     */

    theTabs   = new QTabWidget;
    for (uint i = 0; i < 27; i++)  // each tab belongs to theTabs
        tabs[i]      = new QWidget(theTabs);

    /*
     *   Tables and summary belong to tabs
     */

    for (uint i = 0; i < 26; i++)
    {
        tables[i]    = new QTableWidget(tabs[i+1]); // set the parent to the appropriate tabs
        tables[i]->resize(this->width()*.79,this->height()*.79);
    }
    SummLay = new QVBoxLayout(tabs[0]);
    thefinds = new QHBoxLayout;
    findnext = new QPushButton("Find next");
    findval = new QLineEdit();
    findSSButton = new QPushButton("Find in Table", theGrid);
    findss = new QLineEdit(theGrid);
    findExact = new QCheckBox(theGrid);
    findExactL = new QLabel("Find Exact Match",theGrid);
    textpos = 0;
    theSummary = new QTextEdit();
    theSummary->setFontPointSize(14);
    theSummary->setReadOnly(1);
    theSummary->resize(this->width()*.79,this->height()*0.79);
    thefinds->addWidget(findnext);
    thefinds->addWidget(findval);
    SummLay->addWidget(theSummary);
    SummLay->addLayout(thefinds);
    theButtons[0] = new QPushButton("PARSE",theGrid); // Parse, Put into Heap, Put into BST,  Quit, Clear
    theButtons[0]->setShortcut(QKeySequence::Print);
    theButtons[1] = new QPushButton("HEAP",theGrid);
    theButtons[2] = new QPushButton("AVL",theGrid);
    theButtons[3] = new QPushButton("CLEAR",theGrid);
    theButtons[4] = new QPushButton("QUIT",theGrid);

    for (uint i = 0; i < 27; i++)
    {
        theTabs->addTab(tabs[i],QString());
        setuptab(i);  // set up the layouts
    }
    theLayout->setSizeConstraint(QLayout::SetNoConstraint);
    theLayout->setContentsMargins(0,0,0,0);
    theLayout->addWidget(theTabs,0,0,4,8);
    theLayout->addWidget(theButtons[0],6,0);
    theLayout->addWidget(theButtons[1],6,1);
    theLayout->addWidget(theButtons[2],6,2);
    theLayout->addWidget(theButtons[3],6,3);
    theLayout->addWidget(theButtons[4],6,7);
    theLayout->addWidget(findss,7,0,1,3);
    theLayout->addWidget(findSSButton,7,3,1,1);
    theLayout->addWidget(findExact,7,4,1,1,Qt::AlignRight);
    theLayout->addWidget(findExactL,7,5,1,Qt::AlignLeft);
    theGrid->setLayout(theLayout);
    theGrid->showMaximized();


    QSizePolicy qsp(QSizePolicy::Preferred,QSizePolicy::Preferred);
    qsp.setHeightForWidth(true);
    setSizePolicy(qsp);
    setMinimumSize(500,500);
    connect(actionOpen,    SIGNAL(triggered()), this, SLOT(openfile_input()));
    connect(actionSave,    SIGNAL(triggered()), this, SLOT(openfile_output()));
    connect(findnext,      SIGNAL(clicked()),   this, SLOT(doFind()));
    connect(theButtons[0], SIGNAL(clicked()),   this, SLOT(doParse()));
    connect(theButtons[1], SIGNAL(clicked()),   this, SLOT(doProcessHeap()));
    connect(theButtons[2], SIGNAL(clicked()),   this, SLOT(doProcessBST()));
    connect(theButtons[3], SIGNAL(clicked()),   this, SLOT(doClear()));
    connect(theButtons[4], SIGNAL(clicked()),   this, SLOT(close()));
    connect(findSSButton,  SIGNAL(clicked()),   this, SLOT(doFindinSS()));
    parsed = false;
}

void mwindow::paintEvent()
{

}

void mwindow::doFind()
{
    if (findval->text().isEmpty())
        return;
    theSummary->find(findval->text());
}

void mwindow::doFindinSS()
{
    QList< QTableWidgetItem* > founds; // pointers to found items
    if (findss->text().isEmpty())
        return;
    int charval = findss->text()[0].toUpper().toAscii() - 'A';
    (charval < 0) && (charval = 0);
    theTabs->setCurrentIndex(charval+1);
    tabs[charval+1]->show();
    tabs[charval+1]->focusWidget();
    if (findExact->isChecked())
        founds = tables[charval]->findItems(findss->text(), Qt::MatchExactly);
    else
        founds = tables[charval]->findItems(findss->text(), Qt::MatchStartsWith);
    if ( founds[0] )
    {
        founds[0]->setSelected(1);
        tables[charval]->scrollToItem(founds[0]);
    }
}

void mwindow::resizeEvent()
{
    theGrid->setGeometry(10,10,this->width(),this->height());
    update();
}

void mwindow::setuptab(int i)
{
    QString myid = QChar(i+'A'-1);
    switch (i)
    {
    case 0:
        myid = "SUMMARY";
        break;
    case 1:
        myid = "0-9,A,SpecialChars";
    default:
        break;
    }
    theTabs->setTabText(i, myid);
}

/****************************
 *
 * TO BE TRIGGERED BY FILE Open OR FILE Save As.
 *
 ***************************/

void mwindow::openfile_output()
{
    if (outfile.isOpen())
        outfile.close();
    QString ofilename = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("TXT FILE(*.txt)"));

    if (ofilename.isEmpty())
        return;
    outfile.setFileName(ofilename);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    outStream = new QTextStream(&outfile);
    *outStream << theSummary->toPlainText();
    *outStream << "------------------------\n";
    for (uint i = 0; i < 26; i++)
    {
        for (int row = 0; row < tables[i]->rowCount(); row++ )
        {
            for (int col = 0; col < tables[i]->columnCount(); col++)
            {
                if (tables[i]->item(row,col))
                    *outStream << tables[i]->item(row,col)->text() << ",";
            }
            *outStream << "\n";
        }
    }
    outfile.close();
    delete outStream;
}

void mwindow::openfile_input()
{
    if (infile.isOpen())
        infile.close();
    QString onlyAllow = tr("TXT FILE(*.txt)");
    QString ifilename = QFileDialog::getOpenFileName(this, "Select a file to open for processing", ".", tr("TXT FILE(*.txt)"), &onlyAllow);

    if (ifilename.isEmpty())
        return;

    infile.setFileName(ifilename);

    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text))
        throw FILE_NOT_FOUND;
    inStream = new QTextStream(&infile);
}

void mwindow::doClear()
{
    RegexParser.clear();
    statusBar()->showMessage("CLEARING DATA AND PARSING AND SORTING...", 10000);
    statusBar()->showMessage("CLEARING DATA AND PARSING AND SORTING...", 10000);
    theSummary->clear();
    for (int i=0; i < 26; i++)
    {
        tables[i]->clear();
    }
    statusBar()->showMessage("CLEARED.", 10000);
    statusBar()->showMessage("CLEARED.", 10000);
}
void mwindow::doParse()
{
    doClear();
    statusBar()->showMessage("PARSING...",10000);
    statusBar()->showMessage("PARSING...",10000);
    if (infile.isOpen())
    {
        QElapsedTimer QET1;
        QET1.start();
        *inStream >> RegexParser;
        timetoparse = (1.0*QET1.elapsed())/1000.0;
        infile.close();
        delete inStream; // make the stream free
    }
    else {
        statusBar()->showMessage("OPEN FILE FIRST...CANNOT PARSE...",10000);
        return;
    }
    parsed = true;
    statusBar()->showMessage("Done.",10000);
}

void mwindow::doProcess()
{
    // presort
    // semi-binsort the alpha vectors, just by first letter... coming in in order, line by line
    *presort << RegexParser.theWordlist();
}

void mwindow::getTopTen()
{
    for (int i = 0; i < 10; i++)
        bstten[i] = NULL;
    int bsthigh = 10;
    for (uint i = 0; i < avltrees.size(); i++)
    {
        int bstsize = avltrees[i].thebstroot().size();
        for (int j = 0; j < bstsize ; j++)
        {
            if (!avltrees[i].thebstroot()[j])
                continue;

            if (bsthigh == 10)
            {
                bsthigh--;
                bstten[9] = avltrees[i].thebstroot()[j]; // if we don't have a root
            }
            else
            {
                // want to put value in and we can move the list up to put it in
                if (bsthigh > 0)
                {
                    // find the place from where we need to move up because highest val is >0 we can move the array
                    int t = 9;
                    for (; bstten[t] && bstten[t]->theCount() < avltrees[i].thebstroot()[j]->theCount(); t--);
                    movearrayleft(bstten,1,bsthigh,t);
                    bsthigh--;
                    bstten[t] = avltrees[i].thebstroot()[j]; // in the place that was opened
                }
                else
                {
                    //we need to check FROM WHERE we need to move the list down
                    int t = 9;
                    for (; t >=0 && bstten[t] && bstten[t]->theCount() < avltrees[i].thebstroot()[j]->theCount(); t-- );
                    if (t == 9)
                    {
                        if (!bstten[t])
                            bstten[t] = avltrees[i].thebstroot()[j];
                        else
                            if (bstten[t]->theCount() < avltrees[i].thebstroot()[j]->theCount())
                                bstten[t] = avltrees[i].thebstroot()[j];
                    }
                    else if(t+1 <= 9)
                    {
                        // we need to move from t+1 to the end, right 1 space
                        movearrayright(bstten,1,t+1, 10);
                        // highest value doesn't change even if t+1 IS bsthigh, because
                        bstten[t+1] = avltrees[i].thebstroot()[j];
                    }
                }
            }
        }

    }
}

void mwindow::doProcessBST()
{

    if (!parsed)
    {
        statusBar()->showMessage("MUST PARSE A FILE BEFORE SORTING.",20000);
        return;
    }
    statusBar()->showMessage("PROCESSING BST.",10000);
    for (int i = 0 ; i < 10; i++)
        bstten[i] = NULL;
    statusBar()->showMessage("PROCESSING BST..",10000);
    presort = new presorter<QString>;
    *presort << AVL;
    statusBar()->showMessage("PROCESSING BST...",10000);
    doProcess();
    // reset just in case
    *presort >> avltrees;
    getTopTen();
    statusBar()->showMessage("DONE WITH BST... POPULATING TABLE OF INFORMATION...",10000);
    for (uint i = 0; i < avltrees.size(); i++)
    {
        tables[i]->clear();
        tables[i]->setRowCount(1);
        tables[i]->setColumnCount(6);
        tables[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tables[i]->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Word")));
        tables[i]->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Count")));
        tables[i]->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Paragraph")));
        tables[i]->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("Line")));

        int ref_letter = presort->theBST()[i].thebstroot().size();
        tables[i]->setRowCount(ref_letter+1); // add all rows at once!

        // we have to do a traversal...
        linenumber = 0; // set here
        doTraversalBST(i, 1);
    }
    RegexParser.fleschkinkaid();
    QString BSTTIME = "The time it took to parse and syllablize and regexpify: %1 seconds. The time it took to BSTIFY: %2 seconds\n";
    QString WC = "The word count of the document is: %1\n";
    QString SC = "The number of sentences in the document is: %1\n";
    QString SYC = "The number of syllables in the document is: %1\n";
    QString AVGW = "The average length of words is: %1 and average sentence length is: %2\n";
    QString FK = "The Reading level is %1 and the grade level is %2 \n";
    QString TopTen = "The top ten words encountered in the document are: \n";
    for (int i = 0; i < 9; i++)
    {
        if (bstten[i])
        {
            TopTen.append(bstten[i]->theData());
            TopTen.append(" appeared ");
            TopTen.append(QString::number(bstten[i]->theCount()));
            TopTen.append(" times in: ");
            uint z = 0;
            uint linesz = bstten[i]->theLine().size();
            for (uint j = 0; j < linesz; j++)
            {
                TopTen.append(QString("L: %1 ").arg(bstten[i]->theLine()[j]));
                TopTen.append(QString("P: %1 ,").arg(bstten[i]->theParagraph()[j]));
            }
        }
        TopTen.append("\n\n");
    }
    QString NWL = "The Number of words in\n";
    for (int i = 0; i < 26; i++)
    {
        int currentcount = 0;
        for (int j = 0; j < tables[i]->rowCount(); j++)
        {
            if (!tables[i]->item(j,1))
                break;
            currentcount+=tables[i]->item(j,1)->text().toInt();
        }
        NWL.append(" ");
        NWL.append(QChar('A'+i));
        NWL.append(": ");
        NWL.append(QString::number(currentcount));
        if (!((i+1)%6))
            NWL.append("\n");
    }
    NWL.append("\n");
    theSummary->setText(BSTTIME.arg(QString::number(timetoparse), QString::number(presort->time_last_sort)));
    theSummary->append(NWL);
    theSummary->append(WC.arg(QString::number(RegexParser.num_words)));
    theSummary->append(SC.arg(QString::number(RegexParser.num_sentences)));
    theSummary->append(SYC.arg(QString::number(RegexParser.num_syllables)));
    theSummary->append(AVGW.arg(QString::number(RegexParser.avg_word_len), QString::number(RegexParser.avg_sentence_len)));
    theSummary->append(FK.arg(QString::number(RegexParser.FRL), QString::number(RegexParser.FGL)));
    theSummary->append(TopTen);
    parsed = false;
    statusBar()->showMessage("DONE POPULATING TABLES....", 10000);
    theSummary->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
    RegexParser.clear();
    delete presort;
}

void mwindow::doTraversalBST(int tablenum, int idx)
{
    if ((uint)idx >= avltrees[tablenum].thebstroot().size()-1)
        return;
    if (avltrees[tablenum].thebstroot()[2*idx])
        doTraversalBST(tablenum, 2*idx);
    if (avltrees[tablenum].thebstroot()[idx])
    {

        // we're putting in this node to the table
        tables[tablenum]->setItem(linenumber, 0, new QTableWidgetItem(avltrees[tablenum].thebstroot()[idx]->theData()));
        tables[tablenum]->setItem(linenumber, 1, new QTableWidgetItem(QString::number(avltrees[tablenum].thebstroot()[idx]->theCount())));

        uint paragraphsize = avltrees[tablenum].thebstroot()[idx]->theParagraph().size();
        QString theparas;
        for (uint i = 0; i < paragraphsize; i++)
        {
            theparas.append(QString::number(avltrees[tablenum].thebstroot()[idx]->theParagraph()[i]));
            theparas.append(", ");
        }
        if (theparas[theparas.size()-2] == QChar(','))
            theparas.chop(2);
        tables[tablenum]->setItem(linenumber, 2, new QTableWidgetItem(theparas));

        uint linesize = avltrees[tablenum].thebstroot()[idx]->theLine().size();
        QString thelines;
        for (uint i = 0; i < linesize; i++)
        {
            thelines.append(QString::number(avltrees[tablenum].thebstroot()[idx]->theLine()[i]));
            thelines.append(", ");
        }
        if (thelines[thelines.size()-2] == QChar(','))
            thelines.chop(2);
        tables[tablenum]->setItem(linenumber, 3, new QTableWidgetItem(thelines));
        linenumber++;
    }
    if (avltrees[tablenum].thebstroot()[2*idx+1])
        doTraversalBST(tablenum, 2*idx+1);
}

void mwindow::doProcessHeap()
{
    presort = new presorter<QString>;
    map<QString, node<QString>* > mapforcount;
    map<QString, node<QString>* >::iterator mapIt;
    node<QString>* tenmostfreq[10];
    int highestval = 10;
    for (int i = 0 ; i < 10; i++)
    {
        tenmostfreq[i] = NULL;
    }
    if (!parsed)
    {
        statusBar()->showMessage("PARSING NOT DONE, CANNOT HEAP.",3000);
        return;
    }
    statusBar()->showMessage("PROCESSING HEAP..",1000);
    statusBar()->showMessage("PROCESSING HEAP..",1000);
    *presort << HEAP;
    statusBar()->showMessage("PROCESSING HEAP...",3000);
    doProcess();
    // reset just in case
    // Do process on inStream
    *presort >> ref_Heap; // we get the references using reheapify
    statusBar()->showMessage("DONE PROCESSING HEAP, POPULATING TABLES...",3000);
    statusBar()->showMessage("DONE PROCESSING HEAP, POPULATING TABLES...",3000);
    for (uint i = 0; i < ref_Heap.size(); i++)
    {
        tables[i]->clear();
        tables[i]->setRowCount(1);
        tables[i]->setColumnCount(6);
        tables[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tables[i]->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Word")));
        tables[i]->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Paragraph")));
        tables[i]->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Line")));

        int ref_letter = ref_Heap[i].size();
        tables[i]->setRowCount(ref_letter+1); // add all rows at once!
        for (uint j = ref_letter-1; j > 0; j--)
        {
            if (!presort->theAlphas()[i][ref_Heap[i][j]])
                continue;
            QString this_string = presort->theAlphas()[i][ref_Heap[i][j]]->theData();
            tables[i]->setItem(ref_letter-j-1, 0, new QTableWidgetItem(this_string));
            // map here
            mapIt = mapforcount.find(this_string);
            // returns mapforcount.end() if not found!

            if (mapIt == mapforcount.end())
                // Not found so ... add it
                mapforcount[this_string] = new node<QString>(*presort->theAlphas()[i][ref_Heap[i][j]]);
            else
                *mapforcount[this_string] += *presort->theAlphas()[i][ref_Heap[i][j]]; // add all relevant information (overloaded already)
            tables[i]->setItem(ref_letter-j-1, 1, new QTableWidgetItem(QString::number(presort->theAlphas()[i][ref_Heap[i][j]]->theParagraph()[0])));
            tables[i]->setItem(ref_letter-j-1, 2, new QTableWidgetItem(QString::number(presort->theAlphas()[i][ref_Heap[i][j]]->theLine()[0])));
        }
        // now walk through the map with the iterator
    }
    for (mapIt = mapforcount.begin(); mapIt != mapforcount.end(); mapIt++)
    {
        if (highestval == 10)
        {
            highestval--;
            tenmostfreq[9] = mapIt->second; // if we don't have a root
        }
        else
        {
            // want to put value in and we can move the list up to put it in
            if (highestval >= 0)
            {
                // find the place from where we need to move up because highest val is >0 we can move the array
                int t = 9;

                for (; t >= highestval && tenmostfreq[t] && tenmostfreq[t]->theCount() < mapIt->second->theCount(); t--);
                if (highestval <= t && tenmostfreq[t] && tenmostfreq[t]->theCount() > mapIt->second->theCount())
                {
                    movearrayleft(tenmostfreq,1,highestval,t);
                    highestval--;
                    tenmostfreq[t] = mapIt->second; // in the place that was opened
                }
            }
            // want to put it in and we can't move list up
            else
            {
                //we need to check FROM WHERE we need to move the list down
                int t = 9;
                for (; t >=0 && tenmostfreq[t]->theCount() < mapIt->second->theCount(); t-- );
                // we need to move from t+1 to the end, right 1 space
                movearrayright(tenmostfreq,1,t+1, 9);
                // highest value doesn't change even if t+1 IS highestval, because
                tenmostfreq[t+1] = mapIt->second;
            }
        }
    }
    parsed = false;
    statusBar()->showMessage("DONE POPULATING TABLES....", 10000);
    statusBar()->showMessage("DONE POPULATING TABLES....", 10000);
    RegexParser.fleschkinkaid();
    QString HEAPTIME= "The time it took to parse and syllablize and regexpify: %1 seconds. The time it took to HEAP: %2 seconds\n";
    QString WC = "The word count of the document is: %1 .\n There are this many words beginning with\n A: %2 B: %3 C: %4 D: %5 E: %6 F: %7 G: %8\n H: %9 I: %10 J: %11 K: %12 L: %13 M: %14 N: %15\n O: %16 P: %17 Q: %18 R: %19 S: %20 T: %21 U: %22\n V: %23 W: %24 X: %25 Y: %26 Z: %27\n";
    QString SC = "The number of sentences in the document is: %1\n";
    QString SYC = "The number of syllables in the document is: %1\n";
    QString AVGW = "The average length of words is: %1 and average sentence length is: %2\n";
    QString FK = "The Reading level is %1 and the grade level is %2 \n";
    QString TopTen = "The top ten words encountered in the document are: \n";
    for (int i = 0; i < 9; i++)
    {
        if (tenmostfreq[i])
        {
            TopTen.append(tenmostfreq[i]->theData());
            TopTen.append(" appeared ");
            TopTen.append(QString::number(tenmostfreq[i]->theCount()));
            TopTen.append(" times in: ");
            uint z = 0;
            uint linesz = tenmostfreq[i]->theLine().size();
            for (uint j = 0; j < linesz; j++)
            {
                TopTen.append(QString("L: %1 ").arg(tenmostfreq[i]->theLine()[j]));
                TopTen.append(QString("P: %1 ,").arg(tenmostfreq[i]->theParagraph()[j]));
            }
            TopTen.append("\n\n");
        }
    }
    WC = WC.arg(QString::number(RegexParser.num_words)).
            arg(QString::number(presort->theAlphas()[0].size())).
            arg(QString::number(presort->theAlphas()[1].size())).
            arg(QString::number(presort->theAlphas()[2].size())).
            arg(QString::number(presort->theAlphas()[3].size())).
            arg(QString::number(presort->theAlphas()[4].size())).
            arg(QString::number(presort->theAlphas()[5].size())).
            arg(QString::number(presort->theAlphas()[6].size())).
            arg(QString::number(presort->theAlphas()[7].size())).
            arg(QString::number(presort->theAlphas()[8].size())).
            arg(QString::number(presort->theAlphas()[9].size())).
            arg(QString::number(presort->theAlphas()[10].size())).
            arg(QString::number(presort->theAlphas()[11].size())).
            arg(QString::number(presort->theAlphas()[12].size())).
            arg(QString::number(presort->theAlphas()[13].size())).
            arg(QString::number(presort->theAlphas()[14].size())).
            arg(QString::number(presort->theAlphas()[15].size())).
            arg(QString::number(presort->theAlphas()[16].size())).
            arg(QString::number(presort->theAlphas()[17].size())).
            arg(QString::number(presort->theAlphas()[18].size())).
            arg(QString::number(presort->theAlphas()[19].size())).
            arg(QString::number(presort->theAlphas()[20].size())).
            arg(QString::number(presort->theAlphas()[21].size())).
            arg(QString::number(presort->theAlphas()[22].size())).
            arg(QString::number(presort->theAlphas()[23].size())).
            arg(QString::number(presort->theAlphas()[24].size())).
            arg(QString::number(presort->theAlphas()[25].size()));

    theSummary->setText(HEAPTIME.arg(QString::number(timetoparse), QString::number(presort->time_last_sort)));
    theSummary->append(WC);
    theSummary->append(SC.arg(QString::number(RegexParser.num_sentences)));
    theSummary->append(SYC.arg(QString::number(RegexParser.num_syllables)));
    theSummary->append(AVGW.arg(QString::number(RegexParser.avg_word_len), QString::number(RegexParser.avg_sentence_len)));
    theSummary->append(FK.arg(QString::number(RegexParser.FRL), QString::number(RegexParser.FGL)));
    theSummary->append(TopTen);
    theSummary->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
    RegexParser.clear();
    delete presort;
}

bool mwindow::movearrayleft(node<QString>* arr[], int howmany, int fromleft, int toright)
{
    int temp = fromleft;
    // start at toleft and move it down until where we are moving is the last one... also don't move too far left
    for (; temp-howmany >= 0 &&  temp <= toright; temp++)
    {
        arr[temp-howmany] = arr[temp];
        arr[temp] = NULL;
    }
    return (temp-howmany < 0) ? 0 : 1;
}
bool mwindow::movearrayright(node<QString>* arr[], int howmany, int fromleft, int tomax)
{
    if (fromleft > tomax)
        return 0;
    for (; tomax >= fromleft; tomax--)
    {
        if (tomax+howmany <= 9)
            arr[tomax+howmany] = arr[tomax];
        arr[tomax] = NULL;
    }
    return 1;
}
