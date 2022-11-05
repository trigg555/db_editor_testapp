#include "cachedtreemodel.h"
#include "databasecontroller.h"

CachedTreeModel::CachedTreeModel(QObject *parent)
    : TreeModel(parent)
{
}

CachedTreeModel::CachedTreeModel(DataBaseController *controller, QObject *parent)
    : TreeModel(controller, parent)
{
    connect(dbController, &DataBaseController::databaseUpdated, this, &CachedTreeModel::doClarifyingRequests);
}

CachedTreeModel::~CachedTreeModel()
{
}

void CachedTreeModel::addNode(const QModelIndex &parent)
{
    TreeItem* parentItem = getItem(parent);
    if (!parentItem
            || parentItem->data(TreeItem::ParentKeyRole).toString().isEmpty()
            || parentItem->data(TreeItem::DeletedRole).toBool()
            || parentItem->data(TreeItem::PossiblyDeletedRole).toBool())
        return;

    beginResetModel();

    parentItem->insertChildren(parentItem->childCount(), 1);
    const QString &key = dbController->generateKey();
    const QString &parentKey = parentItem->data(TreeItem::KeyRole).toString();
    NodeData nodeData(key, QString("[new Node]"), parentKey, false, true);
    TreeItem *newItem = parentItem->child(parentItem->childCount() - 1);
    newItem->setData(nodeData);

    // Add to Registry of items
    itemRegistry.insert(key, newItem);

    endResetModel();
}

void CachedTreeModel::deleteNode(const QModelIndex &index)
{
    TreeItem* item = getItem(index);
    if (!item || item->data(TreeItem::DeletedRole).toBool()) {
        return;
    }

    beginResetModel();
    if (item->setData(TreeItem::DeletedRole, true)) {
        item->setData(TreeItem::ModifiedRole, true);
    }
    endResetModel();
}

void CachedTreeModel::editNode(const QModelIndex &index)
{
    TreeItem *item = getItem(index);
    if (!item
            || item->data(TreeItem::DeletedRole).toBool()
            || item->data(TreeItem::PossiblyDeletedRole).toBool()) {
        return;
    }
    item->setEditable(true);
    emit dataChanged(index, index);
}

void CachedTreeModel::reset()
{
    beginResetModel();
    itemRegistry.clear();
    rootItem->removeChildren(0, rootItem->childCount());
    endResetModel();
}

bool CachedTreeModel::fetchNode(const QString &key)
{
    if (!dbController->isItemExists(key))
        return false;

    const NodeData &data = dbController->getItem(key);

    beginResetModel();

    // Check if already added
    if (itemRegistry.contains(key)) {
        TreeItem* existingItem = itemRegistry.value(key);
        existingItem->setData(data);
        existingItem->setPossiblyDeleted(false);
    } else {
        // Find parent Item and add into it
        // Or place item on top level
        TreeItem* parentItem = rootItem->findParent(data.parentKey());
        if (!parentItem)
            parentItem = rootItem;

        parentItem->insertChildren(parentItem->childCount(), 1);
        TreeItem *newItem = parentItem->child(parentItem->childCount() - 1);
        newItem->setData(data);

        // Add to Registry of items
        itemRegistry.insert(key, newItem);
    }

    // Fix hierarchy
    adjustHierarchy();

    endResetModel();
    return true;
}

void CachedTreeModel::applyChanges()
{
    QHash<QString, NodeData> changes;
    beginResetModel();

    // Сheck if at least one item has been deleted
    bool deletedItemIsPresent = false;
    for (TreeItem *item : itemRegistry) {
        if (item && item->data(TreeItem::DeletedRole).toBool()
                && item->data(TreeItem::ModifiedRole).toBool()) {
            deletedItemIsPresent = true;
            break;
        }
    }

    for (TreeItem *item : itemRegistry) {
        if (item->data(TreeItem::DeletedRole).toBool())
            item->setPossiblyDeleted(false);

        // Find the non-deleted item and mark it as possibly deleted
        if (deletedItemIsPresent
                && !item->data(TreeItem::DeletedRole).toBool()
                // Check if it's not top item, according to the task, only one is possible
                && item->data(TreeItem::ParentKeyRole).toString() != dbController->getRootKey()) {
            item->setPossiblyDeleted(true);
        }

        if (item->data(TreeItem::AddedRole).toBool()
                || item->data(TreeItem::ModifiedRole).toBool()) {
            item->setData(TreeItem::AddedRole, false);
            item->setData(TreeItem::ModifiedRole, false);
            changes.insert(item->data().key(), item->data());
        }
    }
    endResetModel();
    dbController->setChanges(changes);
}

void CachedTreeModel::doClarifyingRequests()
{
    std::function<void(TreeItem*)> fetch;
    fetch = [this, &fetch](TreeItem *item) {
        if (item && item->data(TreeItem::PossiblyDeletedRole).toBool()) {
            fetchNode(item->data(TreeItem::KeyRole).toString());
        }

        for (int i = 0; i < item->childCount(); ++i) {
            if (item->child(i)->data(TreeItem::PossiblyDeletedRole).toBool()) {
                fetch(item->child(i));
            }
        }
    };

    // Fetch items from root.
    // After each fetch, items are checked for the possibility of deletion.
    fetch(rootItem);
}
