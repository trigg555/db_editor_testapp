#include "treeitem.h"
#include "nodedata.h"

TreeItem::TreeItem(QObject *parent)
    : QObject(parent)
    , m_parentItem(nullptr)
{
}

TreeItem::TreeItem(TreeItem *parent)
    : QObject()
    , m_parentItem(parent)
{
}

TreeItem::TreeItem(TreeItem *parent, const NodeData &data)
    : QObject()
    , m_nodeData(data)
    , m_parentItem(parent)
{
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

TreeItem *TreeItem::child(int number)
{
    if (number < 0 || number >= m_childItems.size())
        return nullptr;
    return m_childItems.at(number);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::childNumber() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

int TreeItem::columnCount() const
{
    return 1;
}

QVariant TreeItem::data(int role) const
{
    switch (role) {
    case ValueRole: return m_nodeData.value();
    case KeyRole: return m_nodeData.key();
    case ParentKeyRole: return m_nodeData.parentKey();
    case AddedRole: return m_nodeData.added();
    case DeletedRole: return m_nodeData.deleted();
    case ModifiedRole: return m_nodeData.modified();
    case EditableRole: return m_editable;
    case PossiblyDeletedRole: return m_possiblyDeleted;
    default: return QVariant();
    }
}

NodeData TreeItem::data() const
{
    return m_nodeData;
}

bool TreeItem::insertChildren(int position, int count)
{
    if (position < 0 || position > m_childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        TreeItem *item = new TreeItem(this);
        m_childItems.insert(position, item);
    }

    return true;
}

TreeItem *TreeItem::findParent(const QString &key)
{
    if (data(KeyRole).toString() == key)
        return this;

    for (TreeItem* item : m_childItems) {
        TreeItem* wantedParent = item->findParent(key);
        if (wantedParent)
            return wantedParent;
    }
    return nullptr;
}

TreeItem *TreeItem::parent()
{
    return m_parentItem;
}

void TreeItem::setParent(TreeItem *item)
{
    m_parentItem = item;
}

void TreeItem::addChild(TreeItem *item)
{
    m_childItems.append(item);
}

void TreeItem::setChildFree(TreeItem *item)
{
    m_childItems.removeAll(item);
}

bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > m_childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete m_childItems.takeAt(position);

    return true;
}

bool TreeItem::setData(int role, const QVariant &value)
{
    switch (role) {
    case ValueRole:
        if (m_editable && m_nodeData.value() != value.toString()) {
            m_nodeData.setModified(true);
        }
        m_editable = false;
        m_nodeData.setValue(value.toString());
        break;
    case KeyRole:
        m_nodeData.setKey(value.toString());
        break;
    case ParentKeyRole:
        m_nodeData.setParentKey(value.toString());
        break;
    case AddedRole:
        m_nodeData.setAdded(value.toBool());
        break;
    case DeletedRole:
        m_nodeData.setDeleted(value.toBool());
        if (value.toBool()) {
            QVectorIterator<TreeItem*> it(m_childItems);
            while (it.hasNext()) {
                TreeItem *item = it.next();
                item->setData(DeletedRole, true);
                item->setData(ModifiedRole, true);
            }
        }
        break;
    case ModifiedRole:
        m_nodeData.setModified(value.toBool());
        break;
    case EditableRole:
        m_editable = value.toBool();
        break;
    default:
        return false;
    }

    return true;
}

void TreeItem::setData(const NodeData &data)
{
    m_nodeData = data;
}

void TreeItem::setEditable(bool val)
{
    if (m_editable == val)
        return;
    m_editable = val;
}

void TreeItem::setPossiblyDeleted(bool val)
{
    if (m_possiblyDeleted == val)
        return;
    m_possiblyDeleted = val;
}
