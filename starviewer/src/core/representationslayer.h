/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef REPRESENTATIONSLAYER_H
#define REPRESENTATIONSLAYER_H

#include <QObject>

#include "toolrepresentation.h"

#include <QMultiMap>

namespace udg {

class Q2DViewer;

/**
Classe contenidor de ToolRepresentation

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class RepresentationsLayer : public QObject
{
Q_OBJECT
public:
    RepresentationsLayer( Q2DViewer *viewer, QObject *parent = 0 );
    ~RepresentationsLayer();

    ///Afegeix una representacio a la llista.
    void addRepresentation(ToolRepresentation *toolRepresentation);

    /**
    * Esborra totes les primitives que es veuen al visor, és a dir, en el pla i llesques actuals.
    */
    void clearViewer();

    ///Esboora totes les toolRepresentation's del viewer
    void clearAll();

    /**
    * Rep events des del ToolProxy
    * @param eventID tipus d'event
    * @param posX posició X de l'event
    * @param posY posició Y de l'event
    */
    void handleEvent(unsigned int eventID, double posX, double posY);

public slots:
    ///Actualitza les variables de plane i slice
    void refresh();

private:
    /// Viewer al qual pertany la RepresentationsLayer
    Q2DViewer *m_2DViewer;

    /// Pla i llesca en el que es troba en aquell moment el 2D Viewer. Serveix per controlar
    /// els canvis de llesca i de pla, per saber quines primitives hem de netejar
    int m_currentPlane;
    int m_currentSlice;

    ///Contenidors de ToolRepresentation, un per cada pla
    QMultiMap< int, ToolRepresentation *> m_axialToolRepresentations;
    QMultiMap< int, ToolRepresentation *> m_sagitalToolRepresentations;
    QMultiMap< int, ToolRepresentation *> m_coronalToolRepresentations;

};

}

#endif
