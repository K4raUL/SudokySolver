#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtSql>
#include <QTableView>
#include <QVector>
#include "ololomodel.h"
#include <fstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void checkColl (int x, int y, QVector<int>* variants);
    bool fillField ();
    void vectCopy ();
    void vectBackCopy();
    int count();
    void data_rewrite();
    void vector_field_copy();

private:
    Ui::MainWindow *ui;
    //QSqlTableModel model;
    OloloModel ololo_model;
    QVector<int> *field;        // field data
    QVector<int> *startfield;        // field data
    QVector<int> *mask;         // squares 3x3
    QVector<int> **posDig;       // list of possible digits for each cell
    QVector<int> *fieldcopy;
    QVector<int> **posDigcopy;

private slots:
    void changeCell(const QModelIndex & index);
    void init();
    void clear_to_init();
    void load_from_txt();
    void clear_all();
};

#endif // MAINWINDOW_H
