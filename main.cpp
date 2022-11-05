#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "databasecontroller.h"
#include "cachedtreemodel.h"
#include "dbtreemodel.h"
#include "treeitem.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<TreeModel>("", 1, 0, "TreeModel");
    qmlRegisterType<TreeItem>("", 1, 0, "TreeItem");
    qmlRegisterUncreatableType<TreeItem>("com.testapp.treeitem", 1, 0, "TreeItem", "TreeItem can't be created by qml");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    DataBaseController *dbController = new DataBaseController(&engine);

    CachedTreeModel *cm = new CachedTreeModel(dbController, dbController);
    DbTreeModel *dbm = new DbTreeModel(dbController, dbController);

    engine.rootContext()->setContextProperty("cachedTreeModel", cm);
    engine.rootContext()->setContextProperty("dbTreeModel", dbm);
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
