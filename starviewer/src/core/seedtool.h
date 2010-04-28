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
class SeedToolData;

/**
    Eina que serveix per posar llavors en un visor 2D

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SeedTool : public Tool {
Q_OBJECT
public:
    /// Estats de la tool
    enum { None , Seeding };

    SeedTool(QViewer *viewer, QObject *parent = 0 );
    ~SeedTool();

    void handleEvent( long unsigned eventID );

    /// Retorna les dades pròpies de la seed
    ToolData *getToolData() const;

    /// Assigna les dades pròpies de la seed (persistent data)
    void setToolData(ToolData * data);

    /// Posem la llavor i li passem la posició i la llesca: útil per fer-ho per codi
    void setSeed(QVector<double> seedPosition, int slice);

signals:
    void seedChanged(double, double, double);

private:
    /// Posem la llavor
    void setSeed();

    /// Calcula la llesca que s'ha de moure
    void doSeeding();

    /// Atura l'estat de seeding
    void endSeeding();

    /// Actualitzem la posició de la llavor
    void updateSeedPosition();

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Estat de la tool
    int m_state;

    /// Dades específiques de la tool
    SeedToolData *m_myData;

    /// Bool per saber si està pintada o no (si l'hem enviat al drawer)
    bool m_drawn;

};


}


#endif
