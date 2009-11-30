/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQPREVIOUSSTUDIESWIDGET_H
#define UDGQPREVIOUSSTUDIESWIDGET_H

#include <QWidget>
#include <QTreeWidget>

namespace udg {

class Study;
class PreviousStudiesManager;

class QPreviousStudiesWidget : public QFrame
{
Q_OBJECT
public:
    QPreviousStudiesWidget(Study * inputStudy, QWidget * parent = 0 );
    ~QPreviousStudiesWidget();

private:
    void createConnections();
    void initializeTree();
    void initializeLookinForStudiesWidget();
    void insertStudyToTree(Study * study = 0);
    void updateWidthTree();

private slots:
    void insertStudiesToTree(  QList<Study*> studiesList , QHash<QString, QString> hashPacsIDOfStudyInstanceUID );
    void clicked();

private:
    QTreeWidget * m_previousStudiesTree;
    QWidget * m_lookingForStudiesWidget;
    PreviousStudiesManager * m_previousStudiesManager;
};

}
#endif // UDGQPREVIOUSSTUDIESWIDGET_H
