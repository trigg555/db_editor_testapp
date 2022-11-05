#include "nodedata.h"

NodeData::NodeData()
    : m_deleted(false)
    , m_added(false)
    , m_modified(false)
{
}

NodeData::NodeData(const QString &key, const QString &value, const QString &parentKey, bool deleted, bool added)
    : m_key(key)
    , m_parentKey(parentKey)
    , m_value(value)
    , m_deleted(deleted)
    , m_added(added)
    , m_modified(false)
{
}

NodeData::~NodeData()
{
}

QString NodeData::key() const
{
    return m_key;
}

QString NodeData::parentKey() const
{
    return m_parentKey;
}

QString NodeData::value() const
{
    return m_value;
}

bool NodeData::deleted() const
{
    return m_deleted;
}

bool NodeData::added() const
{
    return m_added;
}

bool NodeData::modified() const
{
    return m_modified;
}

void NodeData::setKey(const QString &val)
{
    m_key = val;
}

void NodeData::setParentKey(const QString &val)
{
    m_parentKey = val;
}

void NodeData::setValue(const QString &val)
{
    m_value = val;
}

void NodeData::setDeleted(const bool &val)
{
    m_deleted = val;
}

void NodeData::setAdded(const bool &val)
{
    m_added = val;
}

void NodeData::setModified(const bool &val)
{
    m_modified = val;
}
