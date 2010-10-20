/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCETOOLREPRESENTATION_H
#define UDGDISTANCETOOLREPRESENTATION_H

#include "toolrepresentation.h"

#include <QPointer>

namespace udg {

class DrawerLine;
class DrawerText;

/**
    Eina de distancies. (Càlcul)
*/
class DistanceToolRepresentation : public ToolRepresentation {
Q_OBJECT
public:
    DistanceToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~DistanceToolRepresentation();

    /// Se li passa la linia ja dibuixada per un outliner
    void setLine(DrawerLine *line);

    /// Se li passen els parametres necessaris pel calcul
    void setParams(int view, const double *pixelSpacing, double *spacing);

    /// Calcula la distancia i escriu el text
    void calculate();

private:
    /// Crea els handlers
    void createHandlers();

    /// Mapeja els handlers amb els punts corresponents
    QList<double *> mapHandlerToPoint(ToolHandler *toolHandler);

private slots:
    /// Edita els vèrtexs de les DrawerPrimitive
    void moveAllPoints(double *movement);

private:
    /// Línia que es dibuixa
    QPointer<DrawerLine> m_line;

    ///Text amb el resultat del càlcul
    QPointer<DrawerText> m_text;

    ///Parametres necessaris per la representacio
    int m_view;
    int m_slice;
    const double *m_pixelSpacing;
    double *m_spacing;
};

}

#endif
