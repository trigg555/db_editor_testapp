#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "treeitem.h"

class DataBaseController;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    TreeModel(QObject *parent = nullptr);
    explicit TreeModel(DataBaseController *controller, QObject *parent = nullptr);
    virtual ~TreeModel();

    Q_INVOKABLE virtual void reset();

    QVariant data(const QModelIndex &index, int role = TreeItem::KeyRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;

    QHash<int, QByteArray> roleNames() const override;

protected:
    TreeItem *getItem(const QModelIndex &index) const;
    void adjustHierarchy();

    DataBaseController *dbController;
    TreeItem *rootItem;
    QHash<QString, TreeItem*> itemRegistry;
};

#endif // TREEMODEL_H
