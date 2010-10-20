/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGNONCLOSEDANGLETOOL_H
#define UDGNONCLOSEDANGLETOOL_H

#include "tool.h"
#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerPrimitive;
class DrawerLine;
class NonClosedAngleToolRepresentation;
class LineOutliner;

/**
    Tool per calcular angles oberts
*/
class NonClosedAngleTool : public Tool {
Q_OBJECT
public:
    NonClosedAngleTool(QViewer *viewer, QObject *parent = 0);
    ~NonClosedAngleTool();

    void handleEvent(long unsigned eventID);

private slots:
    /// Indica que l'outliner ha acabat de dibuixar una primitiva
    void outlinerFinished(DrawerPrimitive *primitive);

private:
    /// Possibles estats de la tool
    enum { None, FirstLineFixed, SecondLineFixed };
    
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Outliner que s'utilitza per pintar les dues línies de l'angle obert
    LineOutliner *m_lineOutliner;

    /// ToolRepresentation que s'utilitza
    NonClosedAngleToolRepresentation *m_nonClosedToolRepresentation;

    /// Línies que dibuixem amb l'outliner i donem a la representació
    QPointer<DrawerLine> m_firstLine;
    QPointer<DrawerLine> m_secondLine;
};

}

#endif
