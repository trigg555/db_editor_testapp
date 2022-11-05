#ifndef CACHEDTREEMODEL_H
#define CACHEDTREEMODEL_H

#include "treemodel.h"

class CachedTreeModel : public TreeModel
{
    Q_OBJECT
public:
    CachedTreeModel(QObject *parent = nullptr);
    explicit CachedTreeModel(DataBaseController *controller, QObject *parent = nullptr);
    ~CachedTreeModel();

    Q_INVOKABLE void addNode(const QModelIndex &parent);
    Q_INVOKABLE void deleteNode(const QModelIndex &index);
    Q_INVOKABLE void editNode(const QModelIndex &index);
    /*Q_INVOKABLE*/ void reset() override;
    Q_INVOKABLE bool fetchNode(const QString &key); // triggered by "<<<" button
    Q_INVOKABLE void applyChanges();

private:
    void doClarifyingRequests();

};

#endif // CACHEDTREEMODEL_H
