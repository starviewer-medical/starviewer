/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSEEDTOOL_H
#define UDGSEEDTOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class DrawerPoint;
class SeedToolData;

/**
Eina que serveix per posar llavors en un visor 2D

  @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SeedTool : public Tool
{
Q_OBJECT
public:
    /// estats de la tool
    enum { NONE , SEEDING };

    SeedTool(QViewer *viewer, QObject *parent = 0 );

    ~SeedTool();

    ///funcio manejadora dels events passats.
    void handleEvent( long unsigned eventID );

private:
    /// Posem la llavor
    void setSeed( );

    /// Calcula la llesca que s'ha de moure
    void doSeeding();

    /// Atura l'estat de seeding
    void endSeeding();

    /// Retorna les dades pròpies de la seed
    ToolData *getToolData() const;

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Punt que es dibuixa
    DrawerPoint *m_point;

    ///estat de la tool
    int m_state;

    /// Dades específiques de la tool
    SeedToolData *m_myData;

};


}


#endif
