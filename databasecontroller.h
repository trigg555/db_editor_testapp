#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H

#include <QFile>
#include <QHash>
#include "nodedata.h"

class TreeItem;

class DataBaseController : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseController(QObject *parent = nullptr);

    QString getDataAsString();
    void prepareModelData(TreeItem *rootItem, QHash<QString, TreeItem*> &registry);

    bool isItemExists(const QString &key) const;
    NodeData getItem(const QString &key) const;
    QHash<QString, NodeData>* getAllItems();

    void setChanges(const QHash<QString, NodeData> &changes);;

    QString generateKey() const;
    QString getRootKey() const;

signals:
    void databaseUpdated();

private:
    void setChildrenDeleted(const NodeData & parent);
    QHash<QString, NodeData> m_database;
};

#endif // DATABASECONTROLLER_H
