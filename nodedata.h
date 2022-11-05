#ifndef NODEDATA_H
#define NODEDATA_H

#include <QString>

class NodeData
{
public:
    NodeData();
    NodeData(const QString &key,
             const QString &value,
             const QString &parentKey,
             bool deleted = false,
             bool added = false);
    ~NodeData();

    QString key() const;
    QString parentKey() const;
    QString value() const;
    bool deleted() const;
    bool added() const;
    bool modified() const;

    void setKey(const QString &val);
    void setParentKey(const QString &val);
    void setValue(const QString &val);
    void setDeleted(const bool &val);
    void setAdded(const bool &val);
    void setModified(const bool &val);

private:
    QString m_key;
    QString m_parentKey;
    QString m_value;
    bool m_deleted;
    bool m_added;
    bool m_modified;
};

#endif // NODEDATA_H
