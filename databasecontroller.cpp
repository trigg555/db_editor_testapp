#include "databasecontroller.h"
#include <QRandomGenerator>
#include <QtConcurrent/QtConcurrent>
#include "treeitem.h"


DataBaseController::DataBaseController(QObject *parent)
    : QObject(parent)
{
}

QString DataBaseController::getDataAsString()
{
    QFile file(":/default.txt");
    file.open(QIODevice::ReadOnly);
    const QString defaultData = QString::fromLatin1(file.readAll());
    file.close();

    return defaultData;
}

void DataBaseController::prepareModelData(TreeItem *rootItem, QHash<QString, TreeItem *> &registry)
{
    m_database.clear();
    const QStringList& lines = getDataAsString().split('\n');
    rootItem->setData(TreeItem::KeyRole, QVariant(getRootKey()));

    QVector<TreeItem*> parents;
    QVector<int> indentations;
    parents << rootItem;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            ++position;
        }

        const QString lineData = lines[number].trimmed();

        if (!lineData.isEmpty()) {
            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.
                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            const QString &key = generateKey();

            // Append a new item to the current parent's list of children.
            TreeItem *parent = parents.last();
            parent->insertChildren(parent->childCount(), 1);

            const QString &parentKey = parent->data(TreeItem::KeyRole).toString();

            NodeData nodeData(key, lineData, parentKey);
            TreeItem *item = parent->child(parent->childCount() - 1);
            item->setData(nodeData);
            registry.insert(key, item);

            // Append a new item to the database hash table
            m_database.insert(key, nodeData);
        }
        ++number;
    }
}

bool DataBaseController::isItemExists(const QString &key) const
{
    return m_database.contains(key);
}

NodeData DataBaseController::getItem(const QString &key) const
{
    return m_database.value(key);
}

QHash<QString, NodeData> *DataBaseController::getAllItems()
{
    return &m_database;
}

void DataBaseController::setChanges(const QHash<QString, NodeData> &changes)
{
    auto worker = [this](const QHash<QString, NodeData> &changes) {
        // Inserts changed items into database
        m_database.insert(changes);

        // If any element is deleted, its child elements must be deleted
        for (const NodeData &item : changes) {
            if (item.deleted())
                setChildrenDeleted(item);
        }
    };

    QFuture<void> future = QtConcurrent::run(worker, changes);
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();

    QObject::connect(watcher, &QFutureWatcher<void>::finished, [=]() {
        emit databaseUpdated();
        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

QString DataBaseController::generateKey() const
{
    QRandomGenerator randomGenerator(333);
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 16;

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
        int r = QRandomGenerator::global()->bounded(16384);
        int index = r % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

QString DataBaseController::getRootKey() const
{
    return QString("000000000000000");
}

void DataBaseController::setChildrenDeleted(const NodeData &parent)
{
    for (NodeData &item : m_database) {
        if (item.parentKey() == parent.key()) {
            item.setDeleted(true);
            setChildrenDeleted(item);
        }
    }
}
