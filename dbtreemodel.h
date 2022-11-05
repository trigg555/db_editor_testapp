#ifndef DBTREEMODEL_H
#define DBTREEMODEL_H

#include "treemodel.h"

class DbTreeModel : public TreeModel
{
    Q_OBJECT
public:
    DbTreeModel(QObject *parent = nullptr);
    explicit DbTreeModel(DataBaseController *controller, QObject *parent = nullptr);
    ~DbTreeModel();

    /*Q_INVOKABLE*/ void reset() override;

public slots:
    void fetchAll();

private:

};

#endif // DBTREEMODEL_H
