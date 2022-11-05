#include "dbtreemodel.h"
#include "databasecontroller.h"

DbTreeModel::DbTreeModel(QObject *parent)
    : TreeModel(parent)
{
}

DbTreeModel::DbTreeModel(DataBaseController *controller, QObject *parent)
    : TreeModel(controller, parent)
{
    dbController->prepareModelData(rootItem, itemRegistry);
    connect(dbController, &DataBaseController::databaseUpdated, this, &DbTreeModel::fetchAll);
}

DbTreeModel::~DbTreeModel()
{
}

void DbTreeModel::reset()
{
    beginResetModel();
    itemRegistry.clear();
    rootItem->removeChildren(0, rootItem->childCount());
    dbController->prepareModelData(rootItem, itemRegistry);
    endResetModel();
}

void DbTreeModel::fetchAll()
{
    beginResetModel();

    for (const NodeData &data : *(dbController->getAllItems())) {
        // Find parent Item and add into it
        // Or place item on top level
        TreeItem* parentItem = rootItem->findParent(data.parentKey());
        if (!parentItem)
            parentItem = rootItem;

        // Check if already added
        if (itemRegistry.contains(data.key())) {
            TreeItem *item = itemRegistry.value(data.key());
            if (item)
                item->setData(data);
            continue;
        }

        parentItem->insertChildren(parentItem->childCount(), 1);
        TreeItem *newItem = parentItem->child(parentItem->childCount() - 1);
        newItem->setData(data);

        // Add to Registry of items
        itemRegistry.insert(data.key(), newItem);
    }

    // Fix hierarchy
    adjustHierarchy();

    endResetModel();
}
