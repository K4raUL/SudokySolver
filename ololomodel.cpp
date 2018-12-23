#include "ololomodel.h"
#include <qdebug.h>
#include <qfont.h>
#include <qbrush.h>

OloloModel::OloloModel()
{
    datat = QVector<int>(9*9);
    added = QVector<QVariant>(9*9);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            datat[i*9+j] = 0;
            added[i*9+j] = QVariant(QBrush(Qt::white));
        }
    }
}

QVariant OloloModel::data(const QModelIndex &index, int role) const
{
    //static int i = 0;
    //qDebug() << i << "data";
    //i++;
    if (!index.isValid())
        return QVariant();
    if (role == Qt::BackgroundColorRole) {
        return added[index.row()*9+index.column()];
    }

    if (role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignCenter);
    }

    if (role == Qt::ForegroundRole && datat[index.row()*9 + index.column()]) {
        return QVariant(QBrush(Qt::blue));
    }

    if (role == Qt::FontRole) {
        return QVariant(QFont("Helvetica", 25));
    }

    if (role != Qt::DisplayRole)
        return QVariant();
    return datat[index.row()*9+index.column()];
}

int OloloModel::rowCount(const QModelIndex &parent) const
{
    return 9;
}

int OloloModel::columnCount(const QModelIndex &parent) const
{
    return 9;
}

bool OloloModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::BackgroundColorRole) {
        added[index.row()*9+index.column()] = value;
        return true;
    }
    //if (role != Qt::DisplayRole)
      //  return false;

    int r = index.row(),
        c = index.column();

    //if (column < 0 || column >= itemData.size())
    //    return false;
    datat[r*9+c] = value.toInt();

    return true;
}

int OloloModel::getCell(int r, int c)
{
    return datat[r*9+c];

}

Qt::ItemFlags OloloModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
