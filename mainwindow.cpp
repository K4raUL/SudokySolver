#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect (ui->TableNum, SIGNAL(clicked(const QModelIndex &)), this, SLOT(changeCell(const QModelIndex & )));
    connect (ui->pushButton1, SIGNAL(clicked(bool)), this, SLOT(init()));
    connect (ui->initbutton, SIGNAL(clicked(bool)), this, SLOT(clear_to_init()));
    connect (ui->ClearButton, SIGNAL(clicked(bool)), this, SLOT(clear_all()));
    connect (ui->loadtxt, SIGNAL(clicked(bool)), this, SLOT(load_from_txt()));
    setWindowState(Qt::WindowMaximized);

    ui->TableNum->setModel(&ololo_model);
    ui->TableNum->setEditTriggers(QAbstractItemView::AllEditTriggers);
    QRect rec = QApplication::desktop()->screenGeometry();
    int ht = rec.height();
    ht *= 0.8;
    ui->TableNum->setMinimumHeight(ht+4);
    ui->TableNum->setMinimumWidth(ht+4);
    ui->TableNum->setMaximumHeight(ht+4);
    ui->TableNum->setMaximumWidth(ht+4);
    for (int i = 0; i < 9; i++) {
        ui->TableNum->setColumnWidth(i, ht/9.);
        ui->TableNum->setRowHeight(i, ui->TableNum->columnWidth(i));
    }

    //ui->TableNum->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    ui->TableNum->adjustSize();
    ui->TableNum->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->TableNum->horizontalHeader()->hide();
    ui->TableNum->verticalHeader()->hide();
    ui->TableNum->show();

    ui->TableNum->setFrameStyle(QFrame::Panel | QFrame::Plain);
    ui->TableNum->setLineWidth(2);
    //ui->TableNum->setMidLineWidth(2);
    //QLabel *g = new QLabel;
    //g->setFrameStyle(QFrame::Panel | QFrame::Plain);
    //ui->verticalLayout->addWidget(g);
}

void MainWindow::vectCopy()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            fieldcopy[i][j] = field[i][j];
            posDigcopy[i][j].clear();
            for (int k = 0; k < posDig[i][j].size(); k++)
                posDigcopy[i][j].push_back(posDig[i][j][k]);
        }
    }
}

void MainWindow::vectBackCopy()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            field[i][j] = fieldcopy[i][j];
            posDig[i][j].clear();
            for (int k = 0; k < posDigcopy[i][j].size(); k++)
                posDig[i][j].push_back(posDigcopy[i][j][k]);
        }
    }
}

//----------------initialize vectors----------------//
void MainWindow::init()
{
    fieldcopy = new QVector<int>[9];
    posDigcopy = new QVector<int>*[9];

    field = new QVector<int>[9];
    startfield = new QVector<int>[9];
    mask = new QVector<int>[9];
    posDig = new QVector<int>*[9];
    for (int i = 0; i < 9; i++) {
        posDig[i] = new QVector<int>[9];
        posDigcopy[i] = new QVector<int>[9];
        for (int j = 0; j < 9; j++) {
            field[i].push_back(ololo_model.getCell(i, j));
            fieldcopy[i].push_back(field[i][j]);
            startfield[i].push_back(field[i][j]);
            mask[i].push_back(3*(i/3) + (j/3));
            for (int k = 0; k < 9; k++) {
                if (!field[i][j]) posDig[i][j].push_back(k+1);
                else {
                    posDig[i][j].push_back(field[i][j]);
                    break;
                }
                posDigcopy[i][j].push_back(posDig[i][j][k]);
            }
        }
    }

    vectCopy();
    count();
    vectBackCopy();
    data_rewrite();
}

int MainWindow::count()
{
    int a = -1, b = 1;
    // если нельзя однозначно разрешить с 1 раза - начать перебор
    if (!fillField()) {
        //save
        QVector<int> *fcpy = new QVector<int>[9];
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
               fcpy[i].push_back(fieldcopy[i][j]);
            }
        }
        //empty cells
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (!fieldcopy[i][j] && (a == -1)) {
                    a = i;
                    b = j;
                }
                if (!fieldcopy[i][j] && (posDigcopy[i][j].size() == 0)) {
                    vectBackCopy();
                    data_rewrite();
                    return 0;
                }
            }
        }

        for (int k = 0; k < posDigcopy[a][b].size(); k++) {
            fieldcopy[a][b] = posDigcopy[a][b][k];

            //restore
            if (!count()) {
                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) fieldcopy[i][j] = fcpy[i][j];
                }
            }
            else return 1;
        }
        return 0;
    }
    return 1;   // field is full filled
}

// writing results into data
void MainWindow::data_rewrite()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if ( (field[i][j] != ololo_model.getCell(i, j)) && ololo_model.setData(ololo_model.index(i, j), QVariant(field[i][j]), Qt::DisplayRole))
                ololo_model.setData(ololo_model.index(i, j), QVariant(QBrush(Qt::green)), Qt::BackgroundColorRole);

            if (field[i][j] == 0)
                ololo_model.setData(ololo_model.index(i, j), QVariant(QBrush("#fbbbbb")), Qt::BackgroundColorRole);

            ui->TableNum->reset();
        }
    }
}

bool MainWindow::fillField()
{
    int done = 0, On_Changed;
    int counter[9] = {0};
    int lastpos[9] = {0};

    while (1) {
        On_Changed = 0;
        done = 1;

        // filling PosDig vectors
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                // if cell is empty
                if (!fieldcopy[i][j]) {
                    done = 0;
                    checkColl(i, j, &posDigcopy[i][j]);
                }
                // only one digit is possible in [i][j] cell
                if (posDigcopy[i][j].size() == 1) {
                    fieldcopy[i][j] = posDigcopy[i][j].first();
                    posDigcopy[i][j].clear();
                    On_Changed = 1;
                }
            }
        }
        if (On_Changed) continue;

        //-----check all strings-----
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                for (int k = 0; k < posDigcopy[i][j].size(); k++) {
                    if (!fieldcopy[i][j]) {
                        counter[posDigcopy[i][j][k]-1]++;
                        lastpos[posDigcopy[i][j][k]-1] = 10*i+j;
                    }
                }
            }
            // if all possible string[i] digits contains some digit only 1 time - place it
            for (int d = 0; d < 9; d++) {
                if (counter[d] == 1) {
                    int b = lastpos[d]%10;
                    int a = lastpos[d]/10;
                    fieldcopy[a][b] = d+1;
                    posDigcopy[a][b].clear();
                    On_Changed = 2;
                }
                counter[d] = 0;
                lastpos[d] = 0;
            }
        }
        //-----check all strings-----
        if (On_Changed) continue;
        //-----check all columns-----
        for (int m = 0; m < 9; m++) {
            for (int j = 0; j < 9; j++) {
                for (int k = 0; k < posDigcopy[j][m].size(); k++) {
                    if (!fieldcopy[j][m]) {
                        counter[posDigcopy[j][m][k]-1]++;
                        lastpos[posDigcopy[j][m][k]-1] = 10*j+m;
                    }
                }
            }
            for (int d = 0; d < 9; d++) {
                if (counter[d] == 1) {
                    int b = lastpos[d]%10;
                    int a = lastpos[d]/10;
                    fieldcopy[a][b] = d+1;
                    posDigcopy[a][b].clear();
                    On_Changed = 3;
                }
                counter[d] = 0;
                lastpos[d] = 0;
            }
        }
        //-----check all columns-----
        if (On_Changed) continue;
        //-----check all squares 3x3-----
        for (int s = 0; s < 9; s++) {
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (mask[i][j] == s) {
                        for (int k = 0; k < posDigcopy[i][j].size(); k++) {
                            if (!fieldcopy[i][j]) {
                                counter[posDigcopy[i][j][k]-1]++;
                                lastpos[posDigcopy[i][j][k]-1] = 10*i+j;
                            }
                        }

                    }
                }
            }
            for (int d = 0; d < 9; d++) {
                if (counter[d] == 1) {
                    int b = lastpos[d]%10;
                    int a = lastpos[d]/10;
                    fieldcopy[a][b] = d+1;
                    posDigcopy[a][b].clear();
                    On_Changed = 4;

                }
                counter[d] = 0;
                lastpos[d] = 0;
            }
        }
        //-----check all squares 3v3-----
        if (On_Changed) continue;

        // if no changes in last iteration - exit func
        return (done ? true : false);
    }
}

// filling vectors of possible digits for each cell
void MainWindow::checkColl(int x, int y, QVector<int>* variants)
{
    variants->clear();
    for (int i = 0; i < 9; i++) variants->push_back(i+1);

    for (int v = 0; v < variants->size(); v++) {
        for (int i = 0; i < 9; i++) {
            // collision in a row
            if (variants->at(v) == fieldcopy[x][i]) {
                variants->erase(variants->begin() + v);
                v--;
                break;
            }
            // collision in a coll
            if (variants->at(v) == fieldcopy[i][y]) {
                variants->erase(variants->begin() + v);
                v--;
                break;
            }
        }
    }

    for (int v = 0; v < variants->size(); v++) {
        int ctrl = 0;
        // collision in 3x3
        for (int j = 0; j < 9; j++) {
            for (int k = 0; k < 9; k++) {
                if ((mask[x][y] == mask[j][k]) && (variants->at(v) == fieldcopy[j][k])) {
                    variants->erase(variants->begin() + v);
                    v--;
                    ctrl = 1;
                    break;
                }
            }
            if (ctrl) break;
        }
    }
}

void MainWindow::clear_to_init()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            ololo_model.setData(ololo_model.index(i, j), QVariant(startfield[i][j]), Qt::DisplayRole);
            ololo_model.setData(ololo_model.index(i, j), QVariant(QBrush(Qt::white)), Qt::BackgroundColorRole);
        }
    }
    ui->TableNum->reset();
}

void MainWindow::clear_all()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            ololo_model.setData(ololo_model.index(i, j), 0, Qt::DisplayRole);
            ololo_model.setData(ololo_model.index(i, j), QVariant(QBrush(Qt::white)), Qt::BackgroundColorRole);
        }
    }
    ui->TableNum->reset();
}

void MainWindow::load_from_txt()
{
    int tmp=0;
    std::ifstream in;

    in.open("ex.txt");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            in >> tmp;
            ololo_model.setData(ololo_model.index(i, j), (tmp), Qt::DisplayRole);
        }
    }
    in.close();
    ui->TableNum->reset();
}

void MainWindow::changeCell(const QModelIndex & index)
{
    //qDebug() << index;

}

MainWindow::~MainWindow()
{
    delete ui;
}
