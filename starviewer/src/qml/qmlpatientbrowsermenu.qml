/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

import QtQuick 2.3

Rectangle {
    id: browserMenu

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    property int columns: 2
    property string markedItem: ""
    property int computedContentWidth: calcContentWidth(browserModel, browserMenu)
    property int computedContentHeight: calcContentHeight(browserModel, browserMenu)
    property int maxHeight: 1500000
    property int maxWidth: computedContentWidth + listview.anchors.leftMargin + listview.anchors.rightMargin;
    property int scrollheight
    property string fusionMarkedItem1: "-1"
    property string fusionMarkedItem2: "-1"
    property string fusionLabelText: qsTr("Fusion")

    width: Math.min(computedContentWidth + listview.anchors.leftMargin + listview.anchors.rightMargin, maxWidth);
    height: Math.min(computedContentHeight + listview.anchors.topMargin + listview.anchors.bottomMargin, maxHeight);
    color: myPalette.window

    signal selectedItem(string identifier)
    signal isActive(string identifier)
    signal sizeChanged()

    function calcContentWidth(model, parent)
    {
        var max = 0;
        for (var i = 0; i < model.length; ++i)
        {
            var group = model[i].elements
            for (var j = 0; j < group.length; ++j)
            {
                var item = group[j];
                var textElement = Qt.createQmlObject(
                        'import QtQuick 2.3;'
                        + 'Text {'
                        + '   text: "' + item.text.replace(/"/g, '\\"') + '"; visible: false; font.bold: ' + (browserMenu.markedItem === item.identifier) + '; font.pointSize: ' + applicationFontSize
                        + '}',
                        parent, "calcContentWidth")
                max = Math.max((textElement.width + 15) * 2, max)
                textElement.destroy()
            }
            var captionElement = Qt.createQmlObject(
                    'import QtQuick 2.3;'
                    + 'Text {'
                        + '   text: "' + model[i].caption.replace(/"/g, '\\"') + '"; visible: false; font.pointSize: ' + applicationFontSize
                    + '}',
                    parent, "calcContentWidth")
            max = Math.max(captionElement.width + 20, max)
        }
        return max;
    }


    function calcContentHeight(model, parent)
    {
        var textElement = Qt.createQmlObject(
                'import QtQuick 2.3;'
                + 'Text {'
                + '   text: "any text"; visible: false; font.pointSize: ' + applicationFontSize
                + '}',
                parent, "calcContentHeight")
        var cellHeight = textElement.height + textElement.font.pointSize;

        var height = 0;

        for (var i = 0; i < model.length; ++i) {
            var currentModelElement = model[i]
            var numOfRows = Math.ceil(currentModelElement.elements.length / 2) + currentModelElement.fusionElements.length;
            height += numOfRows * cellHeight;
            height += cellHeight; // Caption
            height += 4; // Margin between caption and grid

            if (currentModelElement.fusionElements.length > 0) {
                height += 5; // Margin between series and fusion series
            }
        }

        height += 10 * (model.length - 1) // Margin between studies

        return height;
    }

    ListView {
        id: listview
        anchors.fill: parent
        anchors.topMargin: 3
        anchors.leftMargin: 3
        anchors.bottomMargin: 3
        anchors.rightMargin: 12
        spacing: 10
        focus: true
        model: browserModel
        delegate: listDelegate
        boundsBehavior: Flickable.StopAtBounds
        section.property: "caption"
        section.criteria: ViewSection.FullString
    }

    Component {
        id: listDelegate

        Item {
            id: seriesGroup
            width: parent.width
            height: childrenRect.height

            Rectangle {
                id: header
                width: parent.width
                height: headerText.height + headerText.font.pointSize - 2 // 2 -> border-top
                radius: 4
                color: "lightsteelblue"
                y: 0

                border {
                    color: "steelblue"
                    width: 2
                }
                Text {
                    id: headerText
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: parent.right
                        leftMargin: 10
                        rightMargin: 10
                    }
                    horizontalAlignment: Text.AlignHCenter
                    text: caption
                    elide: Text.ElideMiddle
                    font.pointSize: applicationFontSize
                }
            }

            GridView {
                id: seriesItems
                model: modelData.elements
                delegate: seriesItemsDelegate
                cellWidth: parent.width / 2
                cellHeight: headerText.height + headerText.font.pointSize

                y: header.height + 5
                height: cellHeight * (Math.round(elements.length / columns))
                width: parent.width
                interactive: false
                flow: GridView.TopToBottom

                Component {
                    id: seriesItemsDelegate

                    Rectangle {
                        property bool hovered: false;

                        width: seriesItems.cellWidth - 4
                        height: seriesItems.cellHeight - 4
                        radius: 4
                        border.color: "darkGray"
                        color: {
                            if (hovered)
                            {
                                return "lightsteelblue"
                            }
                            else if (browserMenu.fusionMarkedItem1 === modelData.identifier || browserMenu.fusionMarkedItem2 === modelData.identifier)
                            {
                                return "#8fd990"
                            }
                            else
                            {
                                return myPalette.window;
                            }
                        }

                        Text {
                            width: parent.width
                            anchors {
                                left: parent.left
                                verticalCenter: parent.verticalCenter
                                leftMargin: 5
                            }
                            text: modelData.text
                            elide: Text.ElideMiddle
                            font.pointSize: applicationFontSize
                            font.bold: {
                                if (browserMenu.markedItem === modelData.identifier) {
                                    return true;
                                }
                                else if (browserMenu.markedItem.indexOf("+") > -1) {
                                    var fusionIDItems = browserMenu.markedItem.split("+");
                                    return fusionIDItems[0] === modelData.identifier || fusionIDItems[1] === modelData.identifier;
                                }
                                else {
                                    return false;
                                }
                            }
                            font.italic: {
                                if (browserMenu.markedItem.indexOf("+") > -1) {
                                    var fusionIDItems = browserMenu.markedItem.split("+");
                                    return fusionIDItems[0] === modelData.identifier || fusionIDItems[1] === modelData.identifier;
                                }
                                else {
                                    return false;
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                browserMenu.isActive(modelData.identifier);
                                parent.hovered = true;
                            }
                            onExited: parent.hovered = false;
                            onClicked: browserMenu.selectedItem(modelData.identifier)
                        }
                    }
                }
            }

            ListView {
                id: fusionItems
                model: modelData.fusionElements
                delegate: fusionItemDelegate

                anchors.top: seriesItems.bottom
                anchors.topMargin: modelData.fusionElements.length > 0? 5: 0;
                width: parent.width
                height: seriesItems.cellHeight * modelData.fusionElements.length
                interactive: false

                Component {
                    id: fusionItemDelegate

                    Item {
                        width: parent.width
                        height: seriesItems.cellHeight

                        Rectangle {
                            id: fusionTitle
                            color: "#8fd990"
                            height: seriesItems.cellHeight - 4
                            width: childrenRect.width
                            radius: 4

                            Text {
                                font.pointSize: applicationFontSize
                                text: " " + fusionLabelText + " "
                                color: "#012911"
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }
                            }
                        }

                        Rectangle {
                            anchors {
                                left: fusionTitle.right
                                right: parent.right
                                leftMargin: 5
                            }
                            height: seriesItems.cellHeight - 4
                            radius: 4
                            border.color: "darkGray"
                            color: myPalette.window

                            Text {
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    verticalCenter: parent.verticalCenter
                                    leftMargin: 5
                                    rightMargin: 5
                                }
                                text: modelData.text
                                elide: Text.ElideMiddle
                                font.bold: (browserMenu.markedItem === modelData.identifier)
                                font.pointSize: applicationFontSize
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    browserMenu.isActive(modelData.identifier);
                                    parent.color = "lightsteelblue"
                                    var fusionIDItems = modelData.identifier.split("+");
                                    browserMenu.fusionMarkedItem1 = fusionIDItems[0];
                                    browserMenu.fusionMarkedItem2 = fusionIDItems[1];
                                }
                                onExited: {
                                    parent.color = "transparent"
                                    browserMenu.fusionMarkedItem1 = "-1"
                                    browserMenu.fusionMarkedItem2 = "-1"
                                }
                                onClicked: browserMenu.selectedItem(modelData.identifier)
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: scrollbar
        anchors.right: browserMenu.right
        width: 8
        color: "black"
        radius: 3
        y: listview.visibleArea.yPosition * listview.height + listview.anchors.topMargin
        height:  listview.visibleArea.heightRatio * listview.height;
        opacity: browserMenu.height + 10 < browserMenu.maxHeight? 0: 0.4
    }

    onHeightChanged: {
        browserMenu.sizeChanged()
    }
}
