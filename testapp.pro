TEMPLATE = app
#TARGET =
QT += qml quick widgets

SOURCES += main.cpp \
    cachedtreemodel.cpp \
    databasecontroller.cpp \
    dbtreemodel.cpp \
    nodedata.cpp \
    treeitem.cpp \
    treemodel.cpp

RESOURCES += qml.qrc \
    data.qrc

# Installation path
# target.path =

INSTALLS += target

HEADERS += \
    cachedtreemodel.h \
    databasecontroller.h \
    dbtreemodel.h \
    nodedata.h \
    treeitem.h \
    treemodel.h
