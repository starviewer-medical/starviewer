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
class PolylineROITool : public ROITool {
Q_OBJECT
public:
    PolylineROITool( QViewer *viewer, QObject *parent = 0 );
    ~PolylineROITool();

private slots:
    /// Fa els càlculs pertinents de la tool després d'haver rebut un signal de figura acabada
    void start();

private:
    /// Mètode per escriure a pantalla les dades calculades.
    void printData();

    /// Calcula les dades estadístiques de la ROI. 
    /// Serà necessari cridar aquest mètode abans si volem obtenir la mitjana i/o la desviació estàndar
    void computeStatisticsData();

    /// Llista amb els valors de gris per calcular la mitjana i la desviació estàndard i altres dades estadístiques si cal.
    QList< double > m_grayValues;

    /// Mitjana de valors de la ROI
    double m_mean;

    /// Desviació estàndar de la ROI
    double m_standardDeviation;
};

}

#endif
