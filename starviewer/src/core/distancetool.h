/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCETOOL_H
#define UDGDISTANCETOOL_H

#include "tool.h"
#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerLine;
class DrawerPrimitive;
class LineOutliner;
class DistanceToolRepresentation;

/**
    Eina per mesurar distàncies en un visor 2D.
    Ens permet fixar dos punts a la pantalla i calcular la distància que els separa.
    
    Per marcar cadascun dels punts es farà amb un clic o doble-clic amb el botó esquerre
    del ratolí.
    Un cop marcat el primer punt de la distància a calcular, es mostrarà una línia que unirà 
    el primer punt marcat amb el punt on es trobi el punter del mouse. Mentres l'usuari no
    hagi marcat el segon punt, no es mostrarà cap informació respecte a la distància en aquell moment.

    Un cop marcat el segon punt, la mesura es mostrarà en mil·límetres, si existeix informació 
    d'espaiat, en píxels altrament.

    Un cop acabada la mesura, es poden annotar successivament tantes mesures com es desitji.

    Les mesures annotades només apareixeran sobre les imatges sobre les que s'han realitzat, 
    però no s'esborren si canviem d'imatge. Si canviem d'imatge però tornem a mostrar-la més endavant
    les annotacions fetes sobre aquesta anteriorment, tornaran a aparèixer.

    Quan es canvïi l'input del visor, les annotacions fetes fins aquell moment s'esborraran.
    Quan es desactivi l'eina, les annotacions fetes fins aquell moment es mantindran.
*/
class DistanceTool : public Tool {
Q_OBJECT
public:
    DistanceTool(QViewer *viewer, QObject *parent = 0);
    ~DistanceTool();

    void handleEvent(long unsigned eventID);

private slots:
    ///Indica que l'outliner ha acabat de dibuixar una primitiva
    void outlinerFinished(DrawerPrimitive *primitive);

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Outliner que s'utilitza per a la línia
    LineOutliner *m_lineOutliner;

    /// ToolRepresentation que s'utilitza
    DistanceToolRepresentation *m_distanceToolRepresentation;

    /// Línia que es dibuixa
    QPointer<DrawerLine> m_line;

};
}

#endif
