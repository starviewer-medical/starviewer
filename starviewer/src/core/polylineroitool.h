/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINEROITOOL_H
#define UDGPOLYLINEROITOOL_H

#include "roitool.h"

namespace udg {
/**
Tool per calcular l'àrea i la mitjana de grisos d'un polígon fet amb ROITool

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PolylineROITool : public ROITool
{
Q_OBJECT
public:
    PolylineROITool( QViewer *viewer, QObject *parent = 0 );

    ~PolylineROITool();

private slots:
    /// fa els càlculs pertinents de la tool després d'haver rebut un signal de figura acabada
    void initCalc();

private:
    /// metode per escriure a pantalla les dades calculades.
    void printData();

    /// metode per calcular la mitjana de gris de la regio del polyline
    double computeGrayMean();

    /// calcula l'area del poligon que defineix la ROI
    double computeArea( const double * spacing = NULL );
};

}

#endif
