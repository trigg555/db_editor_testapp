import QtQuick 2.2
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import QtQml.Models 2.2
import com.testapp.treeitem 1.0

ApplicationWindow {
    readonly property color backgroundBlue: "#dfe5f5"
    readonly property color borderBlue: "#2d527f"
    readonly property color buttonBackgroundBlue: "#8bb7de"
    readonly property color buttonBackgroundBluePressed: "#5180bc"

    function castToBool(val, defaultValue = false) {
        return val === undefined ? defaultValue : val;
    }

    function expandItem(view, item) {
        if(!item.isExpanded) {
            view.expand(item.index);
        }
    }

    visible: true
    width: 800
    height: 600
    title: qsTr("File System")

    Rectangle {
        anchors {
            fill: parent
        }
        color: "lightBlue"
        border.width: 2
        border.color: borderBlue
    }

    Rectangle {
        id: cachedTreeViewBorder
        anchors {
            top: parent.top
            bottom: buttonRow.top
            left: parent.left
            margins: 10
        }
        width: parent.width / 2 - 50
        border.width: 2
        border.color: borderBlue

        TreeView {
            id: cachedTreeView

            function acceptAllChanges() {
                 cachedTreeView.selection.clear();
            }

            anchors.fill: parent
            anchors.margins: 2
            headerVisible: false
            alternatingRowColors: false
            selectionMode: SelectionMode.SingleSelection
            model: cachedTreeModel
            selection: ItemSelectionModel {
                model: cachedTreeModel
            }

            TableViewColumn {
                id: cacheTableViewColumn
                role: "value"
                delegate: Row {
                    id: cacheRow

                    property bool itemAdded: castToBool(cachedTreeModel.data(styleData.index, TreeItem.AddedRole))
                    property bool itemDeleted: castToBool(cachedTreeModel.data(styleData.index, TreeItem.DeletedRole))
                    property bool itemModified: castToBool(cachedTreeModel.data(styleData.index, TreeItem.ModifiedRole))
                    property bool itemEditable: castToBool(cachedTreeModel.data(styleData.index, TreeItem.EditableRole))
                    property bool itemPossiblyDeleted: castToBool(cachedTreeModel.data(styleData.index, TreeItem.PossiblyDeletedRole))
                    property bool itemSelected: styleData.selected

                    onItemEditableChanged: {
                        edit.enabled = itemEditable;
                    }

                    onItemSelectedChanged: {
                        if (!itemSelected  && cacheRow.itemEditable) {
                            // Save value and set item not editable
                            cachedTreeModel.setData(styleData.index, edit.text, TreeItem.ValueRole);
                        }
                        edit.enabled = false;
                    }

                    spacing: 10
                    Rectangle {
                        implicitWidth: edit.contentWidth + edit.horizontalMargin * 2
                        implicitHeight: edit.contentHeight
                        color: edit.enabled ? "white" : "transparent"
                        border {
                            width: 1
                            color: edit.enabled ? "blue" : "transparent"
                        }

                    TextInput {
                        id: edit
                        readonly property int horizontalMargin: 5
                        anchors {
                            left: parent.left
                            top: parent.top
                            leftMargin: horizontalMargin
                        }

                        text: styleData.value
                        color: { if (cacheRow.itemDeleted)
                                     return "red";
                                 else if (cacheRow.itemAdded)
                                     return "green";
                                 else
                                     return "black";
                        }
                        font.strikeout: cacheRow.itemDeleted
                        font.italic: cacheRow.itemModified
                        enabled: false
                        onEnabledChanged: {
                            if (enabled)
                                forceActiveFocus();
                        }

                        onAccepted: {
                            // onItemSelectedChanged will be called
                            cachedTreeView.acceptAllChanges();
                        }

                        onTextChanged: {
                            // Allows to expand all the items correctly
                            if (!edit.enabled && text.length) {
                                cachedTreeViewExpandTimer.start();
                            }
                        }
                    }
                    }

                    Text {
                        id: changedLabel
                        text: qsTr("changed")
                        visible: cacheRow.itemModified
                        color: "darkblue"
                        font.pixelSize: 10
                    }

                    Text {
                        id: possiblyDeletedLabel
                        text: qsTr("may have been deleted, please reload")
                        visible: cacheRow.itemPossiblyDeleted
                        color: "red"
                        font.pixelSize: 12
                    }

                    Timer {
                        id: cachedTreeViewExpandTimer
                        interval: 10;
                        onTriggered: expandItem(cachedTreeView, styleData)
                    }

                    Component.onCompleted: {
                        expandItem(cachedTreeView, styleData);
                    }
                } // Row
            } // TableViewColumn
        } // TreeView
    } // Rectangle

    Rectangle {
        id: dbTreeViewBorder
        anchors {
            top: parent.top
            bottom: buttonRow.top
            right: parent.right
            margins: 10
        }
        width: parent.width / 2 - 50
        border.width: 2
        border.color: borderBlue

        TreeView {
            id: dbTreeView
            anchors.fill: parent
            anchors.margins: 2
            headerVisible: false
            alternatingRowColors: false
            selectionMode: SelectionMode.SingleSelection
            model: dbTreeModel
            selection: ItemSelectionModel {
                model: dbTreeModel
            }

            TableViewColumn {
                role: "value"
                delegate: Row {
                    id: dbRow
                    property bool itemAdded: castToBool(dbTreeModel.data(styleData.index, TreeItem.AddedRole))
                    property bool itemDeleted: castToBool(dbTreeModel.data(styleData.index, TreeItem.DeletedRole))
                    property bool itemModified: castToBool(dbTreeModel.data(styleData.index, TreeItem.ModifiedRole))
                    spacing: 10

                    Text {
                        id: dbValue
                        text: styleData.value
                        color: { if (dbRow.itemDeleted)
                                     return "red";
                                 else if (dbRow.itemAdded)
                                     return "green";
                                 else
                                     return "black";
                        }
                        font.strikeout: dbRow.itemDeleted

                        onTextChanged: {
                            if (text.length) {
                                dbViewExpandTimer.start();
                            }
                        }
                    }

                    Timer {
                        id: dbViewExpandTimer
                        interval: 10;
                        onTriggered: expandItem(dbTreeView, styleData)
                    }

                    Component.onCompleted: {
                        expandItem(dbTreeView, styleData);
                    }
                } // Row
            } // TableViewColumn
        } // TreeView
    } // Rectangle

    Button {
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 100
        }
        width: 60
        id: fetchButton
        text: "<<<"
        style: ButtonStyle {
            background: Rectangle {
                implicitWidth: 60
                implicitHeight: 25
                color: control.pressed ? buttonBackgroundBluePressed : buttonBackgroundBlue
                border.width: 2
                border.color: borderBlue
                radius: 5
            }
        }
        onClicked: {
            cachedTreeView.acceptAllChanges();
            cachedTreeModel.fetchNode(dbTreeView.selection.model.data(dbTreeView.selection.currentIndex, TreeItem.KeyRole));
            dbTreeView.selection.clear();
        }
    }

    Row {
        id: buttonRow
        anchors {
            bottom: parent.bottom
            bottomMargin: 30
            left: parent.left
            leftMargin: 10
        }
        spacing: 20

        Row {
            id: itemActionRow
            spacing: 4

            Repeater {
                model: [ "+", "-", "a"]
                Button {
                    text: modelData
                    style: ButtonStyle {
                        background: Rectangle {
                            implicitWidth: 25
                            implicitHeight: 25
                            color: control.pressed ? buttonBackgroundBluePressed : buttonBackgroundBlue
                            border.width: 2
                            border.color: borderBlue
                            radius: 5
                        }
                    }
                    onClicked: {
                        switch(model.index) {
                        case 0:
                            cachedTreeView.acceptAllChanges();
                            cachedTreeModel.addNode(cachedTreeView.currentIndex);
                            break;
                        case 1:
                            cachedTreeView.acceptAllChanges();
                            cachedTreeModel.deleteNode(cachedTreeView.currentIndex);
                            break;
                        case 2:
                            cachedTreeModel.editNode(cachedTreeView.currentIndex);
                            break;
                        }
                    }
                } // Button
            } // Repeater
        } // Row

        Row {
            id: cacheActionRow
            spacing: 4

            Repeater {
                model: ["Apply", "Reset"]
                Button {
                    text: modelData
                    style: ButtonStyle {
                        background: Rectangle {
                            implicitWidth: 60
                            implicitHeight: 25
                            color: control.pressed ? buttonBackgroundBluePressed : buttonBackgroundBlue
                            border.width: 2
                            border.color: borderBlue
                            radius: 5
                        }
                    }
                    onClicked: {
                        cachedTreeView.acceptAllChanges();
                        switch(model.index) {
                        case 0:
                            cachedTreeModel.applyChanges();
                            break;
                        case 1:
                            cachedTreeModel.reset();
                            dbTreeModel.reset();
                            break;
                        }
                    }
                }
            }
        } // Row
    } // Row
} // ApplicationWindow
