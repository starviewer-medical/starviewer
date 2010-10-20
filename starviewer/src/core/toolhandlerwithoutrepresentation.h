/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLHANDLERWITHOUTREPRESENTATION_H
#define UDGTOOLHANDLERWITHOUTREPRESENTATION_H

#include "toolhandler.h"

namespace udg {
    
class DrawerPrimitive;

/**
    Handlers per figures. No tenen representació.
*/
class ToolHandlerWithoutRepresentation : public ToolHandler {
Q_OBJECT
public:    
    ToolHandlerWithoutRepresentation(QList<DrawerPrimitive *> primitives, QObject *parent = 0);
    ~ToolHandlerWithoutRepresentation();

    void update();

public slots:
    void handleEvents(long unsigned eventID, double *point);

    void isClickNearMe(double *point);

private:
    /// Retorna el moviment entre el punt d'origen i el final
    void getMovement(double *finalPoint, double *movementVector);

private:
    /// Llista amb les primitives d'on ha de calcular la posició
    QList<DrawerPrimitive *> m_primitives;

    /// Punt que fa d'origen per calcular el desplaçament
    double m_startPoint[3];
    
};

}

#endif
