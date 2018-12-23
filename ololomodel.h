#ifndef OLOLOMODEL_H
#define OLOLOMODEL_H

#include <QAbstractTableModel>


class OloloModel : public QAbstractTableModel
{
public:
    OloloModel();

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int getCell(int r, int c);
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QVector<int> datat;
    QVector<QVariant> added;

};

#endif // OLOLOMODEL_H
