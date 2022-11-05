#include "treemodel.h"
#include "databasecontroller.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

TreeModel::TreeModel(DataBaseController *controller, QObject *parent)
    : QAbstractItemModel(parent)
    , dbController(controller)
    , rootItem(new TreeItem())
{
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

void TreeModel::reset()
{
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = getItem(index);
    return item->data(role);
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

void TreeModel::adjustHierarchy()
{
    for (TreeItem *item : itemRegistry) {
        const QString &parentKey = item->data(TreeItem::ParentKeyRole).toString();
        TreeItem *realParentItem = itemRegistry.value(parentKey);
        if (realParentItem && realParentItem != item->parent()) {
            item->parent()->setChildFree(item);
            item->setParent(realParentItem);
            realParentItem->addChild(item);
        }

        // If parent is deleted, set child deleted
        if (realParentItem && realParentItem->data(TreeItem::DeletedRole).toBool()) {
            item->setData(TreeItem::DeletedRole, true);
            // Hide "possible deleted" label
            item->setPossiblyDeleted(false);

        // If item is possibly deleted and parent is not deleted, set child not possibly deleted
        } else if (realParentItem
                   && item->data(TreeItem::PossiblyDeletedRole).toBool()
                   && !realParentItem->data(TreeItem::DeletedRole).toBool()) {
            item->setPossiblyDeleted(false);
        }
    }
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginInsertRows(parent, position, position + rows - 1);
    const bool success = parentItem->insertChildren(position, rows);
    endInsertRows();

    return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginRemoveRows(parent, position, position + rows - 1);
    const bool success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

QHash<int, QByteArray> TreeModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { TreeItem::ValueRole, QByteArrayLiteral("value") },
        { TreeItem::KeyRole, QByteArrayLiteral("key") },
        { TreeItem::ParentKeyRole, QByteArrayLiteral("parentKey") },
        { TreeItem::AddedRole, QByteArrayLiteral("added") },
        { TreeItem::DeletedRole, QByteArrayLiteral("deleted") },
        { TreeItem::ModifiedRole, QByteArrayLiteral("modified") }
    };
    return roles;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    const TreeItem *parentItem = getItem(parent);
    return parentItem ? parentItem->childCount() : 0;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    TreeItem *item = getItem(index);
    bool result = item->setData(role, value);

    if (result)
        emit dataChanged(index, index);

    return result;
}
