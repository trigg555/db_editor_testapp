#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QVector>
#include "nodedata.h"

class TreeItem : public QObject
{
    Q_OBJECT
public:
    enum Roles  {
        ValueRole = Qt::UserRole + 1,
        KeyRole,
        ParentKeyRole,
        AddedRole,
        DeletedRole,
        ModifiedRole,
        EditableRole,
        PossiblyDeletedRole,
    };
    Q_ENUM(Roles)

    explicit TreeItem(QObject *parent = nullptr);
    explicit TreeItem(TreeItem *parent);
    explicit TreeItem(TreeItem *parent, const NodeData &data);
    ~TreeItem();

    TreeItem *child(int number);
    int childCount() const;
    int childNumber() const;
    int columnCount() const;
    QVariant data(int role) const;
    NodeData data() const;
    bool insertChildren(int position, int count);
    TreeItem *findParent(const QString &key);
    TreeItem *parent();
    void setParent(TreeItem *item);
    void addChild(TreeItem *item);
    void setChildFree(TreeItem *item);
    bool removeChildren(int position, int count);
    bool setData(int role, const QVariant &value);
    void setData(const NodeData &data);
    void setEditable(bool val);
    void setPossiblyDeleted(bool newPossiblyDeleted);

private:
    NodeData m_nodeData;
    QVector<TreeItem*> m_childItems;
    TreeItem *m_parentItem;
    bool m_editable {false};
    bool m_possiblyDeleted {false};
};

#endif // TREEITEM_H
