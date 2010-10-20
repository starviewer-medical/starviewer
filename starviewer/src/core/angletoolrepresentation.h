/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGANGLETOOLREPRESENTATION_H
#define UDGANGLETOOLREPRESENTATION_H

#include "toolrepresentation.h"

#include <QPointer>

namespace udg {

class DrawerPolyline;
class DrawerText;

/**
    Representació de l'eina d'angles. (Càlcul)
*/
class AngleToolRepresentation : public ToolRepresentation {
Q_OBJECT
public:
    AngleToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~AngleToolRepresentation();

    /// Se li passa la linia ja dibuixada per un outliner
    void setPolyline(DrawerPolyline *line);

    /// Se li passen els parametres necessaris pel calcul
    void setParams(int view, int slice, double degrees);

    /// Calcula la distancia i escriu el text
    void calculate();

private:
    /// Re-calcula l'angle després de l'edició
    void recalculateAngle();

    /// Calcula la correcta posició del caption de l'angle segons els punts de l'angle
    void placeText();

    /// Crea els handlers
    void createHandlers();

    /// Mapeja els handlers amb els punts corresponents
    QList<double *> mapHandlerToPoint(ToolHandler *toolHandler);

private slots:
    /// Edita els vèrtexs de les DrawerPrimitive
    void moveAllPoints(double *movement);

private:
    /// Línia que es dibuixa
    QPointer<DrawerPolyline> m_polyline;

    /// Text amb el resultat del càlcul
    QPointer<DrawerText> m_text;

    /// Paràmetres necessaris per la representació
    int m_view;
    int m_slice;

    /// Angle que formen en tot moment els dos segments
    double m_currentAngle;

    /// Indica si la Tool ha estat editada
    bool m_edited;
};

}

#endif
